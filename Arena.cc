/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
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


void Arena::AddLifeform(Lifeform lf, const Coord & c) {
  lf->SetCoord(c);
  grid_.At(c).AddLifeform(lf);
  lifeforms_.push_back(lf);
}


void Arena::MoveLifeform(Lifeform lf, const Coord & c) {
  grid_.At(lf->GetCoord()).RemoveLifeform(lf);
  lf->SetCoord(c);
  grid_.At(c).AddLifeform(lf);
}


/**
 * Removes the lifeform from the Arena.  Returns the lifeform if it was
 * found, otherwise nullptr.
 */
Lifeform Arena::RemoveLifeform(const Lifeform & lf) {
  Lifeform ret;

  for (auto iter = lifeforms_.begin(); iter != lifeforms_.end(); ++iter) {
    if (**iter == *lf) {
      ret = *iter;
      lifeforms_.erase(iter);
      break;
    }
  }
  grid_.At(lf->GetCoord()).RemoveLifeform(lf);
  lf->SetKilled();
  dead_lifeforms_count_++;

  return ret;
}


/**
 * Like KillLifeform(), except:
 *   - pick a lifeform at random
 *   - increment death counters
 *   - Return pointer to lifeform
 *
 * Returns nullptr if the arena was empty.
 */
Lifeform Arena::RemoveRandomLifeform() {
  Lifeform ret;

  size_t numlf = lifeforms_.size();
  if (numlf < 1) {
    // Arena is empty
    return nullptr;
  }

  auto victim = lifeforms_.begin() + Random::Int32(0, numlf - 1);
  ret = *victim;
  grid_.At((*victim)->GetCoord()).RemoveLifeform(*victim);
  lifeforms_.erase(victim);

  return ret;
}


std::vector<LifeformImpl *> Arena::GetAdjacentLifeforms(const Coord &c) {
  std::vector<LifeformImpl *> adjacent;

  for (Unit xp = -1; xp <= 1; xp++) {
    for (Unit yp = -1; yp <= 1; yp++) {
      if (xp == 0 && yp == 0) {
        continue;
      }
      Coord t(c.x + xp, c.y + yp);
      for (auto & lf : grid_.At(t).Lifeforms()) {
        adjacent.push_back(lf.get());
      }
    }
  }

  return adjacent;
}


bool Arena::AdjacentLifeforms(const Coord &c) const {
  for (Unit xp = -1; xp <= 1; xp++) {
    for (Unit yp = -1; yp <= 1; yp++) {
      if (xp == 0 && yp == 0) {
        continue;
      }
      Coord t(c.x + xp, c.y + yp);
      if (!grid_.At(t).Lifeforms().empty()) {
        return true;
      }
    }
  }
  return false;
}


}  // namespace evol
