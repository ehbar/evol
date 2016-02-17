/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_LIFEFORM_H_
#define EVOL_LIFEFORM_H_

#include <cstdint>
#include <vector>

#include "Coord.h"
#include "Types.h"

namespace evol {


typedef std::vector<OpCode> Dna;


class Lifeform {
 public:
  Lifeform(uint64_t gen, const Dna & dna)
      : gen_(gen), alive_(true), dna_(dna), energy_(1.0) {
    id_ = ++Lifeform::next_id_;
  }

  Lifeform(const Lifeform &) = delete;
  Lifeform & operator=(const Lifeform &) = delete;

  uint64_t Id() const { return id_; }
  uint64_t Gen() const { return gen_; }
  bool Alive() const { return alive_; }
  void SetKilled() { alive_ = false; }

  /**
   * Return a new lifeform with Dna equal to the current instance.  The caller
   * owns the newly created Lifeform and should probably put it in a unique_ptr.
   * Or they could just leak it.  Some men just want to see the memory fill.
   */
  Lifeform * MakeChild() {
    return new Lifeform(gen_ + 1, dna_);
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
  ActionType RunDna(const void *);

  /**
   * Return copy of the organism's Dna code.
   */
  Dna GetDna() { return dna_; }

  /**
   * Return size of the organism's Dna code.
   */
  size_t GetDnaSize() { return dna_.size(); }

  void SetCoord(const Coord & c) {
    coord_ = c;
  }
  Coord GetCoord() {
    return coord_;
  }

  void SetEnergy(int32_t new_e) {
    energy_ = new_e;
  }
  float GetEnergy() {
    return energy_;
  }

 private:
  // See Lifeform.cc for explanations
  void MutateInsert(int32_t, int32_t);
  void MutateDelete(int32_t, int32_t);
  void MutateChange(int32_t, int32_t);
  void MutateTranslate(int32_t, int32_t);

  static uint64_t next_id_;

  uint64_t id_;
  uint64_t gen_;
  bool alive_;
  Dna dna_;

  // Current energy value of the lifeform
  float energy_;

  // Current coord of the lifeform
  Coord coord_;
};


}  // namespace evol
#endif  // EVOL_LIFEFORM_H_
