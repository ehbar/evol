/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2017 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include <vector>
#include <thread>

#include "Asteroid.h"
//#include "CursesRenderer.h"
#include "SFMLRenderer.h"
#include "EvolEngine.h"
#include "Dumper.h"
#include "Params.h"

using namespace evol;


int main(int argc, char *argv[]) {
  unsigned numCores = Params::kNumEngines;
  if (numCores < 1) {
    numCores = std::thread::hardware_concurrency();
    if (numCores < 1) {
      // Library detection failed
      numCores = 1;
    }
  }

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
  dumper.Start();

  // Renderer thread; this updates the screen and waits for user quit
  //CursesRenderer renderer(&engines, &asteroid, 30);
  SFMLRenderer renderer(&engines, &asteroid, 30);
  renderer.Init();
  renderer.Run();

  // renderer.Run() returns when user has pressed a key to quit

  dumper.DoExit();
  for (unsigned i = 0; i < numCores; ++i) {
    engines[i].DoExit();
  }
  dumper.JoinThread();
  for (unsigned i = 0; i < numCores; ++i) {
    engine_threads[i].join();
  }

  renderer.Cleanup();
  return 0;
}
