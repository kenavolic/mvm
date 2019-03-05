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

#include <set>
#include <type_traits>

#include "mvm/concept.h"
#include "mvm/helpers/list.h"

namespace mvm {
namespace detail {
// unknown instruction is a dummy instr used for
// handling instr from set size to 256
struct unknown_i 
{
  static constexpr char const* const name = "???";
};

template <typename T> struct stack_elements;

template <template <typename...> typename T, typename... Args>
struct stack_elements<T<Args...>> {
  using type = list::mplist<Args...>;
};

template <typename T, bool is_producer =
                          concept ::has_stack_producer_type(reflect::type<T>)>
struct stack_producer_types;

template <typename T> struct stack_producer_types<T, true> {
  using type = typename stack_elements<typename T::stack_producer_type>::type;
};

template <typename T> struct stack_producer_types<T, false> {
  using type = list::mplist<>;
};

template <typename T, bool is_consumer =
                          concept ::has_stack_consumer_type(reflect::type<T>)>
struct stack_consumer_types;

template <typename T> struct stack_consumer_types<T, true> {
  using type = typename stack_elements<typename T::stack_consumer_type>::type;
};

template <typename T> struct stack_consumer_types<T, false> {
  using type = list::mplist<>;
};

template <typename List, bool = list::is_empty_v<List>>
struct stack_types_finder;

template <typename List> struct stack_types_finder<List, false> {
  using partial_types =
      typename stack_producer_types<list::front_t<List>>::type;
  using type =
      list::merge_t<partial_types,
                    typename stack_types_finder<list::pop_front_t<List>>::type>;
};

template <typename List> struct stack_types_finder<List, true> {
  using type = list::mplist<>;
};

template <typename T, typename StackTypes> struct check_consumer {
  // check each consumer type is in the list of authorized stack types
  template <typename U> using pred_type = list::has<U, StackTypes>;

  static constexpr bool value =
      list::check_v<pred_type, typename stack_consumer_types<T>::type>;
};

template <typename ISet, typename StackTypes> struct stack_types_checker {
  template <typename T> using pred_type = check_consumer<T, StackTypes>;

  // browse all rows to check that consumer types are part of stack types
  static constexpr bool value = list::check_v<pred_type, ISet>;
};

template <typename ISet, typename I> struct names_finder;

template <typename ISet, std::size_t... Is>
struct names_finder<ISet, std::index_sequence<Is...>> {
  static constexpr std::array<char const *const, sizeof...(Is)> value = {
      list::at_t<Is, ISet>::name...};
};

template <typename Set> struct instr_set_traits_impl {
  using set_type = Set;

  // Defined instr set
  using instr_set_type = typename set_type::instr_table;

  // Extended instr set to fill set up to 256 instr
  using ext_instr_set_type = list::fill_t<256, unknown_i, instr_set_type>;

  // List of possible stack value types generated from instructions that
  // produces data from the stack
  using stack_list_type = typename stack_types_finder<instr_set_type>::type;

  // Endianness of byte code value
  using endian_type = typename set_type::endian_type;

  static constexpr std::size_t instr_set_size = list::size_v<instr_set_type>;

  // Map used to make assembler impl easier
  static constexpr auto instr_names =
      names_finder<instr_set_type,
                   std::make_index_sequence<instr_set_size>>::value;

  // Check no instruction consumes types from the stack not in the stack list
  static_assert(stack_types_checker<instr_set_type, stack_list_type>::value,
                "[-][mvm] incompatible stack consumer types found");
  static_assert(instr_set_size <= 256,
                "[-][mvm] max instruction set size (256) exceeded");
};

} // namespace detail

namespace traits {
template <typename Set> struct instr_set_traits {
  using set_type = Set;

  using instr_set_type = typename set_type::instr_table;

  using ext_instr_set_type =
      typename detail::instr_set_traits_impl<Set>::ext_instr_set_type;

  using stack_list_type =
      typename detail::instr_set_traits_impl<Set>::stack_list_type;

  using endian_type = typename set_type::endian_type;

  static constexpr auto instr_names =
      detail::instr_set_traits_impl<Set>::instr_names;
};
} // namespace traits
} // namespace mvm