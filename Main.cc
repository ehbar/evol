/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include <unistd.h>
#include <cstdio>
#include <vector>
#include <thread>

#if EVOL_RENDERER_CURSES
#  include "CursesRenderer.h"
#elif EVOL_RENDERER_SFML
#  include "SFMLRenderer.h"
#endif

#include "Asteroid.h"
#include "EvolEngine.h"
#include "Dumper.h"
#include "Params.h"

using namespace evol;


int main(int argc, char *argv[]) {

#if EVOL_RENDERER_CURSES || EVOL_RENDERER_SFML
  unsigned numCores = Params::kNumEngines;
  if (numCores < 1) {
    numCores = std::thread::hardware_concurrency();
    if (numCores < 1) {
      // Library detection failed
      numCores = 1;
    }
  }
#else
  unsigned numCores = 1;
#endif

  std::vector<EvolEngine> engines(numCores);
  std::vector<std::thread> engine_threads(numCores);
  Asteroid asteroid(Params::kAsteroidSize);

  Coord::SetGlobalBounds(Params::kWidth, Params::kHeight);

  for (unsigned i = 0; i < numCores; ++i) {
    engines[i] = EvolEngine(Params::kWidth, Params::kHeight, &asteroid);
    engines[i].Seed(Params::kStartingLifeforms);
    engine_threads[i] = std::thread(&EvolEngine::Run, &engines[i]);
  }

  // Thread which dumps lifeforms to JSON output every few seconds
  Dumper dumper(&engines, Params::kJsonDumpIntervalSeconds);
#if EVOL_RENDERER_CURSES || EVOL_RENDERER_SFML
  // Dumper is also disabled if no renderer used
  dumper.Start();
#endif

  // Renderer thread; this updates the screen and waits for user quit
#if EVOL_RENDERER_CURSES
  CursesRenderer renderer(&engines, &asteroid, 30);
#elif EVOL_RENDERER_SFML
  SFMLRenderer renderer(&engines, &asteroid, 30);
#endif
#if EVOL_RENDERER_CURSES || EVOL_RENDERER_SFML
  renderer.Init();
  renderer.Run();
#else
  // Fallback for no renderer (useful for gdb)
  for (;;) {
    sleep(1);
  }
#endif

#if EVOL_RENDERER_CURSES || EVOL_RENDERER_SFML
  dumper.DoExit();
#endif

  for (unsigned i = 0; i < numCores; ++i) {
    engines[i].DoExit();
  }

#if EVOL_RENDERER_CURSES || EVOL_RENDERER_SFML
  dumper.JoinThread();
#endif

  for (unsigned i = 0; i < numCores; ++i) {
    engine_threads[i].join();
  }

#if EVOL_RENDERER_CURSES || EVOL_RENDERER_SFML
  renderer.Cleanup();
#endif
  puts("Exiting normally");

  return 0;
}
