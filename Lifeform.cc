/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2015 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include "Lifeform.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>  // abort()

#include "Arena.h"
#include "Params.h"
#include "Random.h"


namespace evol {


uint64_t Lifeform::next_id_ = 1;


namespace {

// Flag / registers for Dna execution
typedef uint8_t Flag;

// This bit is set for various CMP opcodes
constexpr uint8_t kCmpFlag = 0x01;


/**
 * Set/clear the given bit in the given flag type based on the boolean
 * value of the predicate.
 */
inline void set_flag(Flag & flags, const Flag & bit, bool pred = true) {
  if (pred)
    flags |= bit;
  else
    flags &= ~bit;
}

/**
 * Gets the flag value.
 */
inline bool get_flag(const Flag & flags, const Flag & bit) {
  return flags & bit;
}

}  // anon namespace


ActionType Lifeform::RunDna(const void *arena__) {
  // This ugly cast is because of a circular dependency in Lifeform and Arena
  const Arena *arena = static_cast<const Arena *>(arena__);

  if (dna_.empty()) {
    // A lifeform with no Dna can't survive
    return ActionType::APOPTOSIS;
  }

  uint8_t flags = 0;

  for (auto opcode = dna_.cbegin(); opcode < dna_.cend(); opcode++) {
    switch (*opcode) {
      // Basic NOP case, do nothing
      case OpCode::NOP:
        continue;
      // Suicide
      case OpCode::APOPTOSIS:
        return ActionType::APOPTOSIS;

      // Set cmp flags based on whether targeted square is occupied
      case OpCode::IS_NORTH_OCCUPIED:
        set_flag(flags, kCmpFlag, arena->GetLifeformCount(coord_.North()));
        continue;
      case OpCode::IS_SOUTH_OCCUPIED:
        set_flag(flags, kCmpFlag, arena->GetLifeformCount(coord_.South()));
        continue;
      case OpCode::IS_EAST_OCCUPIED:
        set_flag(flags, kCmpFlag, arena->GetLifeformCount(coord_.East()));
        continue;
      case OpCode::IS_WEST_OCCUPIED:
        set_flag(flags, kCmpFlag, arena->GetLifeformCount(coord_.West()));
        continue;

      // Set cmp flag if local tile has other lifeforms
      case OpCode::IS_CROWDED:
        set_flag(flags, kCmpFlag, arena->GetLifeformCount(coord_) > 1);
        continue;
      // Set cmp flag if adjacent tiles have other lifeforms (local is ignored)
      case OpCode::IS_NEIGHBOR:
        set_flag(flags, kCmpFlag, !arena->GetAdjacentLifeforms(coord_).empty());
        continue;

      // Final moves, return action
      case OpCode::FINAL_MOVE_NORTH:
        return ActionType::MOVE_NORTH;
      case OpCode::FINAL_MOVE_EAST:
        return ActionType::MOVE_EAST;
      case OpCode::FINAL_MOVE_SOUTH:
        return ActionType::MOVE_SOUTH;
      case OpCode::FINAL_MOVE_WEST:
        return ActionType::MOVE_WEST;
      case OpCode::FINAL_MOVE_RANDOM:
        // Pick random direction
        energy_ -= Params::kRandomMoveCost;  // TODO: This shouldn't be in the DNA processing
        return static_cast<ActionType>(Random::Int32(kActionMoveBegin, kActionMoveEnd));

      // JMP & friends
      case OpCode::JMP1:
        opcode += 1;
        continue;
      case OpCode::JMP2:
        opcode += 2;
        continue;
      case OpCode::JMP3:
        opcode += 3;
        continue;
      case OpCode::JMP4:
        opcode += 4;
        continue;
      case OpCode::JMP5:
        opcode += 5;
        continue;
      case OpCode::CJMP1:
        if (get_flag(flags, kCmpFlag))
          opcode += 1;
        continue;
      case OpCode::CJMP2:
        if (get_flag(flags, kCmpFlag))
          opcode += 2;
        continue;
      case OpCode::CJMP3:
        if (get_flag(flags, kCmpFlag))
          opcode += 3;
        continue;
      case OpCode::CJMP4:
        if (get_flag(flags, kCmpFlag))
          opcode += 4;
        continue;
      case OpCode::CJMP5:
        if (get_flag(flags, kCmpFlag))
          opcode += 5;
        continue;
      default:
        // We should handle all cases in this for loop; not doing so is fatal
        abort();
    }
  }

  // Dna execution ended without a FINAL_* action
  return ActionType::NOTHING;
}


void Lifeform::Mutate() {
  // - Decide how many mutations to perform
  // - For each mutation:
  //   - Decide quantity N, 1 <= N <= L where L is the upper limit
  //     Params::kMaxMutationLength
  //   - Do that mutation

  int mutations = 0;
  int32_t d100 = Random::Int32(0, Params::kMutationDieRoll);
  if (d100 >= Params::kOneMutation)
    mutations = 1;
  if (d100 >= Params::kTwoMutations)
    mutations = 2;

  for (int i = 0; i < mutations; i++) {
    int32_t mutation_start = Random::Int32(0, dna_.size());
    // randomly generated mutation_len must be guaranteed never to be past the
    // end of Dna -- this greatly simplifies the mutation implementations
    int32_t mutation_len = Random::Int32(0, std::min(Params::kMaxMutationLength, static_cast<int32_t>(dna_.size()) - mutation_start));
    if (mutation_len < 1 || mutation_start == static_cast<int32_t>(dna_.size()))
      return;
    int32_t mutation_type = Random::Int32(0, 3);
    switch (mutation_type) {
      case 0:
        MutateInsert(mutation_len, mutation_start);
        break;
      case 1:
        MutateDelete(mutation_len, mutation_start);
        break;
      case 2:
        MutateChange(mutation_len, mutation_start);
        break;
      case 3:
        MutateTranslate(mutation_len, mutation_start);
        break;
      default:
        // Should never happen
        abort();
    }
  }
}


/**
 * Insert DNA at the given offset with the given length.  All codes
 * so inserted are OpCode::NOP.
 */
void Lifeform::MutateInsert(int32_t mutation_len, int32_t mutation_start) {
  dna_.insert(dna_.begin() + mutation_start, mutation_len, OpCode::NOP);
}


/**
 * Delete the given length of Dna at the given offset.
 */
void Lifeform::MutateDelete(int32_t mutation_len, int32_t mutation_start) {
  auto start = dna_.begin() + mutation_start;
  dna_.erase(start, start + mutation_len);
}


/**
 * Scramble Dna starting at the given offset with the given length.
 */
void Lifeform::MutateChange(int32_t mutation_len, int32_t mutation_start) {
  auto start = dna_.begin() + mutation_start;
  auto end = start + mutation_len;
  for (auto oc = start; oc < end; oc++) {
    *oc = static_cast<OpCode>(Random::Int32(kOpcodeBegin, kOpcodeEnd));
  }
}


/**
 * Swap Dna at the given location with another random location.  These can
 * overlap.
 */
void Lifeform::MutateTranslate(int32_t mutation_len, int32_t mutation_start) {
  auto p_start = dna_.begin() + mutation_start;
  auto p_end = p_start + mutation_len;

  auto s_start = dna_.begin() + Random::Int32(0, dna_.size() - mutation_len);
  if (s_start == p_start)
    return;
  auto s_end = s_start + mutation_len;

  // Copy p vector to temp space
  Dna tmp(p_start, p_end);
  // Overwrite p vector with s vector
  for (auto s = s_start, p = p_start; s != s_end && p != p_end; s++, p++) {
    *p = *s;
  }
  // Overwrite old s vector with previously saved p vector
  for (auto s = s_start, p = tmp.begin(); s != s_end && p != tmp.end(); s++, p++) {
    *s = *p;
  }
}


}  // namespace evol;
