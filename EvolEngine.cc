/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include "EvolEngine.h"

#include <sys/time.h>
#include <unistd.h>

#include <cstring>
#include <memory>

#include "Action.h"
#include "Coord.h"
#include "Lifeform.h"
#include "Random.h"
#include "Timer.h"
#include "Types.h"

namespace evol {


void EvolEngine::Seed(unsigned num_lifeforms) {
  for (unsigned i = 0; i < num_lifeforms; i++) {
    for (;;) {
      Coord c = arena_->GetRandomCoordOnArena();
      Lifeform * lf(new Lifeform(0, Dna {OpCode::FINAL_MOVE_RANDOM}));
      arena_->AddAndOwnLifeform(lf, c);
      break;
    }
  }
}


void EvolEngine::Run() {
  Timer loop_timer("Main loop");

  {
    std::lock_guard<std::mutex> lg(volatile_mutex_);
    timers_.assign({&loop_timer});
  }

  while (!do_exit_) {
    std::unique_lock<std::mutex> vl(volatile_mutex_, std::defer_lock);

    // Start main loop timer
    loop_timer.StartCollection();

    // Run each Lifeform's Dna and get its resulting action.  These actions
    // make no change to the arena and will be resolved later in the loop
    std::forward_list<Action> actions;
    for (auto & lf : arena_->Lifeforms()) {
      actions.push_front(Action(lf, lf->RunDna(arena_.get())));
    }

    // Map and resolve all actions
    std::unique_ptr<ActionMap> interactions(MapActions(actions));

    // Time to update the arena and birth/kill lifeforms; take the main lock
    vl.lock();

    ResolveInteractions(interactions.get());

    // Handle energy and replication
    ApplyEnergyLevelsToLifeforms();

    // Handle birth & death
    KillStarvedLifeforms();
    SplitFatLifeforms();

    // Blast a lifeform off into outer space!  (Actually another engine)
    if (Params::kLifeformAsteroidLaunchInterval != 0 && turns_ % Params::kLifeformAsteroidLaunchInterval == 0) {
      asteroid_->LaunchLifeform(arena_->RemoveRandomLifeform());
    }

    // Get a lifeform from outer space!  (Actually another engine)
    if (Params::kLifeformAsteroidLandInterval != 0 && turns_ % Params::kLifeformAsteroidLandInterval == 0) {
      std::unique_ptr<Lifeform> lf = asteroid_->LandLifeform();
      if (lf) {
        Coord c(arena_->GetRandomCoordOnArena());
        arena_->AddAndOwnLifeform(lf.release(), c);
      }
    }

    // End main loop timer
    loop_timer.EndCollection();

    vl.unlock();  // No more changes to arena & lifeforms during this loop iteration
    ++turns_;
  }

  // Clear out the timer export before they fall out of scope
  timers_.clear();
}


std::unique_ptr<ActionMap>
EvolEngine::MapActions(const std::forward_list<Action> & actions) const {
  std::unique_ptr<ActionMap> interactions(new ActionMap());
  for (auto & act : actions) {
    // We place each action into a map of coords -> actions for later
    // resolution
    Coord dest;
    switch (act.type) {
      case ActionType::NOTHING:
        dest = act.actor->GetCoord();
        break;
      case ActionType::APOPTOSIS:
        dest = act.actor->GetCoord();
        break;
      case ActionType::MOVE_NORTH:
        dest = act.actor->GetCoord().North();
        break;
      case ActionType::MOVE_SOUTH:
        dest = act.actor->GetCoord().South();
        break;
      case ActionType::MOVE_EAST:
        dest = act.actor->GetCoord().East();
        break;
      case ActionType::MOVE_WEST:
        dest = act.actor->GetCoord().West();
        break;
    }
    (*interactions)[dest].emplace_back(act);
  }
  return interactions;
}


/**
 * Commit the resolved interactions of each lifeform.
 */
void EvolEngine::ResolveInteractions(ActionMap * interactions) {
  for (auto & elem : *interactions) {
    auto & coord = elem.first;
    auto & actions = elem.second;
    for (auto & act: actions) {
      if (act.type == ActionType::APOPTOSIS) {
        arena_->KillLifeform(act.actor);
      } else if (act.type != ActionType::NOTHING) {
        arena_->MoveLifeform(act.actor, coord);
      }
    }
  }
}


/**
 * To calculate energy available to lifeforms, we iterate the entire arena.  For
 * each UNOCCUPIED square, energy is split evenly between adjacent lifeforms
 * (adjacency meaning being in a 9x9 grid around the square).  For each OCCUPIED
 * square, energy is split only among the occupants.
 *
 * Finally, every lifeform loses kCostOfLiving energy per turn.
 */
void EvolEngine::ApplyEnergyLevelsToLifeforms() {
  Coord c;
  int width = arena_->Width();
  int height = arena_->Height();

  for (c.x = 0; c.x < width; c.x++) {
    for (c.y = 0; c.y < height; c.y++) {
      auto occupants = arena_->GetLifeforms(c);
      float available_energy = arena_->GetEnergy(c);
      float energy_share_per_lf = 0.0;

      if (!occupants.empty()) {
        energy_share_per_lf = available_energy / occupants.size();
        for (auto & lf : occupants) {
          lf->SetEnergy(lf->GetEnergy() + energy_share_per_lf);
        }
      } else {
        // Split all of empty square's energy between adjacent occupants
        auto adjacent = arena_->GetAdjacentLifeforms(c);
        if (adjacent.size() < 1) {
          continue;
        }
        energy_share_per_lf = available_energy / adjacent.size();
        for (auto & lf : adjacent) {
          lf->SetEnergy(lf->GetEnergy() + energy_share_per_lf);
        }
      }
    }
  }

  // Deduct cost of living
  for (auto & lf : arena_->Lifeforms()) {
    lf->SetEnergy(lf->GetEnergy() - Params::kCostOfLiving - Params::kCostOfOpcode * lf->GetDnaSize());
  }
}


void EvolEngine::KillStarvedLifeforms() {
  for (auto & lf : arena_->Lifeforms()) {
    if (lf->GetEnergy() <= 0.0) {
      arena_->KillLifeform(lf);
    }
  }
}


void EvolEngine::SplitFatLifeforms() {
  for (auto & lf : arena_->Lifeforms()) {
    float parent_energy = lf->GetEnergy();
    if (parent_energy >= Params::kMeiosisLevel) {
      Lifeform * baby(lf->MakeChild());
      baby->Mutate();
      parent_energy -= Params::kMeiosisCost;
      baby->SetEnergy(parent_energy / 2.0);
      lf->SetEnergy(parent_energy / 2.0);
      arena_->AddAndOwnLifeform(baby, lf->GetCoord());
    }
  }
}


}  // namespace evol
