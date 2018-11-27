/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_LIFEFORM_H_
#define EVOL_LIFEFORM_H_

#include <atomic>
#include <cstdint>
#include <memory>
#include <vector>

#include "Coord.h"
#include "Random.h"
#include "Types.h"

namespace evol {


typedef std::vector<OpCode> Dna;

class LifeformImpl;

typedef std::shared_ptr<LifeformImpl> Lifeform;

/**
 * Convenience function returns a new lifeform.
 */
inline Lifeform make_lifeform(uint64_t gen, Dna dna, std::shared_ptr<Random> random) {
  return std::make_shared<LifeformImpl>(gen, dna, random);
}


class LifeformImpl {
 public:
  // Must have thread-local random number generator
  LifeformImpl() = delete;
  LifeformImpl(const Lifeform &o) = delete;

  LifeformImpl(uint64_t gen, const Dna & dna, std::shared_ptr<Random> random)
      : gen_(gen),
        alive_(true),
        energy_(1.0),
        coord_(),
        dna_(dna),
        random_(random) {
    id_ = ++LifeformImpl::next_id_;
  }

  LifeformImpl & operator=(const LifeformImpl & o) {
    id_ = o.id_;
    gen_ = o.gen_;
    alive_ = o.alive_;
    energy_ = o.energy_;
    coord_ = o.coord_;
    dna_ = o.dna_;
    random_ = o.random_;
    return *this;
  }

  uint64_t Id() const { return id_; }
  uint64_t Gen() const { return gen_; }

  bool Alive() const { return alive_; }

  void SetKilled() { alive_ = false; }

  /**
   * Return a new lifeform with Dna equal to the current instance.
   */
  Lifeform MakeChild() const {
    return std::make_shared<LifeformImpl>(gen_ + 1, dna_, random_);
  }

  /**
   * Mutate the Dna of the current lifeform by inserting, deleting, changing, or
   * translating one or more opcodes.
   */
  void Mutate();

  /**
   * Return the ActionType selected by the lifeform's Dna code for its current
   * situation.  This is the method that actually executes the Dna.  Its
   * only argument is a pointer to the Arena.
   * 
   * TODO: This is a void * because we can't include Arena.h due to circular
   * deps.  We should fix this.  Might be sane to move DNA processing to its
   * own class, some kind of lifeform VM maybe.
   */
  ActionType RunDna(void *);

  /**
   * Return copy of the organism's Dna code.
   */
  Dna GetDna() const { return dna_; }

  /**
   * Return size of the organism's Dna code.
   */
  size_t GetDnaSize() const { return dna_.size(); }

  void SetCoord(const Coord & c) {
    coord_ = c;
  }
  Coord GetCoord() const {
    return coord_;
  }

  void SetEnergy(int32_t new_e) {
    energy_ = new_e;
  }
  float GetEnergy() const {
    return energy_;
  }

  bool operator==(const LifeformImpl & o) const {
    return Id() == o.Id();
  }
  bool operator!=(const LifeformImpl & o) const {
    return Id() != o.Id();
  }

 private:
  // See Lifeform.cc for explanations
  void MutateInsert(int32_t, int32_t);
  void MutateDelete(int32_t, int32_t);
  void MutateChange(int32_t, int32_t);
  void MutateTranslate(int32_t, int32_t);

  static std::atomic<uint64_t> next_id_;

  uint64_t id_;
  uint64_t gen_;
  bool alive_;
  float energy_;
  Coord coord_;
  Dna dna_;
  std::shared_ptr<Random> random_;
};


}  // namespace evol
#endif  // EVOL_LIFEFORM_H_
