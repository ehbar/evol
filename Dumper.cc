/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include "Dumper.h"

#include <cstring>
#include <chrono>
#include <mutex>
#include <vector>

#include "Arena.h"
#include "EvolEngine.h"


namespace evol {


static const char * kDumperFilename = "lifeform-dump.json";


/**
 * Entry point and main loop for the Dumper thread.
 */
void Dumper::DumpLoop() {
  for (;;) {
    if (DidGetExitAfterDelay()) {
      break;
    }
    DumpAllEngines();
  }

  // Final data dump before we exit
  DumpAllEngines();
}


/**
 * Lock all engines and grab their lifeforms.  We leave the engines locked
 * since we're only getting pointers to the lifeforms and they'll still need
 * to be converted to libjson-c objects.
 *
 * TODO: Copy the lifeforms before JSON-ifying them so we can release the
 * engine locks sooner.
 */
void Dumper::DumpAllEngines() {
  std::vector<Lifeform *> all_lifeforms;

  // Copy lifeform list from each engine
  for (size_t i = 0; i < engines_->size(); ++i) {
    engine_locks_[i].lock();
    const Arena * arena = (*engines_)[i].GetArena();
    auto arena_lifeforms = arena->Lifeforms();
    all_lifeforms.insert(all_lifeforms.end(), arena_lifeforms.cbegin(), arena_lifeforms.cend());
  }

  // Jsonify lifeforms
  auto json_lifeform_array = JsonifyLifeforms(all_lifeforms.cbegin(), all_lifeforms.cend());

  // Release engines
  for (auto & lck : engine_locks_) {
    lck.unlock();
  }

  // libjson's functions require non-const filename arg :/
  constexpr size_t kMaxFilename = 256;
  char filename[kMaxFilename];
  strncpy(filename, kDumperFilename, kMaxFilename);
  json_object_to_file_ext(filename, json_lifeform_array.get(),
                          JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOZERO);
}


}  // namespace evol
