/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#pragma once
#include <cstdint>
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>

#include "EvolEngine.h"
#include "LifeformWatermarks.h"
#include "Timer.h"

namespace evol {


class SFMLRenderer {
 public:
  SFMLRenderer(std::vector<EvolEngine> * engines = nullptr, Asteroid * asteroid = nullptr, int fps = 60)
      : target_fps_(fps),
        did_init_(false),
        engines_(engines),
        asteroid_(asteroid) {
    engine_stats_.resize(engines->size());
  }
  ~SFMLRenderer() { Cleanup(); };

  SFMLRenderer(const SFMLRenderer &) = delete;
  SFMLRenderer & operator=(const SFMLRenderer &) = delete;

  void Init();
  void Run();
  void Cleanup();

 private:
  // General state
  int target_fps_;
  bool did_init_;
  std::vector<EvolEngine> * engines_;
  std::vector<LifeformWatermarks> engine_stats_;
  Asteroid * asteroid_;

  // SFML state
  std::unique_ptr<sf::RenderWindow> sfWindow_;
  std::unique_ptr<sf::Font> sfFont_;

  void RenderFrame(const Timer *);
};


}  // namespace evol
