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
#include <iostream>
#include <type_traits>

#include "mvm/except.h"
#include "mvm/helpers/list.h"
#include "mvm/helpers/num_parse.h"
#include "mvm/helpers/reflect.h"
#include "mvm/instr_set.h"
#include "mvm/instr_set_traits.h"
#include "mvm/macros.h"
#include "mvm/program_chunk.h"

#define MVM_ASSEMBLE_I(n)                                                      \
  case n: {                                                                    \
    using instr_type =                                                         \
        list::at_t<n, typename instr_set_traits_type::ext_instr_set_type>;     \
    return assemble_instr<instr_type>(                                         \
        static_cast<uint8_t>(n), (tokens.erase(std::begin(tokens)), tokens));  \
  } break;

namespace mvm {
///
/// @brief Basic assembler
///
/// The implementation is not focused on performance but
/// on educational purpose.
///
template <typename Set> class base_assembler {
  using instr_set_type = Set;
  using instr_set_traits_type =
      typename traits::instr_set_traits<instr_set_type>;

public:
  ///
  /// @brief Assemble code chunk
  ///
  static prog_chunk assemble(std::istream &stream) {
    prog_chunk c;
    std::string line;
    while (std::getline(stream, line)) {
      auto line_assembly = assemble(line);
      c.code.insert(std::end(c.code), std::cbegin(line_assembly),
                    std::cend(line_assembly));
    }

    return c;
  }

private:
  static std::vector<uint8_t> assemble(std::string const &line) {
    std::istringstream iss(line);
    std::vector<std::string> tokens(std::istream_iterator<std::string>{iss},
                                    std::istream_iterator<std::string>());

    if (tokens.empty()) // need at leat an instruction name
    {
      throw mexcept("[-][mvm] no instruction token",
                    status_type::NO_INSTR_NAME);
    }

    auto iname_it = std::find_if(
        std::cbegin(instr_set_traits_type::instr_names),
        std::cend(instr_set_traits_type::instr_names),
        [&](auto const &val) { return std::string(val) == tokens[0]; });

    if (iname_it == std::cend(instr_set_traits_type::instr_names)) {
      throw mexcept("[-][mvm] invalid instruction name",
                    status_type::BAD_INSTR_NAME);
    }

    switch (std::distance(std::cbegin(instr_set_traits_type::instr_names),
                          iname_it)) {
      MVM_UNROLL_256(MVM_ASSEMBLE_I)
    default:
      throw mexcept("[-][mvm] instruction opcode overflow",
                    status_type::INSTR_OPCODE_OVERFLOW);
    }
  }

  template <typename I, std::size_t Index>
  static void serial_attribute(std::vector<uint8_t> &bytes,
                               std::string const &token) {
    using cc_type = typename I::code_consumer_type;
    auto ser = num::serial<list::at_t<Index, cc_type>,
                           instr_set_type::template code_value_repr<
                               list::at_t<Index, cc_type>>::size,
                           typename instr_set_traits_type::endian_type>(token);
    std::copy(std::cbegin(ser), std::cend(ser), std::back_inserter(bytes));
  }

  template <typename I, std::size_t... Is>
  static void assemble_attributes(std::vector<uint8_t> &bytes,
                                  std::vector<std::string> const &tokens,
                                  std::index_sequence<Is...>) {
    (serial_attribute<I, Is>(bytes, tokens[Is]), ...);
  }

  template <typename I>
  static std::vector<uint8_t>
  assemble_instr(uint8_t i, std::vector<std::string> const &attributes) {
    std::vector<uint8_t> bytes = {i};

    if constexpr (concept ::has_code_consumer_type(reflect::type<I>)) {
      using cc_type = typename I::code_consumer_type;

      if (attributes.size() != list::size_v<cc_type>) {
        throw mexcept("[-][mvm] invalid instruction operands",
                      status_type::BAD_INSTR_OPERAND);
      }

      assemble_attributes<I>(bytes, attributes,
                             std::make_index_sequence<list::size_v<cc_type>>());
    } else {
      if (attributes.size() != 0) {
        throw mexcept("[-][mvm] invalid instruction operands",
                      status_type::BAD_INSTR_OPERAND);
      }
    }

    return bytes;
  }
};
} // namespace mvm