/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include "Random.h"

#include <memory>
#include <random>

namespace evol {


std::unique_ptr<std::default_random_engine> Random::rand_generator_(nullptr);


}  // namespace evol
