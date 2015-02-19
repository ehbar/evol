/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_ASTEROID_H_
#define EVOL_ASTEROID_H_

#include <memory>
#include <mutex>
#include <vector>

#include "Lifeform.h"
#include "Random.h"

namespace evol {


/**
 * The "Asteroid" is a container for sharing lifeforms between engines; every so
 * often an engine will offer up one of its lifeforms to this queue, and every
 * so often an engine will grab a lifeform back.  This encourages more rapid
 * development between arenas.
 *
 * The analogy is to an asteroid which may (or may not) have brought life to the
 * Earth, i.e. the theory of panspermia.
 */
class Asteroid {
 public:
  Asteroid(unsigned max_size) : max_size_(max_size), landed_(0), launched_(0) {
    lifeforms_.reserve(max_size_);
  }

  /**
   * Put the given lifeform on the "asteroid."  This method takes memory
   * ownership of the lifeform.
   */
  void LaunchLifeform(std::unique_ptr<Lifeform> lf) {
    std::lock_guard<std::mutex> lg(lifeforms_mutex_);

    if (lifeforms_.size() >= max_size_) {
      // Vector is full, just swap out a random lifeform already on the
      // asteroid.  The lifeform so removed will be erased.
      int32_t i = Random::Int32(0, lifeforms_.size() - 1);
      lifeforms_[i].swap(lf);
      lf.reset(nullptr);
    } else {
      lifeforms_.push_back(std::move(lf));
    }
    ++launched_;
  }

  /**
   * Land a random lifeform from the "asteroid."  Returns the lifeform, or
   * nullptr if the asteroid is empty.
   */
  std::unique_ptr<Lifeform> LandLifeform() {
    std::lock_guard<std::mutex> lg(lifeforms_mutex_);

    if (lifeforms_.empty()) {
      return nullptr;
    }

    int32_t i = Random::Int32(0, lifeforms_.size() - 1);
    std::unique_ptr<Lifeform> lf = std::move(lifeforms_[i]);
    lifeforms_.erase(lifeforms_.begin() + i);
    ++landed_;
    return lf;
  }

  uint32_t NumLanded() {
    std::lock_guard<std::mutex> lg(lifeforms_mutex_);
    return landed_;
  }
  uint32_t NumLaunched() {
    std::lock_guard<std::mutex> lg(lifeforms_mutex_);
    return launched_;
  }
  uint32_t NumWaiting() {
    std::lock_guard<std::mutex> lg(lifeforms_mutex_);
    return lifeforms_.size();
  }

 private:
  const unsigned max_size_;
  std::mutex lifeforms_mutex_;
  std::vector<std::unique_ptr<Lifeform>> lifeforms_;

  uint32_t landed_;
  uint32_t launched_;
};



}  // namespace evol
#endif  // EVOL_ASTEROID_H_
