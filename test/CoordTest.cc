#include "Coord.h"
#include "gtest/gtest.h"

namespace evol {


TEST(CoordTest, StartsAtZero) {
  Coord c;

  EXPECT_EQ(c.x, 0);
  EXPECT_EQ(c.y, 0);
}


}  // namespace evol
