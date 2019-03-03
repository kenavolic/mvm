// Copyright 2019 Ken Avolic <kenavolic@none.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include <cstdint>

namespace mvm {
enum class status_type : uint16_t {
  SUCCESS = 0,
  NO_INSTR_NAME,
  BAD_INSTR_NAME,
  INSTR_OPCODE_OVERFLOW,
  BAD_INSTR_OPERAND,
  INVALID_INSTR_OPCODE,
  INTERNAL_ERROR,
  UNKNOWN_ERROR
};
}