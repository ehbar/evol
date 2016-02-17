/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_ARENA_BLOCK_H_
#define EVOL_ARENA_BLOCK_H_

#include <list>
#include <utility>

#include "Lifeform.h"

namespace evol {


class ArenaBlock {
 public:
  ArenaBlock(int e = 1) : energy_(e) {}

  ArenaBlock(const ArenaBlock &) = delete;

  ArenaBlock(ArenaBlock && other) {
    energy_ = other.energy_;
    other.energy_ = 1;
    lifeforms_ = std::move(other.lifeforms_);
  }

  ArenaBlock & operator=(const ArenaBlock &other) = delete;

  float GetEnergy() const { return energy_; }
  void SetEnergy(float new_energy) {
    energy_ = new_energy;
  }

  /**
   * Returns a const pointer to the ArenaBlock's list of lifeforms.
   */
  const std::list<Lifeform *> * Lifeforms() const {
    return &lifeforms_;
  }

  /**
   * Adds the given Lifeform to the block.
   */
  void AddLifeform(Lifeform * lf) {
    lifeforms_.push_back(lf);
  }

  /**
   * Removes the given lifeform from the block, if it was there.
   */
  void RemoveLifeform(Lifeform * victim) {
    for (auto iter = lifeforms_.begin(); iter != lifeforms_.end(); iter++) {
      if (*iter == victim) {
        lifeforms_.erase(iter);
        break;
      }
    }
  }

  /**
   * Returns true if the lifeform is in the block.
   */
  bool HasLifeform(Lifeform * lf) {
    for (auto & occupant : lifeforms_) {
      if (occupant == lf) {
        return true;
      }
    }
    return false;
  }

 private:
  float energy_;
  std::list<Lifeform *> lifeforms_;
};


}  // namespace evol
#endif  // EVOL_ARENA_BLOCK_H_
