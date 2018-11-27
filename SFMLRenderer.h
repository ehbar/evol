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
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EvolEngine.h"
#include "LifeformWatermarks.h"
#include "Timer.h"

namespace evol {




class SFMLRenderer {
 public:
  SFMLRenderer(std::shared_ptr<std::vector<EvolEngine>> engines = nullptr, std::shared_ptr<Asteroid> asteroid = nullptr, int fps = 15)
      : target_fps_(fps),
        did_init_(false),
        engines_(engines),
        asteroid_(asteroid),
        engine_stats_(engines->size()),
        fontSize_(16) {}

  ~SFMLRenderer() { Cleanup(); };

  SFMLRenderer(const SFMLRenderer &) = delete;
  SFMLRenderer & operator=(const SFMLRenderer &) = delete;

  void Init();
  void Run();
  void Cleanup();

 private:
  enum PanelViewTarget { NONE, OVERVIEW, ENGINE, ARENA, HELP = 999 };

  struct PanelView {
    PanelViewTarget target;
    int engine_number;
    PanelView() : target(PanelViewTarget::OVERVIEW), engine_number(0) {}; 
  };

  // General state
  int target_fps_;
  bool did_init_;
  std::shared_ptr<std::vector<EvolEngine>> engines_;
  std::shared_ptr<Asteroid> asteroid_;
  std::vector<LifeformWatermarks> engine_stats_;

  // What the user wants to see
  PanelView panel_view_;

  // SFML state
  sf::RenderWindow sfWindow_;
  sf::Font sfFont_;
  unsigned fontSize_;

  // Private methods
  void RenderOverview();
  void RenderEngine() { abort(); }  // XXX unimplemented
  void RenderArena() { abort(); }   // XXX unimplemented
  void RenderHelp() { abort(); }    // XXX unimplemented
  sf::Text MakeText(const char *fmt, ...);
  void DrawText(const sf::Color & color, float x, float y, const char *fmt, ...);
  void HandleKey(const sf::Event & event);
};


}  // namespace evol
