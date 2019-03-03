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

#include "mvm/base_stack.h"
#include "mvm/except.h"
#include "mvm/helpers/list.h"
#include "mvm/helpers/num_parse.h"
#include "mvm/helpers/reflect.h"
#include "mvm/instr_set.h"
#include "mvm/instr_set_traits.h"
#include "mvm/macros.h"
#include "mvm/program_chunk.h"

#define MVM_INTERPRET_I(n)                                                     \
  case n: {                                                                    \
    using instr_type =                                                         \
        list::at_t<n, typename instr_set_traits_type::ext_instr_set_type>;     \
    this->apply<instr_type>();                                                 \
  } break;

namespace mvm {
///
/// @brief Basic interpreter
///
/// The implementation is not focused on performance but
/// on educational purpose.
///
template <typename Set> class base_interpreter {
  using instr_set_type = Set;
  using instr_set_traits_type =
      typename traits::instr_set_traits<instr_set_type>;
  using stack_type = vstack<typename instr_set_traits_type::stack_list_type>;

  instr_set_type &m_iset;
  prog_chunk m_chunk;
  stack_type m_vstack;
  uint8_t *m_ip;

public:
  base_interpreter(instr_set_type &iset) : m_iset{iset} {}

  ///
  /// @brief Interpret code chunk
  ///
  void interpret(prog_chunk &&c) {
    m_chunk = std::move(c);
    m_ip = &(m_chunk.code[0]);

    this->run();
  }

private:
  void run() {
    while (m_ip <= &(m_chunk.code[m_chunk.code.size() - 1])) {
      switch (this->read_next()) {
        MVM_UNROLL_256(MVM_INTERPRET_I)
      default:
        throw mexcept("[-][mvm] instruction opcode overflow",
                      status_type::INSTR_OPCODE_OVERFLOW);
      }
    }
  }

  uint8_t read_next() { return *m_ip++; }

  template <typename T, std::size_t N, typename Endian> auto parse() {
    auto ip = m_ip;
    m_ip += N;

    return num::parse<T, N, Endian>(ip);
  }

  template <typename I, std::size_t... Is>
  auto parse_code_values(std::index_sequence<Is...>) {
    using c_type = typename I::code_consumer_type;
    return c_type{this->parse<
        list::at_t<Is, c_type>,
        instr_set_type::template code_value_repr<list::at_t<Is, c_type>>::size,
        typename instr_set_traits_type::endian_type>()...};
  }

  template <typename I, std::size_t... Is>
  auto pop_stack_values(std::index_sequence<Is...>) {
    using c_type = typename I::stack_consumer_type;
    return c_type{m_vstack.template pop<list::at_t<Is, c_type>>()...};
  }

  template <typename I> auto apply_stack_consume() {
    using c_type = typename I::stack_consumer_type;
    return I::apply(
        m_iset,
        pop_stack_values<I>(std::make_index_sequence<list::size_v<c_type>>()));
  }

  template <typename I> auto apply_code_consume() {
    using c_type = typename I::code_consumer_type;
    return I::apply(
        m_iset,
        parse_code_values<I>(std::make_index_sequence<list::size_v<c_type>>()));
  }

  template <typename I> auto apply_stack_and_code_consume() {
    using sc_type = typename I::stack_consumer_type;
    using cc_type = typename I::code_consumer_type;

    return I::apply(
        m_iset,
        pop_stack_values<I>(std::make_index_sequence<list::size_v<sc_type>>()),
        parse_code_values<I>(
            std::make_index_sequence<list::size_v<cc_type>>()));
  }

  template <typename I> auto apply_consume() {
    if constexpr (concept ::has_stack_consumer_type(reflect::type<I>) &&
                  concept ::has_code_consumer_type(reflect::type<I>)) {
      return this->apply_stack_and_code_consume<I>();
    } else if constexpr (concept ::has_stack_consumer_type(reflect::type<I>)) {
      return this->apply_stack_consume<I>();
    } else if constexpr (concept ::has_code_consumer_type(reflect::type<I>)) {
      return this->apply_code_consume<I>();
    } else {
      return I::apply(m_iset);
    }
  }

  template <typename I, typename T, std::size_t... Is>
  void apply_produce(T const &values, std::index_sequence<Is...>) {
    (m_vstack.template push(stack_at<Is>(values)), ...);
  }

  template <typename I> void apply() {
    std::optional<ip_data> opt_ip_data;

    if constexpr (!std::is_same_v<I, detail::unknown_i>) {
      auto res = this->apply_consume<I>();

      if constexpr (concept ::has_stack_producer_type(reflect::type<I>)) {
        this->apply_produce<I>(
            std::get<0>(res),
            std::make_index_sequence<
                list::size_v<typename I::stack_producer_type>>());

        opt_ip_data = std::get<1>(res);
      } else {
        opt_ip_data = res;
      }
    } else {
      throw mexcept("[-][mvm] invalid instruction",
                    status_type::INVALID_INSTR_OPCODE);
    }

    // handle ip
    if (opt_ip_data) {
      m_ip += opt_ip_data.value();
    }
  }
};
} // namespace mvm