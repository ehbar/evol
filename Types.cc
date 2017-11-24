/*
 * Evol: The non-life evolution simulator.
 *
 * Copyright 2014-2017 Eric Barrett <arctil@gmail.com>.
 *
 * This program is distributed under the terms of the GNU General Public
 * License Version 3.  See file `COPYING' for details.
 */

#include "Types.h"

#include <string>
#include <unordered_map>

namespace evol {


// E.g.
//   OC(NOP)
// becomes
//   { OpCode::NOP, "NOP" }
//
#define OC(x) { (OpCode::x), #x }

const std::unordered_map<OpCode, std::string> kOpcodeStrings {
  OC(NOP),
  OC(APOPTOSIS),
  OC(IS_NORTH_OCCUPIED),
  OC(IS_SOUTH_OCCUPIED),
  OC(IS_EAST_OCCUPIED),
  OC(IS_WEST_OCCUPIED),
  OC(IS_CROWDED),
  OC(IS_NEIGHBOR),
  OC(JMP1),
  OC(JMP2),
  OC(JMP3),
  OC(JMP4),
  OC(JMP5),
  OC(CJMP1),
  OC(CJMP2),
  OC(CJMP3),
  OC(CJMP4),
  OC(CJMP5),
  OC(FINAL_MOVE_NORTH),
  OC(FINAL_MOVE_EAST),
  OC(FINAL_MOVE_SOUTH),
  OC(FINAL_MOVE_WEST),
  OC(FINAL_MOVE_RANDOM),
  OC(END)
};


}  // namespace evol
