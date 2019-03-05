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

#include <string>

#include "mvm/except.h"
#include "mvm/program_chunk.h"

namespace mvm {
///
/// @brief Basic disassembler
///
/// The implementation is not focused on performance but
/// on educational purpose.
///
template <typename Set> class base_disassembler {
  using instr_set_type = Set;
  using instr_set_traits_type =
      typename traits::instr_set_traits<instr_set_type>;
  using instr_set_desc_type =
      typename instr_set_traits_type::instr_set_desc_type;

  prog_chunk m_chunk;
  uint8_t *m_code_ptr;

public:
  ///
  /// @brief Disassemble code chunk
  ///
  std::string disassemble(prog_chunk &&c);

private:
  // internal disassemble method
  std::string disassemble();
  // assert op is in chunk
  bool assert_in_chunk();

  // parse and update ip
  template <typename T, std::size_t N, typename Endian>
  void parse(std::string &instr);

  // parse operand in bytecode
  template <typename I, std::size_t Index>
  void parse_operand(std::string &instr);

  // dissassemble instr operands
  template <typename I, std::size_t... Is>
  void disassemble_operands(std::string &instr, std::index_sequence<Is...>);

  // disassemble instr
  template <typename I> std::string disassemble_instr();
};
// impl
template <typename Set>
std::string base_disassembler<Set>::disassemble(prog_chunk &&c) {
  m_chunk = std::move(c);
  m_code_ptr = &(m_chunk.code[0]);

  return this->disassemble();
}

template <typename Set> std::string base_disassembler<Set>::disassemble() {
  std::string prog;
  while (assert_in_chunk()) {
    instr_set_visitor<instr_set_desc_type>()(
        *m_code_ptr, [this, &prog](auto &&arg) {
          using instr_type = std::decay_t<decltype(arg)>;
          prog.append(this->disassemble_instr<instr_type>() + "\n");
        });
    ++m_code_ptr;
  }

  return prog;
}

template <typename Set> bool base_disassembler<Set>::assert_in_chunk() {
  return (m_code_ptr <= &(m_chunk.code[m_chunk.code.size() - 1]));
}

template <typename Set>
template <typename T, std::size_t N, typename Endian>
void base_disassembler<Set>::parse(std::string &instr) {
  auto code_ptr = m_code_ptr + 1;
  m_code_ptr += N;

  if (!assert_in_chunk()) {
    throw mexcept("[-][mvm] bytecode overflow", status_type::CODE_OVERFLOW);
  }

  instr.append(std::to_string(num::parse<T, N, Endian>(code_ptr)));
}

template <typename Set>
template <typename I, std::size_t Index>
void base_disassembler<Set>::parse_operand(std::string &instr) {
  using c_type = typename I::code_consumer_type;
  this->parse<
      list::at_t<Index, c_type>,
      instr_set_type::template code_value_repr<list::at_t<Index, c_type>>::size,
      typename instr_set_traits_type::endian_type>(instr);

  if (Index < (list::size_v<c_type> - 1)) {
    instr.push_back(' ');
  }
}

template <typename Set>
template <typename I, std::size_t... Is>
void base_disassembler<Set>::disassemble_operands(std::string &instr,
                                                  std::index_sequence<Is...>) {
  (parse_operand<I, Is>(instr), ...);
}

template <typename Set>
template <typename I>
std::string base_disassembler<Set>::disassemble_instr() {
  std::string instr{I::name};

  if constexpr (concept ::has_code_consumer_type(reflect::type<I>)) {
    using cc_type = typename I::code_consumer_type;
    instr.push_back(' ');
    this->disassemble_operands<I>(
        instr, std::make_index_sequence<list::size_v<cc_type>>());
  }

  return instr;
}
} // namespace mvm