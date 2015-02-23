/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include <cstdint>

#include "Coord.h"
#include "gtest/gtest.h"

using namespace evol;


// Prime numbers to be extra weird
constexpr int32_t kMaxWidth = 13;
constexpr int32_t kMaxHeight = 17;


class CoordTest : public ::testing::Test {
 protected:
  virtual void SetUp() {
    Coord::SetGlobalBounds(kMaxWidth, kMaxHeight);
  }

  // virtual void TearDown() {}
};


TEST_F(CoordTest, StartsAtZero) {
  Coord c;

  EXPECT_EQ(0, c.x);
  EXPECT_EQ(0, c.y);
}


TEST_F(CoordTest, HoldsMaxAndMinValues) {
  Coord c(kMaxWidth - 1, kMaxHeight - 1);

  EXPECT_EQ(kMaxWidth - 1, c.x);
  EXPECT_EQ(kMaxHeight - 1, c.y);

  c = Coord(0, 0);
  EXPECT_EQ(0, c.x);
  EXPECT_EQ(0, c.y);
}


TEST_F(CoordTest, WrapsMaxValues) {
  Coord ca(kMaxWidth, kMaxHeight);
  EXPECT_EQ(0, ca.x);
  EXPECT_EQ(0, ca.y);

  Coord cb(kMaxWidth + 1, kMaxHeight + 1);
  EXPECT_EQ(1, cb.x);
  EXPECT_EQ(1, cb.y);

  Coord cc(-1, -1);
  EXPECT_EQ(kMaxWidth - 1, cc.x);
  EXPECT_EQ(kMaxHeight - 1, cc.y);

  Coord cd(-2, -3);
  EXPECT_EQ(kMaxWidth - 2, cd.x);
  EXPECT_EQ(kMaxHeight - 3, cd.y);
}


TEST_F(CoordTest, Directions) {
  Coord c;

  Coord cn = c.North();
  EXPECT_EQ(0, cn.x);
  EXPECT_EQ(kMaxHeight - 1, cn.y);

  Coord cs = c.South();
  EXPECT_EQ(0, cs.x);
  EXPECT_EQ(1, cs.y);

  Coord ce = c.East();
  EXPECT_EQ(1, ce.x);
  EXPECT_EQ(0, ce.y);

  Coord cw = c.West();
  EXPECT_EQ(kMaxWidth - 1, cw.x);
  EXPECT_EQ(0, cw.y);
}
