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

#include "mvm/assembler.h"
#include "mvm/bytecode_serializer.h"
#include "mvm/disassembler.h"
#include "mvm/except.h"
#include "mvm/instr_set.h"
#include "mvm/interpreter.h"
#include "mvm/status.h"
#include "mvm/value_stack.h"

namespace mvm {

///
/// @brief Basic back-end
///
template <typename Set,
          typename InstanceList = list::mplist<
              meta_bytecode<bytecode_serializer>,
              meta_value_stack<value_stack<
                  typename traits::instr_set_traits<Set>::set_stack_type>>>>
class vm {
  using instr_set_type = Set;
  using bytecode_serializer_type = instance_of_t<InstanceList, meta_bytecode>;
  using interpreter_type = interpreter<Set, InstanceList>;
  using assembler_type = assembler<Set, bytecode_serializer_type>;
  using disassembler_type = disassembler<Set, bytecode_serializer_type>;

  interpreter_type m_interpreter;
  assembler_type m_assembler;
  disassembler_type m_disassembler;

public:
  vm(instr_set_type &iset) : m_interpreter{iset} {}

  ///
  /// @brief Interpret code chunk
  ///
  auto interpret(prog_chunk const &c) {
    return translate([&]() { m_interpreter.interpret(c); });
  }

  ///
  /// @brief Assemble code chunk
  ///
  auto assemble(std::istream &stream) const {
    return translate([&]() { return m_assembler.assemble(stream); });
  }

  ///
  /// @brief Disassemble code chunk
  ///
  auto disassemble(prog_chunk const &c) {
    return translate([&]() { return m_disassembler.disassemble(c); });
  }
};
} // namespace mvm