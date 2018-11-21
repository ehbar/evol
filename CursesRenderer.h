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
#include <vector>

#include "EvolEngine.h"
#include "LifeformWatermarks.h"
#include "Timer.h"

namespace evol {


/**
 * Generate some tidy output on the terminal screen.
 */
class CursesRenderer {
 public:
  CursesRenderer(std::vector<EvolEngine> * engines = nullptr, Asteroid * asteroid = nullptr, int fps = 60)
      : target_fps_(fps),
        did_init_(false),
        engines_(engines),
        asteroid_(asteroid) {
    engine_stats_.resize(engines->size());
  }
  ~CursesRenderer() { Cleanup(); };

  CursesRenderer(const CursesRenderer &) = delete;
  CursesRenderer & operator=(const CursesRenderer &) = delete;

  void Init();
  void Run();
  void Cleanup();

 private:
  int target_fps_;
  bool did_init_;
  std::vector<EvolEngine> * engines_;
  std::vector<LifeformWatermarks> engine_stats_;
  Asteroid * asteroid_;

  void RenderFrame(const Timer *);
};


}  // namespace evol
