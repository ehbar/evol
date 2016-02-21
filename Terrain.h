#pragma once

#include <memory>
#include <stdint>

#include "Grid.h"

namespace evol {


typedef uint32_t TerrainDim;
typedef int16_t TerrainHeight;


class Terrain {
  Terrain() = delete;

  Terrain(TerrainDim x, TerrainDim y) : width_(x), height_(y) {
    heights_.reset(x, y, 0);
  }

  // Generate a terrain with the given minimum and maximum heights.
  void ScrambleHeights(TerrainHeight min, TerrainHeight max);

  TerrainHeight GetHeightAt(TerrainDim x, TerrainDim y) const {
    return heights_->At(x, y);
  }

 private:
  TerrainDim width_;
  TerrainDim height_;

  std::unique_ptr<Grid<TerrainDim, TerrainHeight>> heights_;
};


}  // namespace evol
