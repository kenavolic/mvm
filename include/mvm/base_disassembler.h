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

#include "mvm/program_chunk.h"
#include "mvm/except.h"

#define MVM_DISASSEMBLE_I(n)                                                      \
  case n: {                                                                    \
    using instr_type =                                                         \
        list::at_t<n, typename instr_set_traits_type::ext_instr_set_type>;     \
    prog.append(disassemble_instr<instr_type>() + "\n");                                         \
  } break;

namespace mvm
{
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

prog_chunk m_chunk;
uint8_t *m_code_ptr;

public:
  ///
  /// @brief Disassemble code chunk
  ///
  std::string disassemble(prog_chunk&& c)
  {
    m_chunk = std::move(c);
    m_code_ptr = &(m_chunk.code[0]);

    return this->disassemble();
  }

private:

std::string disassemble()
{
    std::string prog;
while (assert_in_chunk()) {
      switch (*m_code_ptr) {
        MVM_UNROLL_256(MVM_DISASSEMBLE_I)
      default:
        throw mexcept("[-][mvm] instruction opcode overflow",
                      status_type::INSTR_OPCODE_OVERFLOW);
      }
      ++m_code_ptr;
    }

    return prog;
}

bool assert_in_chunk()
{
    return (m_code_ptr <= &(m_chunk.code[m_chunk.code.size() - 1]));
}

template <typename T, std::size_t N, typename Endian> void parse(std::string& instr) {
    auto code_ptr = m_code_ptr + 1;
    m_code_ptr += N;

    if (!assert_in_chunk())
    {
        throw mexcept("[-][mvm] bytecode overflow",
                    status_type::CODE_OVERFLOW);
    }

    instr.append(std::to_string(num::parse<T, N, Endian>(code_ptr)));
  }

  template <typename I, std::size_t Index>
  void parse_code_value(std::string& instr) {
    using c_type = typename I::code_consumer_type;
    this->parse<
        list::at_t<Index, c_type>,
        instr_set_type::template code_value_repr<list::at_t<Index, c_type>>::size,
        typename instr_set_traits_type::endian_type>(instr);

    if (Index < (list::size_v<c_type> - 1))
    {
        instr.push_back(' ');
    }
  }

  template <typename I, std::size_t... Is>
  void disassemble_attributes(std::string& instr, std::index_sequence<Is...>) {
    (parse_code_value<I, Is>(instr), ...);
  }

  template <typename I>
  std::string
  disassemble_instr() {
    std::string instr{I::name};

     if (std::is_same_v<I, detail::unknown_i>) {
            throw mexcept("[-][mvm] invalid instruction",
                    status_type::INVALID_INSTR_OPCODE);
    }

    if constexpr (concept ::has_code_consumer_type(reflect::type<I>)) {
      using cc_type = typename I::code_consumer_type;
      instr.push_back(' ');
      this->disassemble_attributes<I>(instr, std::make_index_sequence<list::size_v<cc_type>>());
    }

    return instr;
  }

    };
}