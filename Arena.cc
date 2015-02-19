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

#include "Coord.h"
#include "Lifeform.h"

namespace evol {


#define ARENA_VEC_OFFSET(c) ((c).y * width_ + (c).x)


void Arena::AddAndOwnLifeform(Lifeform * lf, const Coord & c) {
  lf->SetCoord(c);
  blocks_[ARENA_VEC_OFFSET(c)].AddLifeform(lf);
  lifeforms_.emplace_back(lf);
}


std::list<Lifeform *> Arena::GetLifeforms(const Coord & c) const {
  return *blocks_[ARENA_VEC_OFFSET(c)].Lifeforms();
}


int32_t Arena::GetLifeformCount(const Coord &c) const {
  return blocks_[ARENA_VEC_OFFSET(c)].Lifeforms()->size();
}


void Arena::MoveLifeform(Lifeform * lf, const Coord & c) {
  blocks_[ARENA_VEC_OFFSET(lf->GetCoord())].RemoveLifeform(lf);
  lf->SetCoord(c);
  blocks_[ARENA_VEC_OFFSET(c)].AddLifeform(lf);
}


/**
 * Find the lifeform's pointer in our list.  Once it's there, move the
 * unique_ptr to a local "floating" reference and remove the list element.  Do
 * final cleanup and return the unique_ptr in case the caller wishes to do
 * anything else.
 */
std::unique_ptr<Lifeform> Arena::KillLifeform(Lifeform * lf) {
  std::unique_ptr<Lifeform> floating_reference;

  for (auto iter = lifeforms_.begin(); iter != lifeforms_.end(); ++iter) {
    if (iter->get() == lf) {
      floating_reference = std::move(*iter);
      lifeforms_.erase(iter);
      break;
    }
  }
  if (!floating_reference) {
    // Lifeform pointer wasn't found in the master list
    abort();   // XXX debug only, should just return nullptr
  }

  blocks_[ARENA_VEC_OFFSET(lf->GetCoord())].RemoveLifeform(lf);
  lf->SetKilled();
  dead_lifeforms_count_++;

  return floating_reference;
}


/**
 * Like KillLifeform(), except:
 *   - pick a lifeform at random
 *   - do not flag lifeform as killed or increment death counters
 */
std::unique_ptr<Lifeform> Arena::RemoveRandomLifeform() {
  std::unique_ptr<Lifeform> floating_reference;
  int32_t victim_index = Random::Int32(0, lifeforms_.size() - 1);
  int32_t index = 0;
  auto iter = lifeforms_.begin();

  for (; iter != lifeforms_.end(); ++iter, ++index) {
    if (index == victim_index) {
      floating_reference = std::move(*iter);
      lifeforms_.erase(iter);
      break;
    }
  }
  if (!floating_reference) {
    // Should be impossible
    abort();
  }

  blocks_[ARENA_VEC_OFFSET(floating_reference->GetCoord())].RemoveLifeform(floating_reference.get());
  return floating_reference;
}


std::list<Lifeform *> Arena::GetAdjacentLifeforms(const Coord &c) const {
  std::list<Lifeform *> adjacent;

  for (int32_t xp = -1; xp <= 1; xp++) {
    for (int32_t yp = -1; yp <= 1; yp++) {
      if (xp == 0 && yp == 0) {
        continue;
      }
      Coord t(c.x + xp, c.y + yp);
      for (auto & lf : *blocks_[ARENA_VEC_OFFSET(t)].Lifeforms()) {
        adjacent.push_back(lf);
      }
    }
  }

  return adjacent;
}


float Arena::GetEnergy(const Coord &c) const {
  return blocks_[ARENA_VEC_OFFSET(c)].GetEnergy();
}


}  // namespace evol
