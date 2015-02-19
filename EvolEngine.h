/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_EVOL_ENGINE_H_
#define EVOL_EVOL_ENGINE_H_

#include <stdlib.h>

#include <atomic>
#include <cstdint>
#include <ctime>
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
#include "Timer.h"

namespace evol {


typedef std::unordered_map<Coord, std::vector<Action>> ActionMap;


class EvolEngine {
 public:
  EvolEngine()
      : do_exit_(true), arena_(nullptr), turns_(0), asteroid_(nullptr) {}

  EvolEngine(int width, int height, Asteroid * asteroid = nullptr)
      : do_exit_(false),
        arena_(new Arena(width, height)),
        turns_(0),
        asteroid_(asteroid) {}

  EvolEngine & operator=(EvolEngine && other) {
    if (!do_exit_)
      abort();  // never overwrite a running engine!!

    // Take on value of their do_exit_ and leave it set to true
    do_exit_ = other.do_exit_.exchange(true);

    std::lock(volatile_mutex_, other.volatile_mutex_);
    std::lock_guard<std::mutex> lgt(volatile_mutex_, std::adopt_lock);
    std::lock_guard<std::mutex> lgo(other.volatile_mutex_, std::adopt_lock);

    arena_ = std::move(other.arena_);
    turns_ = other.turns_;
    timers_.clear();  // these don't move; list is kept for export
    asteroid_ = other.asteroid_;
    other.turns_ = 0;
    other.timers_.clear();
    other.asteroid_ = nullptr;

    return *this;
  }

  explicit EvolEngine(const EvolEngine &) = delete;
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
   * Gets volatile mutex (see explanation below).
   */
  std::mutex * GetVolatileMutex() { return &volatile_mutex_; }

  /**
   * Gets const pointer to Arena the engine is using.
   */
  const Arena * GetArena() const { return arena_.get(); }

  /**
   * Gets pointer to the list of timers the engine is using.
   */
  const std::list<const Timer *> * GetTimers() const { return &timers_; }

 private:
  // Loop condition for Run().
  std::atomic<bool> do_exit_;

  std::unique_ptr<Arena> arena_;

  // Number of turns since start of simulation
  uint64_t turns_;

  // List of timers currently in use by the program
  std::list<const Timer *> timers_;

  // The volatile mutex is held whenever the engine reserves the right to change
  // state information (member objects such as arena, lifeforms, etc.).  It will
  // also be held by outside threads which want to read this information, like
  // the renderer.  Because it is a single point of contention every user should
  // strive to minimize the time this lock is held.
  std::mutex volatile_mutex_;

  // External Asteroid object (for moving lifeforms between engines).
  Asteroid * asteroid_;

  /**
   * Post-Dna processing, this method will collate a list of Actions decided by
   * the living lifeforms into a map of coord => [action1, action2, ...].  The
   * map is heavyweight so we return it by pointer.
   */
  std::unique_ptr<ActionMap> MapActions(const std::forward_list<Action> &) const;

  /**
   * Resolve lifeform actions as collated by MapActions.
   */
  void ResolveInteractions(ActionMap *);

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
