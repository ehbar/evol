/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_EVOL_ENGINE_H_
#define EVOL_EVOL_ENGINE_H_

#include <stdlib.h>
#include <atomic>
#include <cstdint>
#include <forward_list>
#include <list>
#include <memory>
#include <mutex>
#include <vector>
#include <unordered_map>

#include "Action.h"
#include "Asteroid.h"
#include "Arena.h"
#include "Params.h"
#include "Random.h"
#include "Timer.h"

namespace evol {


typedef std::unordered_map<Coord, std::vector<Action>> ActionMap;

class EvolEngine {
 public:
  EvolEngine()
      : do_exit_(true),
        random_(nullptr),
        arena_(nullptr),
        asteroid_(nullptr),
        turns_(0),
        timer_() {}

  EvolEngine(int width, int height, std::shared_ptr<Asteroid> asteroid)
      : do_exit_(false),
        random_(nullptr),
        asteroid_(asteroid),
        turns_(0),
        timer_("Main loop") {
    random_ = std::make_shared<Random>();
    arena_.reset(new Arena(width, height, random_));
  }

  EvolEngine & operator=(EvolEngine && other) {
    if (!do_exit_)
      abort();  // never overwrite a running engine!!

    // Take on value of their do_exit_ and leave theirs set to true
    do_exit_ = other.do_exit_.exchange(true);

    std::lock(mutex_, other.mutex_);
    std::lock_guard<std::mutex> lgt(mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> lgo(other.mutex_, std::adopt_lock);

    random_ = std::move(other.random_);
    arena_ = std::move(other.arena_);
    asteroid_ = std::move(other.asteroid_);
    std::swap(turns_, other.turns_);
    timer_ = std::move(other.timer_);

    return *this;
  }

  EvolEngine(const EvolEngine &) = delete;
  EvolEngine & operator=(const EvolEngine & other) = delete;

  /**
   * Set exit flag
   */
  void DoExit() {
    do_exit_ = true;
  }

  /**
   * Loads a given number of lifeforms into random coordinates on the arena.
   */
  void Seed(unsigned num_lifeforms);

  /**
   * Begins simulation.  Will not exit until do_exit_ is set.
   */
  void Run();

  /**
   * Returns engine mutex (see explanation below).
   */
  std::mutex & Mutex() { return mutex_; }

  /**
   * Returns const reference to engine arena.
   */
  const Arena & GetArena() const { return *arena_.get(); }

  /**
   * Returns const reference to the timer created by the engine.
   */
  const Timer & GetTimer() const { return timer_; }

 private:
  // Loop condition for Run().
  std::atomic<bool> do_exit_;

  // Thread-local random number generator
  std::shared_ptr<Random> random_;

  // Game pieces: The arena
  std::unique_ptr<Arena> arena_;
  // The asteroid exchanges lifeforms between engines
  std::shared_ptr<Asteroid> asteroid_;

  // Number of turns since start of simulation
  uint64_t turns_;

  // Main loop timer
  Timer timer_;

  /**
   * The engine mutex is held whenever the engine reserves the right to
   * change state information (member objects such as arena, lifeforms, etc.).
   * It will also be held by outside threads which want to read this
   * information, like the renderer.  Because it is a single point of
   * contention, every user should strive to minimize the time this lock is
   * held.
   */
  std::mutex mutex_;

  /**
   * Post-Dna processing, this method will collate a list of Actions decided by
   * the living lifeforms into a map of coord => [action1, action2, ...].  The
   * map is heavyweight so we return it by pointer.
   */
  ActionMap MapActions(const std::forward_list<Action> &) const;

  /**
   * Resolve lifeform actions as collated by MapActions.
   */
  void ResolveInteractions(ActionMap &);

  /**
   * Calculate the total energy available to lifeforms on the grid, and apply
   * this to what's alive.
   */
  void ApplyEnergyLevelsToLifeforms();

  /**
   * lifeforms with energy <= 0.0 are kill
   */
  void KillStarvedLifeforms();

  /**
   * lifeforms with energy >= Params::kMeiosisLevel form babby
   */
  void SplitFatLifeforms();
};


}  // namespace evol
#endif  // EVOL_EVOL_ENGINE_H_
