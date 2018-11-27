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

#include "Coord.h"
#include "Lifeform.h"
#include "Types.h"

namespace evol {


void EvolEngine::Seed(unsigned num_lifeforms) {
  for (unsigned i = 0; i < num_lifeforms; i++) {
    for (;;) {
      Coord c = arena_->GetRandomCoordOnArena();
      arena_->AddLifeform(make_lifeform(0, Dna {OpCode::FINAL_MOVE_RANDOM}, random_), c);
      break;
    }
  }
}


void EvolEngine::Run() {
  auto loop_timer = std::make_shared<Timer>("Main loop");

  {
    std::lock_guard<std::mutex> lg(mutex_);
    timers_.push_front(loop_timer);
  }

  while (!do_exit_) {
    std::unique_lock<std::mutex> vl(mutex_, std::defer_lock);

    // Start main loop timer
    loop_timer->StartCollection();

    // Run each Lifeform's Dna and get its resulting action.  These actions
    // make no change to the arena and will be resolved later in the loop
    std::forward_list<Action> actions;
    for (auto & lf : arena_->Lifeforms()) {
      actions.push_front(Action(lf, lf->RunDna(arena_.get())));
    }

    // Map and resolve all actions
    ActionMap interactions{MapActions(actions)};

    // Time to update the arena and birth/kill lifeforms; take the main lock
    vl.lock();

    ResolveInteractions(interactions);

    // Handle energy and replication
    ApplyEnergyLevelsToLifeforms();

    // Handle birth & death
    KillStarvedLifeforms();
    SplitFatLifeforms();

    // Blast a lifeform off into outer space!  (Actually another engine)
    if (asteroid_ && Params::kLifeformAsteroidLaunchInterval != 0 && turns_ % Params::kLifeformAsteroidLaunchInterval == 0) {
      auto lf = arena_->RemoveRandomLifeform();
      if (lf) {
        asteroid_->LaunchLifeform(lf);
      }
    }

    // Get a lifeform from outer space!  (Actually another engine)
    if (asteroid_ && Params::kLifeformAsteroidLandInterval != 0 && turns_ % Params::kLifeformAsteroidLandInterval == 0) {
      auto lf = asteroid_->LandLifeform();
      if (lf) {
        Coord c(arena_->GetRandomCoordOnArena());
        arena_->AddLifeform(lf, c);
      }
    }

    // End main loop timer
    loop_timer->EndCollection();
    ++turns_;
    vl.unlock();
  }

  {
    // Lock block
    std::lock_guard<std::mutex> lg(mutex_);
    timers_.clear();
  }
}


ActionMap EvolEngine::MapActions(const std::forward_list<Action> & actions) const {
  ActionMap interactions{ActionMap()};
  for (auto act : actions) {
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
    interactions[dest].emplace_back(act);
  }
  return interactions;
}


/**
 * Commit the resolved interactions of each lifeform.
 */
void EvolEngine::ResolveInteractions(ActionMap & interactions) {
  for (auto & elem : interactions) {
    auto & coord = elem.first;
    auto & actions = elem.second;
    for (auto & act: actions) {
      if (act.type == ActionType::APOPTOSIS) {
        arena_->RemoveLifeform(act.actor);
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
 * Finally, every lifeform loses a base amount of energy + (numbero of opcodes *
 * cost per opcode) every turn.  The opcode cost discourages large amounts of
 * junk Dna which consume CPU cycles.
 */
void EvolEngine::ApplyEnergyLevelsToLifeforms() {
  Coord c;
  int width = arena_->Width();
  int height = arena_->Height();

  for (c.x = 0; c.x < width; c.x++) {
    for (c.y = 0; c.y < height; c.y++) {
      auto occupants = arena_->LifeformsAt(c);
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
        if (adjacent.empty()) {
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
    float energy = lf->GetEnergy() - Params::kCostOfLiving - Params::kCostOfOpcode * lf->GetDnaSize();
    lf->SetEnergy(energy);
  }
}


void EvolEngine::KillStarvedLifeforms() {
  for (auto & lf : arena_->Lifeforms()) {
    if (lf->GetEnergy() <= 0.0) {
      lf->SetKilled();
      arena_->RemoveLifeform(lf);
    }
  }
}


void EvolEngine::SplitFatLifeforms() {
  auto lifeforms = arena_->Lifeforms();
  for (auto & lf : lifeforms) {
    if (!lf) {
      abort();
    }
    float parent_energy = lf->GetEnergy();
    if (parent_energy >= Params::kMeiosisLevel) {
      Lifeform baby = lf->MakeChild();
      baby->Mutate();
      parent_energy -= Params::kMeiosisCost;
      baby->SetEnergy(parent_energy / 2.0);
      lf->SetEnergy(parent_energy / 2.0);
      arena_->AddLifeform(baby, lf->GetCoord());
    }
  }
}


}  // namespace evol
