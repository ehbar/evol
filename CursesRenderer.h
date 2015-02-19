/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_CURSES_RENDERER_H_
#define EVOL_CURSES_RENDERER_H_

#include <cstdint>
#include <vector>

#include "EvolEngine.h"
#include "Timer.h"


namespace evol {


struct LifeformWatermarks {
  LifeformWatermarks()
      : lifeforms_count_lowater(UINT64_MAX),
        lifeforms_count_hiwater(0) {}

  uint64_t lifeforms_count_lowater;
  uint64_t lifeforms_count_hiwater;
};


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

  explicit CursesRenderer(const CursesRenderer &) = delete;
  explicit CursesRenderer(CursesRenderer &&) = delete;

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
#endif  // EVOL_CURSES_RENDERER_H_
