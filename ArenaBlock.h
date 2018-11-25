/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
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


typedef float Energy;
typedef int16_t Elevation;


class ArenaBlock {
 public:
  ArenaBlock(Energy en = 1.0f, Elevation el = 0) : energy_(en), elevation_(el) {}

  ArenaBlock(ArenaBlock && other) {
    energy_ = other.energy_;
    elevation_ = other.elevation_;
    lifeforms_ = std::move(other.lifeforms_);
    other.lifeforms_.clear();
  }

  // Delete clone & assign constructors because it's not obvious how we'd handle
  // the list of occupant lifeforms in the source ArenaBlock
  ArenaBlock(const ArenaBlock &) = delete;
  ArenaBlock & operator=(const ArenaBlock &other) = delete;


  /**
   * Get and set Energy value of the block
   */
  Energy GetEnergy() const { return energy_; }
  void SetEnergy(Energy new_energy) {
    energy_ = new_energy;
  }

  /**
   * Get and set Elevation value of the block
   */
  Elevation GetElevation() const { return elevation_; }
  void SetElevation(Elevation new_elevation) {
    elevation_ = new_elevation;
  }

  /**
   * Returns reference to the ArenaBlock's list of lifeforms.
   */
  std::vector<Lifeform> & Lifeforms() { return lifeforms_; }
  const std::vector<Lifeform> & Lifeforms() const { return lifeforms_; }

  /**
   * Adds the given Lifeform to the block.
   */
  Lifeform & AddLifeform(const Lifeform & lf) {
    lifeforms_.push_back(lf);
    return lifeforms_.back();
  }

  /**
   * Removes the given lifeform from the block, if it was there.
   */
  void RemoveLifeform(const Lifeform & victim) {
    for (auto iter = lifeforms_.begin(); iter != lifeforms_.end(); iter++) {
      if (**iter == *victim) {
        lifeforms_.erase(iter);
        break;
      }
    }
  }

  /**
   * Returns true if the lifeform is in the block.
   */
  bool HasLifeform(const Lifeform & lf) {
    for (auto & occupant : lifeforms_) {
      if (*occupant == *lf) {
        return true;
      }
    }
    return false;
  }

 private:
  Energy energy_;
  Elevation elevation_;
  std::vector<Lifeform> lifeforms_;
};


}  // namespace evol
#endif  // EVOL_ARENA_BLOCK_H_
