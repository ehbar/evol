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
#include <chrono>
#include <vector>

#include "EvolEngine.h"
#include "LifeformWatermarks.h"
#include "Timer.h"

namespace evol {


/**
 * Generate some tidy output on the terminal screen.
 */
class ConsoleRenderer {
 public:
  ConsoleRenderer(std::shared_ptr<std::vector<EvolEngine>> engines = nullptr, std::shared_ptr<Asteroid> asteroid = nullptr, int interval = 60)
      : interval_(interval),
        engines_(engines),
        asteroid_(asteroid),
        engine_stats_(engines->size()) {}

  ConsoleRenderer(const ConsoleRenderer &) = delete;
  ConsoleRenderer & operator=(const ConsoleRenderer &) = delete;

  void Run();

 private:
  bool did_init_;
  int interval_;
  std::shared_ptr<std::vector<EvolEngine>> engines_;
  std::vector<LifeformWatermarks> engine_stats_;
  std::shared_ptr<Asteroid> asteroid_;
};


}  // namespace evol
