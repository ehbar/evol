/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_TIMER_H_
#define EVOL_TIMER_H_

#include <stdlib.h>
#include <sys/time.h>

#include <algorithm>  // std::min/max
#include <array>
#include <cstdint>
#include <string>

namespace evol {


/**
 * Various data returned by the Timer::GetStats() method.
 */
struct TimerStats {
  TimerStats() : sample_count(0), us_min(0), us_max(0), us_avg(0) {}

  std::string description;
  int64_t sample_count;  // number of samples taken
  int64_t us_min;        // shortest sample time (microseconds)
  int64_t us_max;        // longest sample time (microseconds)
  int64_t us_avg;        // average sample time (microseconds)
};


/**
 * The Timer class is used to collect and digest statistics on a timed portion
 * of the program.  A different instance should be used for each code section to
 * be measured.  Somewhat Linux-specific.
 */
class Timer {
 public:
  Timer() : sample_count_(0), sample_index_(0) {}
  Timer(const std::string & desc) : description_(desc), sample_count_(0), sample_index_(0) {}

  explicit Timer(const Timer &) = delete;
  explicit Timer(Timer &&) = delete;

  void StartCollection() {
    if (gettimeofday(&start_, nullptr) < 0) {
      abort();
    }
  }

  void EndCollection() {
    struct timeval end;

    if (gettimeofday(&end, nullptr) < 0)
      abort();

    int64_t us = (end.tv_sec * 1e6 + end.tv_usec) - (start_.tv_sec * 1e6 + start_.tv_usec);
    if (us < 0)
      abort();  // time travel?!
    samples_[sample_index_] = us;

    ++sample_index_;
    if (sample_index_ >= kCircularBufferSamples) {
      // Circular buffer end reached, wrap to front
      sample_index_ -= kCircularBufferSamples;
    }

    ++sample_count_;
    if (sample_count_ >= kCircularBufferSamples) {
      // We'll never have more samples than the buffer size
      sample_count_ = kCircularBufferSamples;
    }
  }

  /**
   * Returns min/max/average time for the samples currently in memory.  Calling
   * this iterates the sample buffer so it's pretty heavy.  You may call
   * GetStats() even if the timer is currently collecting.
   */
  TimerStats GetStats() const {
    TimerStats stats;

    if (sample_count_ < 1) {
      // No samples collected, so skip the math
      return stats;
    }

    int64_t sum = 0, min = INT64_MAX, max = INT64_MIN;
    for (int i = 0; i < sample_count_; ++i) {
      sum += samples_[i];
      min = std::min(min, samples_[i]);
      max = std::max(max, samples_[i]);
    }
    if (sum < 0 || min < 0 || max < 0)
      abort();

    stats.description = description_;
    stats.sample_count = sample_count_;
    stats.us_min = min;
    stats.us_max = max;
    stats.us_avg = sum / sample_count_;

    return stats;
  }


 private:
  static constexpr int kCircularBufferSamples = 1000;

  std::string description_;
  struct timeval start_;
  int sample_count_;
  int sample_index_;
  std::array<int64_t, kCircularBufferSamples> samples_;
};


}  // namespace evol
#endif  // EVOL_TIMER_H_
