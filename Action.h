/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2017 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_ACTION_H_
#define EVOL_ACTION_H_

#include "Lifeform.h"
#include "Types.h"

namespace evol {


/**
 * An Action represents the intention of a Lifeform after a single turn of DNA
 * processing.
 */
struct Action {
 public:
  Action() : actor(nullptr), type(ActionType::NOTHING) {}
  Action(Lifeform * a, ActionType t) : actor(a), type(t) {}

  // Lifeform performing this action
  Lifeform * actor;
  // The thing the lifeform wants to do
  ActionType type;
};


}  // namespace evol
#endif  // EVOL_ACTION_H_
