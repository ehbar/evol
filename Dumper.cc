/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
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


/**
 * Entry point and main loop for the Dumper thread.
 */
void Dumper::DumpLoop() {
  std::unique_lock<std::mutex> lck(do_exit_mutex_, std::defer_lock);
  std::vector<Lifeform *> all_lifeforms;

  // For some stupid reason, json_object_to_file_ext doesn't take a const
  // arg, so we have to put the filename in a mutable space.  Leave some
  // space in case it does something unholy like add a file extension
  char filename[256];
  strcpy(filename, "lifeform-dump.json");  // TODO: Make this a CLI flag

  // Assemble our vector of engine locks
  std::vector<std::unique_lock<std::mutex>> engine_locks(engines_->size());
  for (unsigned i = 0; i < engines_->size(); ++i) {
    EvolEngine & e = (*engines_)[i];
    engine_locks[i] = std::unique_lock<std::mutex>(*(e.GetVolatileMutex()), std::defer_lock);
  }

  for (;;) {
    if (DidGetExitAfterDelay())
      break;

    // Lock all engines and grab their lifeforms.  We leave the engines locked
    // since we're only getting pointers to the lifeforms and they'll still need
    // to be converted to libjson-c objects.
    //
    // TODO: Copy the lifeforms before JSON-ifying them so we can release the
    // engine locks sooner.
    //
    for (unsigned i = 0; i < engines_->size(); ++i) {
      engine_locks[i].lock();
      const Arena * arena = (*engines_)[i].GetArena();
      auto arena_lifeforms = arena->Lifeforms();
      all_lifeforms.insert(all_lifeforms.end(), arena_lifeforms.cbegin(), arena_lifeforms.cend());
    }

    auto json_lifeform_array = JsonifyLifeforms(all_lifeforms.cbegin(), all_lifeforms.cend());
    for (auto & lck : engine_locks) {
      lck.unlock();
    }
    all_lifeforms.clear();
    json_object_to_file_ext(filename, json_lifeform_array.get(),
                            JSON_C_TO_STRING_PRETTY | JSON_C_TO_STRING_NOZERO);
  }
}


}  // namespace evol
