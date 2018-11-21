/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#pragma once

#include <cassert>

#include "Coord.h"

namespace evol {


/**
 * Generic type describing a 2-dimensional container with value type V.  The 2D plane
 * has coordinates (0, 0) at the "top left" or "northwest" corner and (x-1, y-1) at
 * the "bottom right"/"southeast" corner.
 *
 * V must have a default constructor because I suck at C++.
 */
template <typename V>
class Grid {
 public:
  Grid() = delete;

  /**
   * For the constructor, the supplied x and y values should be the
   * size of the grid, such that (x - 1, y - 1) is the coordinate of the
   * "bottom right".
   */
  Grid(const Unit xMax, const Unit yMax) {
    assert(xMax > 0 && yMax > 0);
    xMax_ = xMax;
    yMax_ = yMax;
    spaces_.resize(xMax_ * yMax_);
  }

  /**
   * Return reference to contained object at the given coords.
   */
  V & At(const Coord & c) {
    assert(c.y >= 0 && c.y < yMax_ && c.x >= 0 && c.x < xMax_);
    return spaces_[c.y * xMax_ + c.x];
  }

  Unit XMax() const { return xMax_; }
  Unit YMax() const { return yMax_; }

  /**
   * Normalize the given coordinate to a point wrapped within the grid.
   */
  Coord * Normalize(Coord * c) {
    if (c->x >= xMax_) {
      c->x %= xMax_;
    } else if (c->x < 0) {
      c->x = c->x % xMax_ + xMax_;
    }
    if (c->y >= yMax_) {
      c->y %= yMax_;
    } else if (c->y < 0) {
      c->y = c->y % yMax_ + yMax_;
    }

    return c;
  }

 private:
  Unit xMax_;
  Unit yMax_;
  std::vector<V> spaces_;
};


}  // namespace evol
