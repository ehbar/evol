/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include "Arena.h"

#include <stdlib.h>  // abort()

#include <cstdint>

#include "ArenaBlock.h"
#include "Coord.h"
#include "Lifeform.h"

namespace evol {


void Arena::AddAndOwnLifeform(Lifeform * lf, const Coord & c) {
  lf->SetCoord(c);
  grid_->At(c).AddLifeform(lf);
  lifeforms_.emplace_back(lf);
}


std::list<Lifeform *> Arena::GetLifeforms(const Coord & c) const {
  return *(grid_->At(c).Lifeforms());
}


size_t Arena::GetLifeformCount(const Coord &c) const {
  return grid_->At(c).Lifeforms()->size();
}


void Arena::MoveLifeform(Lifeform * lf, const Coord & c) {
  grid_->At(lf->GetCoord()).RemoveLifeform(lf);
  lf->SetCoord(c);
  grid_->At(c).AddLifeform(lf);
}


/**
 * Find the lifeform's pointer in our list.  Once it's there, move the
 * unique_ptr to a local temp variable and remove the list element.  Do
 * final cleanup and return the unique_ptr in case the caller wishes to do
 * anything else.
 */
std::unique_ptr<Lifeform> Arena::KillLifeform(Lifeform * lf) {
  std::unique_ptr<Lifeform> tmp;

  for (auto iter = lifeforms_.begin(); iter != lifeforms_.end(); ++iter) {
    if (iter->get() == lf) {
      tmp = std::move(*iter);
      lifeforms_.erase(iter);
      break;
    }
  }
  if (!tmp) {
    // Lifeform pointer wasn't found in the master list
    abort();   // XXX debug only, should just return nullptr
  }

  grid_->At(lf->GetCoord()).RemoveLifeform(lf);
  lf->SetKilled();
  dead_lifeforms_count_++;

  return tmp;
}


/**
 * Like KillLifeform(), except:
 *   - pick a lifeform at random
 *   - do not flag lifeform as killed or increment death counters
 */
std::unique_ptr<Lifeform> Arena::RemoveRandomLifeform() {
  std::unique_ptr<Lifeform> tmp;
  size_t victim_index = Random::Int32(0, lifeforms_.size() - 1);
  size_t index = 0;
  auto iter = lifeforms_.begin();

  for (; iter != lifeforms_.end(); ++iter, ++index) {
    if (index == victim_index) {
      tmp = std::move(*iter);
      lifeforms_.erase(iter);
      break;
    }
  }
  if (!tmp) {
    abort();
  }

  grid_->At(tmp->GetCoord()).RemoveLifeform(tmp.get());
  return tmp;
}


std::list<Lifeform *> Arena::GetAdjacentLifeforms(const Coord &c) const {
  std::list<Lifeform *> adjacent;

  for (Unit xp = -1; xp <= 1; xp++) {
    for (Unit yp = -1; yp <= 1; yp++) {
      if (xp == 0 && yp == 0) {
        continue;
      }
      Coord t(c.x + xp, c.y + yp);
      for (auto & lf : *(grid_->At(t).Lifeforms())) {
        adjacent.push_back(lf);
      }
    }
  }

  return adjacent;
}


Energy Arena::GetEnergy(const Coord &c) const {
  return grid_->At(c).GetEnergy();
}


Elevation Arena::GetElevation(const Coord &c) const {
  return grid_->At(c).GetElevation();
}


}  // namespace evol
