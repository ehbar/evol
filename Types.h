/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2018 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#ifndef EVOL_TYPES_H_
#define EVOL_TYPES_H_

#include <cstdint>
#include <string>
#include <unordered_map>

namespace evol {


enum class ActionType {
  NOTHING = 0,
  APOPTOSIS,
  MOVE_NORTH,
  MOVE_EAST,
  MOVE_SOUTH,
  MOVE_WEST,
};

// Define the range of enum class integers representing moves (for random
// selection or iteration)
const int32_t kActionMoveBegin = static_cast<int>(ActionType::MOVE_NORTH);
const int32_t kActionMoveEnd = static_cast<int>(ActionType::MOVE_WEST);

typedef uint8_t OpcodeBasicType;


enum class OpCode : OpcodeBasicType {
  NOP = 0,    // No action; should always be first and zero

  APOPTOSIS,  // Suicide

  // Conditionals
  IS_NORTH_OCCUPIED,  // Sets/clears cmp flag if north square is/isn't occupied
  IS_SOUTH_OCCUPIED,  // etc.
  IS_WEST_OCCUPIED,
  IS_EAST_OCCUPIED,
  IS_CROWDED,         // Sets cmp flag if current square has other lifeform
  IS_NEIGHBOR,        // Sets cmp flag if any cardinal neighbor is occupied

  JMP1,               // Skips next instruction
  JMP2,               // Skips next two instructions
  JMP3,               // Skips next three instructions
  JMP4,               // etc.
  JMP5,               // etc.

  CJMP1,              // Skips next instruction if cmp flag is set
  CJMP2,              // Skips next two instructions if cmp flag is set
  CJMP3,              // etc.
  CJMP4,              // etc.
  CJMP5,              // etc.

  // These codes always terminate execution of Dna:
  FINAL_MOVE_NORTH,
  FINAL_MOVE_EAST,
  FINAL_MOVE_SOUTH,
  FINAL_MOVE_WEST,
  FINAL_MOVE_RANDOM,

  END  // must always be last
};


// Define the range of opcodes for random generation
const int32_t kOpcodeBegin = static_cast<int32_t>(OpCode::NOP);
const int32_t kOpcodeEnd = static_cast<int32_t>(OpCode::END) - 1;

// Enum-to-string map (for human-readable serialization); this should
// be updated in Types.cc when opcodes are changed
extern const std::unordered_map<OpCode, std::string> kOpcodeStrings;

}  // namespace evol


namespace std {

// Hash template specialization for Opcode enum
template <>
struct hash<evol::OpCode> {
  std::size_t operator()(const evol::OpCode &c) const {
    auto hasher = hash<evol::OpcodeBasicType>();
    return hasher(static_cast<evol::OpcodeBasicType>(c));
  }
};


}  // namespace std
#endif  // EVOL_TYPES_H
