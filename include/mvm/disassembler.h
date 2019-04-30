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

#include "mvm/concept.h"
#include "mvm/except.h"
#include "mvm/helpers/typestring.h"
#include "mvm/program.h"
#include "mvm/trace.h"

#include <string>
#include <type_traits>

namespace mvm {
///
/// @brief Basic disassembler
///
template <typename Set, typename MetaCodeImpl> class disassembler {
  using instr_set_type = Set;
  using instr_set_traits_type =
      typename traits::instr_set_traits<instr_set_type>;
  using instr_set_desc_type =
      typename instr_set_traits_type::instr_set_desc_type;
  using code_impl_type = MetaCodeImpl;

  rebasable_ip m_rebased_ip;

public:
  ///
  /// @brief Disassemble code chunk
  ///
  std::string disassemble(prog_chunk const &c);

private:
  // internal disassemble method
  std::string disassemble();

  // disassemble instr
  template <typename I> std::string disassemble_instr();

  // dissassemble instr operands
  template <typename I, std::size_t... Is>
  void disassemble_operands(std::string &instr, std::index_sequence<Is...>);

  // parse operand in bytecode
  template <typename I, std::size_t Index>
  void parse_operand(std::string &instr);

  // parse and update ip
  template <typename T, std::size_t N, typename Endian>
  void parse(std::string &instr);
};

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

template <typename Set, typename MetaCodeImpl>
std::string
disassembler<Set, MetaCodeImpl>::disassemble(prog_chunk const &chunk) {
  m_rebased_ip.rebase(&(chunk.code[0]),
                      &(chunk.code[0]) + chunk.code.size() - 1);
  return this->disassemble();
}

template <typename Set, typename MetaCodeImpl>
std::string disassembler<Set, MetaCodeImpl>::disassemble() {
  std::string prog;
  while (m_rebased_ip.assert_in_chunk()) {
    LOG_INFO("disassembler -> process instruction opcode "
             << static_cast<int>(*m_rebased_ip));
    instr_set_visitor<instr_set_desc_type>()(
        *m_rebased_ip, [this, &prog](auto &&arg) {
          using instr_type = std::decay_t<decltype(arg)>;
          prog.append(this->disassemble_instr<instr_type>() + "\n");
        });
    ++m_rebased_ip;
  }

  return prog;
}

template <typename Set, typename MetaCodeImpl>
template <typename I>
std::string disassembler<Set, MetaCodeImpl>::disassemble_instr() {
  std::string instr{typestring::char_seq<typename I::name_type>::value};

  LOG_INFO("disassembler -> disassemble instruction " << instr);

  if constexpr (concept ::is_code_consumer_v<I>) {
    using cc_type = typename I::bytecode_type;
    instr.push_back(' ');
    this->disassemble_operands<I>(
        instr, std::make_index_sequence<list::size_v<cc_type>>());
  }

  return instr;
}

template <typename Set, typename MetaCodeImpl>
template <typename I, std::size_t... Is>
void disassembler<Set, MetaCodeImpl>::disassemble_operands(
    std::string &instr, std::index_sequence<Is...>) {
  (parse_operand<I, Is>(instr), ...);
}

template <typename Set, typename MetaCodeImpl>
template <typename I, std::size_t Index>
void disassembler<Set, MetaCodeImpl>::parse_operand(std::string &instr) {
  using cc_type = typename I::bytecode_type;
  this->parse<list::at_t<Index, cc_type>,
              instr_set_type::template code_value_repr<
                  list::at_t<Index, cc_type>>::size,
              typename instr_set_type::template code_value_repr<
                  list::at_t<Index, cc_type>>::endian_type>(instr);

  if (Index < (list::size_v<cc_type> - 1)) {
    instr.push_back(' ');
  }
}

template <typename Set, typename MetaCodeImpl>
template <typename T, std::size_t N, typename Endian>
void disassembler<Set, MetaCodeImpl>::parse(std::string &instr) {
  uint8_t *ip = m_rebased_ip;
  m_rebased_ip += N;

  if (!m_rebased_ip.assert_in_chunk()) {
    throw mexcept("[-][mvm] bytecode overflow", status_type::CODE_OVERFLOW);
  }

  instr.append(
      std::to_string(code_impl_type{}.template parse<T, N, Endian>(ip + 1)));
}

} // namespace mvm