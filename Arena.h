/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_ARENA_H_
#define EVOL_ARENA_H_

#include <cstdint>
#include <algorithm>
#include <memory>
#include <vector>

#include "ArenaBlock.h"
#include "Coord.h"
#include "Grid.h"
#include "Lifeform.h"
#include "Random.h"

namespace evol {


/**
 * The Arena is the grid which all lifeforms live upon.  It has methods for
 * accessing said lifeforms.
 *
 */
class Arena {
 public:
  Arena() = delete;
  Arena(Unit w, Unit h) : width_(w), height_(h), dead_lifeforms_count_(0), grid_(Grid<ArenaBlock>(w, h)) {
    assert(w > 0 && h > 0);
  }

  Arena(const Arena &) = delete;
  Arena(Arena &&) = delete;
  Arena & operator=(const Arena &) = delete;

  int Width() const { return width_; }
  int Height() const { return height_; }

  /**
   * Returns the vector of living lifeforms.
   */
  std::vector<Lifeform> Lifeforms() { return lifeforms_; }
  const std::vector<Lifeform> Lifeforms() const { return lifeforms_; }

  /**
   * Return total number of live lifeforms.
   */
  uint64_t NumLifeforms() const { return lifeforms_.size(); }

  /**
   * Return total number of dead lifeforms.
   */
  uint64_t NumDeadLifeforms() const { return dead_lifeforms_count_; }

  /**
   * Add the given lifeform at the given x/y coord.
   */
  void AddLifeform(Lifeform lf, const Coord & c);

  /**
   * Returns Lifeforms at the given location.
   */
  std::vector<Lifeform> LifeformsAt(const Coord & c) { return grid_.At(c).Lifeforms(); }
  const std::vector<Lifeform> LifeformsAt(const Coord & c) const { return grid_.At(c).Lifeforms(); }

  /**
   * Returns count of Lifeforms at the given location.
   */
  uint64_t NumLifeformsAt(const Coord & c) { return grid_.At(c).Lifeforms().size(); }

  /**
   * Move the lifeform to the given location.
   */
  void MoveLifeform(Lifeform lf, const Coord & c);

  /**
   * Removes the given lifeform from the plane if it exists.  Returns the
   * lifeform if it was found, else nullptr.
   */
  Lifeform RemoveLifeform(const Lifeform & lf);

  /**
   * Remove a random lifeform.  Returns the lifeform if it was removed,
   * else nullptr (this will happen if the arena is empty).
   */
  Lifeform RemoveRandomLifeform();

  /**
   * Returns all lifeforms in squares adjacent to the given Coord.
   */
  std::vector<LifeformImpl *> GetAdjacentLifeforms(const Coord &);

  /**
   * Returns true if there are adjacent lifeforms.
   */
  bool AdjacentLifeforms(const Coord &c) const;

  /**
   * Return energy available at the given coordinate.
   */
  Energy GetEnergy(const Coord &c) const { return grid_.At(c).GetEnergy(); }

  /**
   * Return elevation of the given coordinate.
   */
  Elevation GetElevation(const Coord &c) const { return grid_.At(c).GetElevation(); }

  /**
   * Utility method: Return a coord guaranteed to be in the bounds of this arena
   */
  Coord GetRandomCoordOnArena() const {
    return Coord(Random::Int32(0, width_ - 1),
                 Random::Int32(0, height_ - 1));
  }

 private:
  Unit width_;
  Unit height_;
  uint64_t dead_lifeforms_count_;

  std::vector<Lifeform> lifeforms_;

  // Grid of ArenaBlocks representing the "physical" space.
  Grid<ArenaBlock> grid_;

};


}  // namespace evol
#endif  // EVOL_ARENA_H_
