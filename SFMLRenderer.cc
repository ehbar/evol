/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#pragma GCC diagnostic ignored "-Wswitch"  // this is the dumbest warning ever
#include "SFMLRenderer.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <mutex>
#include <string>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Arena.h"

namespace evol {


static constexpr size_t kMaxStringSize = 511;
static constexpr unsigned int kFontPixels = 16;
static constexpr const char * kFontFace = "/usr/share/fonts/truetype/freefont/FreeMono.ttf";


void SFMLRenderer::Init() {
  if (did_init_) {
    abort();
  }

  sfWindow_.setFramerateLimit(target_fps_);
  sfWindow_.create(sf::VideoMode(768, 512), "evol");
  // XXX make this good:
  if (!sfFont_.loadFromFile(kFontFace)) {
    abort();
  }

  did_init_ = true;
}


void SFMLRenderer::Run() {
  if (!did_init_) {
    abort();
  }

  while (sfWindow_.isOpen()) {
    sf::Event event;
    while (sfWindow_.pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          sfWindow_.close();
          break;
        case sf::Event::Resized:
          // SFML will scale the view, but we handle that internally, so resize
          {
            auto newSize = sfWindow_.getSize();
            sf::View v(sf::FloatRect(0.f, 0.f, newSize.x, newSize.y));
            sfWindow_.setView(v);
          }
          break;
        case sf::Event::KeyPressed:
          HandleKey(event);
          break;
      }
    }

    sfWindow_.clear(sf::Color::Black);
    switch (panel_view_.target) {
      case PanelViewTarget::OVERVIEW:
        RenderOverview();
        break;
      case PanelViewTarget::ENGINE:
        RenderEngine();
        break;
      case PanelViewTarget::ARENA:
        RenderArena();
        break;
      case PanelViewTarget::HELP:
        RenderHelp();
        break;
      default:
        abort();
        break;
    }
    sfWindow_.display();
  }
}


void SFMLRenderer::Cleanup() {
  if (did_init_) {
    // nop?
  }
}


/**
 * Show the engines arranged in a grid, with stats printed onto each.
 */
void SFMLRenderer::RenderOverview() {
  // Calculate size of each engine in our grid of what's running
  float numAcross = ceil(sqrt(engines_->size()));

  auto panelSize = sfWindow_.getSize();
  panelSize.y = std::max(panelSize.y - 25, 0U);  // -25 = leave room at bottom for global stats

  sf::Vector2f rectsize(panelSize.x / numAcross - 5, panelSize.y / numAcross - 5);

  // Display stats for each engine in their own box
  int x = 0, y = 0;
  for (auto & e : *engines_) {
    // Draw box representing engine
    sf::RectangleShape engineRect(rectsize);
    engineRect.setFillColor(sf::Color(255, 255, 255));

    sf::Vector2f rectpos;
    rectpos.x = x * (panelSize.x / numAcross);
    rectpos.y = y * (panelSize.y / numAcross);

    engineRect.setPosition(rectpos);
    sfWindow_.draw(engineRect);

    // Fill box with some simple stats
    uint64_t num_alive = 0;
    uint64_t num_dead = 0;
    uint64_t dna_count = 0;
    uint64_t highest_gen = 0;
    TimerStats tstats;
    {
      // Lock the engine.  Keep this part as fast as possible
      std::lock_guard<std::mutex> eg(e.Mutex());

      auto & arena = e.GetArena();
      num_alive = arena.LifeformsCount();
      num_dead = arena.DeadLifeformsCount();
      for (auto & lf : arena.Lifeforms()) {
        dna_count += lf->GetDnaSize();
        highest_gen = std::max(highest_gen, lf->Gen());
      }
      tstats = e.GetTimers().front()->GetStats();  // theoretically this returns multiple timers, implement one for now
    }

    sf::Color black(0, 0, 0);
    DrawText(black, rectpos.x + 2, rectpos.y + 2,
             "Live: %u\nDead: %u\nAvg Dna len: %.2f\n",
             num_alive, num_dead, static_cast<float>(dna_count) / num_alive,
             highest_gen);
    sf::Color timerColor(0, 0, 64);
    DrawText(timerColor, rectpos.x + 2, rectpos.y + (panelSize.y / numAcross) - 75,
             "%s:\n  %ld avg (%ld/s)\n  %ld mn, %ld mx",
             tstats.description.c_str(),
             static_cast<long int>(tstats.us_avg),
             static_cast<long int>(1e6 / tstats.us_avg),
             static_cast<long int>(tstats.us_min),
             static_cast<long int>(tstats.us_max)
    );

    // Set up for next engine
    if (++x >= numAcross) {
      x = 0;
      y += 1;
    }
  }
}


/**
 * Draw the given sprintf-style formatted text with the given color at the given
 * x, y.
 */
void SFMLRenderer::DrawText(const sf::Color & color, float x, float y, const char *fmt, ...) {
  char tmpStr[kMaxStringSize+1];
  va_list ap;

  va_start(ap, fmt);
  vsnprintf(tmpStr, kMaxStringSize, fmt, ap);
  va_end(ap);

  sf::Text theText(tmpStr, sfFont_, kFontPixels);
  theText.setFillColor(color);
  theText.setOutlineColor(color);
  theText.setPosition(x, y);
  sfWindow_.draw(theText);
}


/**
 * Handle a character entered by the user.
 */
void SFMLRenderer::HandleKey(const sf::Event & event) {
  if (event.key.code == sf::Keyboard::Escape) {
    sfWindow_.close();
  }
  else if (event.key.code == sf::Keyboard::O) {
    panel_view_.target = PanelViewTarget::OVERVIEW;
  }
  else if (event.key.code == sf::Keyboard::E) {
    panel_view_.target = PanelViewTarget::ENGINE;
  }
  else if (event.key.code == sf::Keyboard::A) {
    panel_view_.target = PanelViewTarget::ARENA;
  }
  else if (event.key.code >= sf::Keyboard::Num0 && event.key.code <= sf::Keyboard::Num9) {
    // Handle number keys for engine selection in various views.  0-9 = engine.
    // Shift key adds 10 for 10-19, ctrl adds 20 for 20-29, both will add 30 for
    // 30-39
    size_t engine_num = static_cast<size_t>(event.key.code) - static_cast<size_t>(sf::Keyboard::Num0);
    assert(engine_num >= 0);
    if (event.key.shift) {
      engine_num += 10;
    }
    if (event.key.control) {
      engine_num += 20;
    }
    if (engine_num >= engines_->size()) {
      // do nothing if there aren't that many engines
      // TODO: maybe display an error message or play an error sound?
      return;
    }
    panel_view_.engine_number = engine_num;
  }
}


}  // namespace evol
