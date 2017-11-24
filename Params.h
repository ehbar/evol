/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2017 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_PARAMS_H_
#define EVOL_PARAMS_H_

#include <cstdint>
#include <functional>  // std::hash

namespace evol {


namespace Params {
/* Main.cc */

  ////////////////////////////////////////////////////////////////////////////
  // Engine and arena configs

  // Number of engines to run; 0 = autodetect
  static constexpr int kNumEngines = 0;

  // Arena dimensions
  static constexpr int kWidth = 64;
  static constexpr int kHeight = 64;

  // Number of starting lifeforms to seed
  static constexpr int kStartingLifeforms = 10;

  ////////////////////////////////////////////////////////////////////////////
  // Asteroid settings

  // Max number of lifeforms that can be on asteroid
  static constexpr int kAsteroidSize = 128;

  // Interval in # of turns when we should launch/land a lifeform from any
  // given engine.  0 means never
  static constexpr uint64_t kLifeformAsteroidLaunchInterval = 12000;
  static constexpr uint64_t kLifeformAsteroidLandInterval = 13000;

  ////////////////////////////////////////////////////////////////////////////
  // Lifeform energy costs

  // Base energy deducted per lifeform per turn (i.e. metabolism energy use)
  static constexpr float kCostOfLiving = 2.0;

  // Energy cost of each Dna opcode per turn; added to cost of living
  static constexpr float kCostOfOpcode = 0.01;

  // Energy level required to split
  static constexpr float kMeiosisLevel = 200.0;

  // Energy level lost after split
  static constexpr float kMeiosisCost = 25.0;

  // Energy cost of random move
  static constexpr float kRandomMoveCost = 1.0;

  ////////////////////////////////////////////////////////////////////////////
  // JSON dump settings

  // Time interval for JSON dump of extant lifeforms
  static constexpr int kJsonDumpIntervalSeconds = 60;

  ////////////////////////////////////////////////////////////////////////////
  // Lifeform Dna code parameters

  // Maximum number of Dna opcodes affected in adds/deletes/transmutations
  static constexpr int32_t kMaxMutationLength = 9;

  // Max range of mutation-check die roll; min is 0.  E.g. 99 means range
  // [0, 99]
  static constexpr int32_t kMutationDieRoll = 99;

  // One mutation if 98 > die rolled >= 93 (5%)
  static constexpr int32_t kOneMutation = 93;    // one mutation if die >= 93 (5%)
  // Two mutations if die rolled >= 98 (2%)
  static constexpr int32_t kTwoMutations = 98;   // two mutations if die >= 98  (2%)
}  // namespace Params


}  // namespace std
#endif  // EVOL_PARAMS_H_
