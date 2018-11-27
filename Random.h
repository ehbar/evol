/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_RANDOM_H_
#define EVOL_RANDOM_H_

#include <cstdint>
#include <random>

namespace evol {

class Random {
 public:
  Random() {
    std::random_device rd;
    generator_.seed(rd());
  }

  /**
   * Return a random int32_t in range [min, max].
   */
  int32_t Int32(int32_t min, int32_t max) {
    std::uniform_int_distribution<int32_t> die_roll(min, max);
    return die_roll(generator_);
  }

 private:
  std::default_random_engine generator_;
};


}  // namespace evol
#endif  // EVOL_RANDOM_H_
