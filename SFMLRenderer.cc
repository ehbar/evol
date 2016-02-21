/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#pragma GCC diagnostic ignored "-Wswitch"  // this is the dumbest warning ever
#include "SFMLRenderer.h"

#include <cstdint>
#include <cstdio>
#include <cstdarg>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "Arena.h"
#include "Timer.h"

namespace evol {


static constexpr size_t kMaxStringSize = 256;

// Stupid hack for print formatting strings.  Probably super slow.
// std::stringstream sucks.
// TODO: Use Folly or boost for this
static inline std::string MakeString(const char *fmt, ...) {
  va_list ap;
  char tmpStr[kMaxStringSize];

  va_start(ap, fmt);
  vsnprintf(tmpStr, kMaxStringSize, fmt, ap);
  va_end(ap);
  return std::string(tmpStr);
}


void SFMLRenderer::Init() {
  did_init_ = true;
  sfWindow_.reset(new sf::RenderWindow(sf::VideoMode(512, 512), "evol"));
  sfWindow_->setFramerateLimit(target_fps_);
  sfFont_.reset(new sf::Font);

  // XXX make this good:
  if (!sfFont_->loadFromFile("/usr/share/fonts/liberation/LiberationMono-Regular.ttf")) {
    abort();
  }
}


void SFMLRenderer::Cleanup() {
  if (did_init_) {
    // nop?
  }
}


void SFMLRenderer::Run() {
  if (!did_init_) {
    abort();
  }

  while (sfWindow_->isOpen()) {
    sf::Event event;
    while (sfWindow_->pollEvent(event)) {
      switch (event.type) {
        case sf::Event::Closed:
          sfWindow_->close();
          break;
        case sf::Event::Resized:
          // SFML will scale the view, but we handle that internally, so resize
          auto newSize = sfWindow_->getSize();
          sf::View v(sf::FloatRect(0.f, 0.f, newSize.x, newSize.y));
          sfWindow_->setView(v);
          break;
      }
    }

    sfWindow_->clear(sf::Color::Black);
    RenderFrame(nullptr);
    sfWindow_->display();
  }
}


void SFMLRenderer::RenderFrame(const Timer * poll_timer) {
  // Calculate size of each engine in our grid of what's running
  double numAcross = ceil(sqrt(engines_->size()));
  auto winSize = sfWindow_->getSize();
  sf::Vector2f rectsize(winSize.x / numAcross - 5, winSize.y / numAcross - 5);

  // Display stats for each engine in their own box
  int x = 0, y = 0;
  for (auto & e : *engines_) {
    // Draw box representing engine
    sf::RectangleShape engineRect(rectsize);
    engineRect.setFillColor(sf::Color(255, 255, 255));

    sf::Vector2f rectpos;
    rectpos.x = x * (winSize.x / numAcross);
    rectpos.y = y * (winSize.y / numAcross);

    engineRect.setPosition(rectpos);
    sfWindow_->draw(engineRect);

    // Fill box with some simple stats
    unsigned numAlive;
    {
      std::unique_lock<std::mutex> eg(*e.GetVolatileMutex());
      numAlive = e.GetArena()->LifeformsCount();
    }
    sf::Text aliveText(MakeString("%u alive\n", numAlive), *sfFont_, 16);
    aliveText.setColor(sf::Color(0, 0, 0));
    aliveText.setPosition(rectpos.x + 2, rectpos.y + 2);
    sfWindow_->draw(aliveText);

    // Set up for next engine
    if (++x >= numAcross) {
      x = 0;
      y += 1;
    }
  }
}


}  // namespace evol
