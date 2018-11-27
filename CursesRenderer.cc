/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include "CursesRenderer.h"

#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/time.h>

#include <cstdint>
#include <list>

#include "Arena.h"
#include "Timer.h"

namespace evol {


void CursesRenderer::Init() {
  did_init_ = true;
  initscr();
  cbreak();
  noecho();
  nonl();
  nodelay(stdscr, 1);
}


void CursesRenderer::Cleanup() {
  if (did_init_) {
    endwin();
    did_init_ = false;
  }
}


void CursesRenderer::Run() {
  if (!did_init_) {
    abort();
  }

  int us_per_frame = 1e6 / target_fps_;
  struct timeval start_time, end_time;

  for (;;) {
    int key = getch();

    if (key == 27 /* ESC */ || key == 'q' || key == 'Q') {
      // User wants to exit
      break;
    }
    if (gettimeofday(&start_time, nullptr) < 0)
      abort();
    RenderFrame(nullptr);
    if (gettimeofday(&end_time, nullptr) < 0)
      abort();
    int64_t elapsed = (end_time.tv_sec - start_time.tv_sec) * 1e6 + (end_time.tv_usec - start_time.tv_usec);
    if (elapsed < us_per_frame) {
      usleep(us_per_frame - elapsed);
    }
  }
}


void CursesRenderer::RenderFrame(const Timer * poll_timer) {
  // Clear curses screen
  erase();

  int engine_num = 0;
  int line = 0;
  char out[256];

  // Print meta-collection stats at the top (how long it took us to ask all the
  // engines for data)
  if (poll_timer) {
    auto poll_timer_stats = poll_timer->GetStats();
    snprintf(out, sizeof(out), "Poll time: %lld avg us", (long long)poll_timer_stats.us_avg);
    mvaddstr(line, 0, out);
    line += 2;
  }

  // Track total number of living & dead lifeforms across all engines
  uint64_t total_num_alive = 0;
  uint64_t total_num_dead = 0;

  // Keep count of Dna length; this divided by total_num_alive is global average
  // length
  uint64_t total_dna_len = 0;

  // Total loop time is summarized in the footer
  uint64_t loop_time_sum = 0;

  for (EvolEngine & engine : *engines_) {
    uint64_t num_alive = 0;
    uint64_t num_dead = 0;
    uint64_t highest_gen = 0;
    float average_dna_len = 0;
    TimerStats timer_stats;

    {
      // Lock the engine.  Clock is ticking now!
      std::unique_lock<std::mutex> eg(engine.Mutex());

      // Get lifeform count summary
      auto & arena = engine.GetArena();
      num_alive = engine.GetArena().NumLifeforms();
      total_num_alive += num_alive;
      num_dead = arena.NumDeadLifeforms();
      total_num_dead += num_dead;

      // Get average DNA length
      for (auto & lf : arena.Lifeforms()) {
        uint64_t dl = lf->GetDnaSize();
        average_dna_len += dl;
        total_dna_len += dl;
        highest_gen = std::max(highest_gen, lf->Gen());
      }
      average_dna_len /= num_alive;

      timer_stats = engine.GetTimer().GetStats();
    }
    // Engine unlocked; we can do the slow stuff now

    snprintf(out, sizeof(out), "Engine %d", engine_num);
    mvaddstr(line++, 0, out);

    // Print lifeform count summary & hi/lo-water marks
    LifeformWatermarks & lf_stats = engine_stats_[engine_num];
    if (num_alive > lf_stats.lifeforms_count_hiwater)
      lf_stats.lifeforms_count_hiwater = num_alive;
    if (num_alive < lf_stats.lifeforms_count_lowater)
      lf_stats.lifeforms_count_lowater = num_alive;
    snprintf(out, sizeof(out), "Total lifeforms: %llu living (%llu lo, %llu hi); %llu dead",
             static_cast<long long unsigned>(num_alive),
             static_cast<long long unsigned>(lf_stats.lifeforms_count_lowater),
             static_cast<long long unsigned>(lf_stats.lifeforms_count_hiwater),
             static_cast<long long unsigned>(num_dead));
    mvaddstr(line++, 4, out);

    // Print average Dna size
    snprintf(out, sizeof(out), "Lifeform average Dna size: %.2f; hi gen %llu",
             average_dna_len,
             static_cast<long long unsigned>(highest_gen));
    mvaddstr(line++, 4, out);

    // Print engine loop timer stats
    snprintf(out, sizeof(out), "%s time (1e-6s): %llu avg (%llu/sec excl. overhead), %llu min, %llu max",
              timer_stats.description.c_str(),
              static_cast<long long unsigned>(timer_stats.us_avg),
              static_cast<long long unsigned>(1e6 / timer_stats.us_avg),
              static_cast<long long unsigned>(timer_stats.us_min),
              static_cast<long long unsigned>(timer_stats.us_max));
    loop_time_sum += timer_stats.us_avg;
    mvaddstr(line++, 4, out);

    ++line;  // blank space at end
    ++engine_num;
  }
  auto num_engines = engines_->size();
  snprintf(out, sizeof(out), "Avg loop time: %llu 1e-6s",
           static_cast<long long unsigned>(loop_time_sum / num_engines));
  mvaddstr(line, 0, out);
  snprintf(out, sizeof(out), "Total performance: %llu/sec (excl. overhead)",
           static_cast<long long unsigned>(num_engines * num_engines * 1e6 / loop_time_sum));
  mvaddstr(line++, 30, out);

  snprintf(out, sizeof(out), "Total lifeforms: %lu living, %lu dead; %.2f avg Dna size",
           static_cast<long unsigned>(total_num_alive),
           static_cast<long unsigned>(total_num_dead),
           static_cast<float>(total_dna_len) / total_num_alive);
  mvaddstr(line++, 0, out);

  snprintf(out, sizeof(out), "Asteroid: %lu landed, %lu launched, %lu occupying",
           static_cast<long unsigned>(asteroid_->NumLanded()),
           static_cast<long unsigned>(asteroid_->NumLaunched()),
           static_cast<long unsigned>(asteroid_->NumWaiting()));
  mvaddstr(line++, 0, out);

  refresh();
}


}  // namespace evol
