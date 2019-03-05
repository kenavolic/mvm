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

#include "mvm/base_assembler.h"
#include "mvm/base_disassembler.h"
#include "mvm/base_interpreter.h"
#include "mvm/except.h"
#include "mvm/status.h"

namespace mvm {
///
/// @brief Basic back-end
///
/// The implementation is not focused on performance but
/// on educational purpose.
///
template <typename Set> class base_vm {
  using instr_set_type = Set;
  using interpreter_type = base_interpreter<Set>;
  using assembler_type = base_assembler<Set>;
  using disassembler_type = base_disassembler<Set>;

  interpreter_type m_interpreter;
  disassembler_type m_disassembler;

public:
  base_vm(instr_set_type &iset) : m_interpreter{iset} {}

  ///
  /// @brief Interpret code chunk
  ///
  auto interpret(prog_chunk &&c) {
    return translate([&]() { m_interpreter.interpret(std::move(c)); });
  }

  ///
  /// @brief Assemble code chunk
  ///
  auto assemble(std::istream &stream) const {
    return translate([&]() { return assembler_type::assemble(stream); });
  }

  ///
  /// @brief Disassemble code chunk
  ///
  auto disassemble(prog_chunk &&c) {
    return translate([&]() { return m_disassembler.disassemble(std::move(c)); });
  }
};
} // namespace mvm