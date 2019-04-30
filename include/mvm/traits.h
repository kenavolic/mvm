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

#include "mvm/meta.h"

#include <type_traits>
#include <variant>
#include <vector>

namespace mvm {
namespace details {
// Hide traits intermediate computation in details ns
template <typename Set> struct instr_set_traits_impl {
  using set_type = Set;
  using instr_set_desc_type = typename set_type::instr_table;

  // List of possible stack value types generated from instructions that
  // produces data to the value stack
  using set_stack_type = set_stack_types_t<instr_set_desc_type>;

  // Generic endianness
  using endian_type = typename set_type::endian_type;

  static constexpr std::size_t instr_set_size =
      list::size_v<instr_set_desc_type>;

  // Map used to make assembler impl easier
  static constexpr auto instr_names =
      names_aggregator<instr_set_desc_type,
                       std::make_index_sequence<instr_set_size>>::value;

  static_assert(instr_set_size <= 256,
                "[-][mvm] max instruction set size (256) exceeded");
};

} // namespace details

namespace traits {

///
/// @brief traits class for instruction set
///
template <typename Set> struct instr_set_traits {
  using set_type = Set;

  using instr_set_desc_type = typename set_type::instr_table;

  using set_stack_type =
      typename details::instr_set_traits_impl<Set>::set_stack_type;

  using endian_type = typename set_type::endian_type;

  static constexpr auto instr_names =
      details::instr_set_traits_impl<Set>::instr_names;

  template <typename T>
  static constexpr auto type_size = set_type::template code_value_repr<T>::size;

  template <typename T>
  using type_endianness =
      typename set_type::template code_value_repr<T>::endian_type;
};

///
/// @brief traits class for value_stack
///
template <typename TypeList, std::size_t = list::size_v<TypeList>>
struct value_stack_traits {

  using value_type = list::rebind_t<std::variant, TypeList>;
  using stack_type = std::vector<value_type>;

  template <typename T> static auto get_val(value_type &&val) {
    return std::get<T>(std::forward<value_type>(val));
  }
};

template <typename TypeList> struct value_stack_traits<TypeList, 1> {
  using value_type = list::front_t<TypeList>;
  using stack_type = std::vector<value_type>;

  template <typename T> static auto get_val(T val) { return val; }
};

///
/// @brief traits class for producer type
///
template <typename I> struct producers_traits {
  using producers_type = typename I::producers_type;
  static_assert(list::size_v<producers_type> == 1,
                "[-][mvm] only single producer handled now");

  using producer_type = list::front_t<producers_type>;
  using producer_datalist_type = typename producer_type::meta_data_type;

  using data_type = list::front_t<producer_datalist_type>;
};

template <typename Consumers> struct consumers_traits {
  using front_consumer_type = list::front_t<Consumers>;
  using front_consumer_data_type = typename front_consumer_type::meta_data_type;
  using consumers_minus_one = list::pop_front_t<Consumers>;
};

} // namespace traits
} // namespace mvm