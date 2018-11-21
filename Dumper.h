/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_DUMPER_H_
#define EVOL_DUMPER_H_

#include <ctime>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "EvolEngine.h"
#include "LifeformJson.h"

namespace evol {


class Dumper {
 public:
  Dumper() = delete;

  Dumper(std::vector<EvolEngine> * engines, time_t interval = 30)
        : engines_(engines), dump_interval_secs_(interval), do_exit_(false) {
    // Build a lock for each engine
    engine_locks_.resize(engines_->size());
    for (unsigned i = 0; i < engines_->size(); ++i) {
      EvolEngine & e = (*engines_)[i];
      engine_locks_[i] = std::unique_lock<std::mutex>(*(e.GetVolatileMutex()), std::defer_lock);
    }
  }

  /**
   * Start the Dumper thread.  Returns after launching it.
   */
  void Start() {
    std::lock_guard<std::mutex> lg(do_exit_mutex_);
    do_exit_ = false;
    thread_ = std::thread(&Dumper::DumpLoop, this);
  }

  /**
   * Tell Dumper thread to exit gracefully.
   */
  void DoExit() {
    {
      std::lock_guard<std::mutex> lg(do_exit_mutex_);
      do_exit_ = true;
    }
    do_exit_cv_.notify_all();
  }

  /**
   * Joins Dumper thread; call after DoExit() to ensure last data dump is
   * finished.
   */
  void JoinThread() {
    if (thread_.joinable()) {
      thread_.join();
    }
  }

  /**
   * Sleeps dump_interval_secs_ waiting for do_exit_ to be set.  Returns
   * do_exit_ status.
   */
  bool DidGetExitAfterDelay() {
    std::unique_lock<std::mutex> lk(do_exit_mutex_);
    auto now = std::chrono::system_clock::now();
    if (do_exit_cv_.wait_until(lk, now + std::chrono::seconds(dump_interval_secs_),
                               [this](){ return do_exit_; })) {
      // do_exit_ is set
      return true;
    }
    // do_exit_ is not set, we just timed out
    return false;
  }

  void DumpLoop();

 private:
  void DumpAllEngines();

  // Engine hooks
  std::vector<EvolEngine> * engines_;
  std::vector<std::unique_lock<std::mutex>> engine_locks_;

  // Misc runtime state
  time_t dump_interval_secs_;
  bool do_exit_;
  std::mutex do_exit_mutex_;
  std::condition_variable do_exit_cv_;
  std::thread thread_;
  std::unique_ptr<char[]> filename_;
};


}  // namespace evol
#endif  // EVOL_DUMPER_H_
