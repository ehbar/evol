/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_ARENA_H_
#define EVOL_ARENA_H_

#include <cstdint>
#include <list>
#include <memory>
#include <vector>

#include "ArenaBlock.h"
#include "Coord.h"
#include "Lifeform.h"
#include "Random.h"

namespace evol {


/**
 * The Arena is the grid which all lifeforms live upon.  It has methods for
 * accessing said lifeforms.
 *
 * The Arena owns the memory of all lifeforms loaded into it.
 */
class Arena {
 public:
  Arena(int32_t w, int32_t h) : width_(w), height_(h), blocks_(w * h), dead_lifeforms_count_(0) {}

  explicit Arena(const Arena &) = delete;
  explicit Arena(Arena &&) = delete;

  int Width() const { return width_; }
  int Height() const { return height_; }

  /**
   * Return a vector of living lifeforms.
   */
  std::vector<Lifeform *> Lifeforms() const {
    std::vector<Lifeform *> lfv(lifeforms_.size());
    int i = 0;
    for (auto & lfp : lifeforms_) {
      lfv[i++] = lfp.get();
    }
    return lfv;
  }

  /**
   * Return total number of live lifeforms.
   */
  uint64_t LifeformsCount() const { return lifeforms_.size(); }

  /**
   * Return total number of dead lifeforms.
   */
  uint64_t DeadLifeformsCount() const { return dead_lifeforms_count_; }

  /**
   * Add the given lifeform at the given x/y coord.  This takes memory ownership
   * of the lifeform, which must be passed in a unique_ptr container.
   */
  void AddAndOwnLifeform(Lifeform * lf, const Coord & c);

  /**
   * Returns a list of Lifeforms at the given location.
   */
  std::list<Lifeform *> GetLifeforms(const Coord & c) const;

  /**
   * Returns a count of Lifeforms at the given location.
   */
  int32_t GetLifeformCount(const Coord &c) const;

  /**
   * Move the lifeform to the given location.
   */
  void MoveLifeform(Lifeform * lf, const Coord & c);

  /**
   * Kill the given lifeform.  Marks it dead, removes it from the arena plane,
   * and releases memory ownership of its instance to the caller.  Since it's in
   * a unique_ptr, if the caller ignores the return value it will just be freed.
   */
  std::unique_ptr<Lifeform> KillLifeform(Lifeform *);

  /**
   * Remove a random lifeform.  Releases memory ownership to the caller.
   */
  std::unique_ptr<Lifeform> RemoveRandomLifeform();

  /**
   * Returns all lifeforms in squares adjacent to the given Coord.  Does not
   * return lifeforms in the Coord itself.
   */
  std::list<Lifeform *> GetAdjacentLifeforms(const Coord &) const;

  /**
   * Return energy available at the given coordinate.
   */
  float GetEnergy(const Coord &c) const;

  /**
   * Utility method: Return a coord guaranteed to be in the bounds of this arena
   */
  Coord GetRandomCoordOnArena() const {
    return Coord(Random::Int32(0, width_ - 1),
                 Random::Int32(0, height_ - 1));
  }

 private:
  int32_t width_;
  int32_t height_;

  // This is the master list of lifeforms.  Any reference to the lifeform will
  // be broken (probably causing a crash or other badness) if it's removed from
  // this list unless effort is made to retain the lifeform.
  std::list<std::unique_ptr<Lifeform>> lifeforms_;

  // Grid of ArenaBlocks representing the "physical" space.
  std::vector<ArenaBlock> blocks_;

  uint64_t dead_lifeforms_count_;
};


}  // namespace evol
#endif  // EVOL_ARENA_H_
