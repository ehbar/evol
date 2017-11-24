/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2017 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#pragma once
#include <cstdint>

namespace evol {


struct LifeformWatermarks {
  LifeformWatermarks()
      : lifeforms_count_lowater(UINT64_MAX),
        lifeforms_count_hiwater(0) {}

  uint64_t lifeforms_count_lowater;
  uint64_t lifeforms_count_hiwater;
};


}  // namespace evol
