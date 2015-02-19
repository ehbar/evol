/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_RANDOM_H_
#define EVOL_RANDOM_H_

#include <unistd.h>
#include <sys/types.h>

#include <chrono>
#include <cstdint>
#include <memory>
#include <random>

namespace evol {


class Random {
 public:
  /**
   * Return a random int32_t in range [min, max].
   */
  static int32_t Int32(int32_t min, int32_t max) {
    if (Random::rand_generator_ == nullptr) {
      auto seed = std::chrono::system_clock::now().time_since_epoch().count();
      seed ^= getpid();
      rand_generator_.reset(new std::default_random_engine(seed));
    }

    std::uniform_int_distribution<int32_t> die_roll(min, max);
    return die_roll(*rand_generator_);
  }

 private:
  static std::unique_ptr<std::default_random_engine> rand_generator_;
};


}  // namespace evol
#endif  // EVOL_RANDOM_H_
