/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#pragma once
#include <cstdint>
#include <functional>  // std::hash

namespace evol {


typedef int32_t Unit;


// Cartesian coordinate with origin at 0, 0 and equality comparison.
class Coord {
 public:
  Unit x;
  Unit y;

  /**
   * Set the bounding box of all Coord objects globally.  Newly set Coord
   * objects will wrap their coordinates to remain inside this square.  Since
   * this doesn't affect existing Coord objects, this is normally done at
   * runtime init and never changed again.
   */
  static void SetGlobalBounds(Unit max_x, Unit max_y) {
    Coord::max_x_ = max_x;
    Coord::max_y_ = max_y;
  }

  Coord() : x(0), y(0) {}
  Coord(Unit xx, Unit yy) : x(xx), y(yy) {
    Normalize();
  }
  Coord(const Coord & other) : x(other.x), y(other.y) {}
  Coord(Coord && other) : x(other.x), y(other.y) {}
  Coord & operator=(const Coord & other) {
    x = other.x;
    y = other.y;
    return *this;
  }

  bool operator==(const Coord & other) const {
    return x == other.x && y == other.y;
  }

  Coord North() { return Coord(x, y - 1); }
  Coord South() { return Coord(x, y + 1); }
  Coord East() { return Coord(x + 1, y); }
  Coord West() { return Coord(x - 1, y); }

  /**
   * Wraps the coordinate inside the global boundary box.
   */
  void Normalize() {
    Normalize(max_x_, max_y_);
  }

  /**
   * Wraps the coordinate inside the given boundary box.
   */
  void Normalize(Unit xMax, Unit yMax) {
    if (x < 0) {
      x = x % xMax + xMax;
    } else if (x >= Coord::max_x_) {
      x %= xMax;
    }
    if (y < 0) {
      y = y % yMax + yMax;
    } else if (y >= Coord::max_x_) {
      y %= yMax;
    }
  }

 private:
  static Unit max_x_;
  static Unit max_y_;
};


}  // namespace evol
namespace std {


// Hash template specialization for Coord; allows use in maps etc.
// TODO: Only reliable on 64-bit systems!  Should be fixed ASAP
template <>
struct hash<evol::Coord> {
  std::size_t operator()(const evol::Coord &c) const {
    auto hasher = hash<int64_t>();
    return hasher(c.x) << 32 | hasher(c.y);
  } 
};


}  // namespace std
