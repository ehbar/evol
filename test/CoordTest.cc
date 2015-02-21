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

  Coord c1;
};


TEST_F(CoordTest, StartsAtZero) {
  Coord c;

  EXPECT_EQ(0, c.x);
  EXPECT_EQ(0, c.y);
}


TEST_F(CoordTest, HoldsMaxValues) {
  c1 = Coord(kMaxWidth - 1, kMaxHeight - 1);
  EXPECT_EQ(kMaxWidth - 1, c1.x);
  EXPECT_EQ(kMaxHeight - 1, c1.y);
}


TEST_F(CoordTest, WrapsMaxValues) {
  c1 = Coord(kMaxWidth, kMaxHeight);
  c1.Normalize();
  EXPECT_EQ(0, c1.x);
  EXPECT_EQ(0, c1.y);

  c1 = Coord(kMaxWidth + 1, kMaxHeight + 1);
  c1.Normalize();
  EXPECT_EQ(1, c1.x);
  EXPECT_EQ(1, c1.y);

  c1 = Coord(-1, -1);
  c1.Normalize();
  EXPECT_EQ(kMaxWidth - 1, c1.x);
  EXPECT_EQ(kMaxHeight - 1, c1.y);
}
