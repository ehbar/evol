/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
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
#include <mutex>
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
 *
 * This class is designed to be very fast under concurrent access.  It uses a
 * per-instance mutex, which is locked as short a time as possible while data
 * is copied in or out.
 */
class Timer {
 public:
  Timer() : sample_count_(0), sample_index_(0) {}
  Timer(const std::string & desc) : description_(desc), sample_count_(0), sample_index_(0) {}

  Timer(const Timer &o) {
    std::lock_guard our_lock(mutex_);
    std::lock_guard their_lock(o.mutex_);

    description_ = o.description_;
    start_ = o.start_;
    sample_count_ = o.sample_count_;
    sample_index_ = o.sample_index_;
    samples_ = o.samples_;
  }

  Timer & operator=(const Timer &o) {
    std::lock_guard our_lock(mutex_);
    std::lock_guard their_lock(o.mutex_);

    description_ = o.description_;
    start_ = o.start_;
    sample_count_ = o.sample_count_;
    sample_index_ = o.sample_index_;
    samples_ = o.samples_;

    return *this;
  }

  void StartCollection() {
    timeval now;
    if (gettimeofday(&now, nullptr) < 0)
      abort();

    {
      // Lock block
      std::lock_guard<std::mutex> lg_(mutex_);
      start_ = now;
    }
  }

  void EndCollection() {
    struct timeval end;

    if (gettimeofday(&end, nullptr) < 0)
      abort();

    {
      // Lock block
      std::lock_guard<std::mutex> lg_(mutex_);

      int64_t us = (end.tv_sec * 1e6 + end.tv_usec) - (start_.tv_sec * 1e6 + start_.tv_usec);
      if (us < 0) {
        // Clock changed, skip this sample
        return;
      }

      samples_[sample_index_] = us;
      if (++sample_index_ >= kCircularBufferSamples) {
        // Circular buffer end reached, wrap to front
        sample_index_ -= kCircularBufferSamples;
      }
      if (++sample_count_ >= kCircularBufferSamples) {
        // We'll never have more samples than the buffer size
        sample_count_ = kCircularBufferSamples;
      }
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

    // Copy samples and count out of the instance as quickly as possible
    // since we hold the mutex while doing so
    decltype(samples_) samples;
    decltype(sample_count_) sample_count;
    {
      // Lock block
      std::lock_guard<std::mutex> lg_(mutex_);
      samples = samples_;
      sample_count = sample_count_;
    }

    int64_t sum = 0, min = INT64_MAX, max = INT64_MIN;
    for (int i = 0; i < sample_count; ++i) {
      sum += samples_[i];
      min = std::min(min, samples[i]);
      max = std::max(max, samples[i]);
    }
    if (sum < 0 || min < 0 || max < 0)
      abort();

    stats.description = description_;  // this is not in the mutex but we do not expect it to change
    stats.sample_count = sample_count;
    stats.us_min = min;
    stats.us_max = max;
    stats.us_avg = sum / sample_count;

    return stats;
  }


 private:
  static constexpr int kCircularBufferSamples = 1000;

  std::string description_;
  struct timeval start_;
  int sample_count_;
  int sample_index_;
  mutable std::mutex mutex_;
  std::array<int64_t, kCircularBufferSamples> samples_;
};


}  // namespace evol
#endif  // EVOL_TIMER_H_
