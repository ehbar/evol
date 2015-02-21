/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_COORD_H_
#define EVOL_COORD_H_

#include <cstdint>
#include <functional>  // std::hash

namespace evol {


// Cartesian coordinate with origin at 0, 0 and equality comparison.
class Coord {
 public:
  int32_t x;
  int32_t y;

  /**
   * Set the bounding box of all Coord objects globally.  Newly set Coord
   * objects will wrap their coordinates to remain inside this square.  Since
   * this doesn't affect existing Coord objects, this is normally done at
   * runtime init and never changed again.
   */
  static void SetGlobalBounds(int32_t max_x, int32_t max_y) {
    Coord::max_x_ = max_x;
    Coord::max_y_ = max_y;
  }

  Coord() : x(0), y(0) {}
  Coord(int32_t xx, int32_t yy) : x(xx), y(yy) {
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
   * Wraps the coordinate inside the boundary box.
   */
  void Normalize() {
    // TODO: Do the math for these instead of naively looping
    while (x < 0) {
      x += Coord::max_x_;
    }
    while (x >= Coord::max_x_) {
      x -= Coord::max_x_;
    }
    while (y < 0) {
      y += Coord::max_y_;
    }
    while (y >= Coord::max_y_) {
      y -= Coord::max_y_;
    }
  }

 private:
  static int32_t max_x_;
  static int32_t max_y_;

};


}  // namespace evol
namespace std {


// Hash template specialization for Coord; allows use in maps etc.
template <>
struct hash<evol::Coord> {
  std::size_t operator()(const evol::Coord &c) const {
    auto hasher = hash<int64_t>();
    return hasher(c.x) << 32 | hasher(c.y);
  } 
};


}  // namespace std
#endif  // EVOL_COORD_H_
