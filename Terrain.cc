#include "Terrain.h"

#include <cmath>


/**
 * Given a positive integer j, return k such that k == 2^x + 1 and
 * k >= j.
 */
static TerrainDim MinDiamondSquareDim(TerrainDim j) {
  if (j < 2) {
    j = 2;
  }
  double k = pow(2, ceil(log2(j - 1)));
  return static_cast<TerrainDim>(k) + 1;
}


/**
 * Generate a terrain with the given minimum and maximum heights.
 * We do this using the Diamond-Square algorithm (DS). DS requires
 * a square with sides of length 2^N+1.  A given Terrain object
 * can be rectangular and of any scalar size, so we'll generate
 * the height map in a separate object and copy it over.
 */
void Terrain::ScrambleHeights(TerrainHeight min, TerrainHeight max) {
  auto biggestDim = (width_ > height_ ? width_ : height_);
  auto squareDim = MinDiamondSquareDim(biggestDim);

  std::unique_ptr<TerrainDim[]> square { new TerrainDim[squareDim * squareDim] };
  

}
