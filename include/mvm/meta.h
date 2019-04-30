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

#include "mvm/helpers/list.h"
#include "mvm/helpers/typestring.h"
#include "mvm/helpers/utils.h"
#include "mvm/program.h"

#include <tuple>
#include <type_traits>

namespace mvm {

///////////////////////////////////////////////////////////////
// base meta model used to describe an instruction set
///////////////////////////////////////////////////////////////

// first mandatory meta concept is a stack
template <typename Instance> struct meta_value_stack {};

// second mandatory meta concept is bytecode to interpret
template <typename Instance> struct meta_bytecode {};

template <typename... Ts> using meta_type_list = list::mplist<Ts...>;

// bind a meta concept and a list of types to handle
template <template <typename> typename Meta, typename... Ts> struct meta_tie {
  template <typename T> using meta_type = Meta<T>;
  using meta_data_type = meta_type_list<Ts...>;
};

// bind a meta concept and an instance of the concept
template <typename Meta, typename Instance> struct instance {};

// count_from is a utility used to define
// instruction with dependant meta concept
template <typename T> struct count_from;

template <template <typename> typename Meta, typename T>
struct count_from<meta_tie<Meta, T>> {
  using meta_type = meta_tie<Meta, T>;
  using type = T;
};

// consumer type is just a binding between a meta concept
// and a list of types to handle
template <template <typename> typename Meta, typename... Ts>
using consumer = meta_tie<Meta, Ts...>;

// same as consumer
template <template <typename> typename Meta, typename... Ts>
using producer = meta_tie<Meta, Ts...>;

// iterable consumer is a specific consumer
template <template <typename> typename Meta, typename Iterable,
          typename Counter>
struct iterable_consumer : consumer<Meta, Iterable> {
  using counter_type = Counter;
};

// producer list
template <typename... Ps> struct producers {
  // TODO: check all are producers
};

// consumers list
template <typename... Cs> struct consumers {
  // TODO: check all are consumers
};

// shortcuts for empty producers and consumers
using no_prod = producers<>;
using no_cons = consumers<>;

///////////////////////////////////////////////////////////////
// type helpers implementation details
///////////////////////////////////////////////////////////////

namespace details {

template <typename T> struct flatten_tie_types;

template <template <typename...> typename T, typename... MetaTie>
struct flatten_tie_types<T<MetaTie...>> {
  using type = list::concat_all_t<typename MetaTie::meta_data_type...>;
};

template <template <typename...> typename T> struct flatten_tie_types<T<>> {
  using type = meta_type_list<>;
};

template <typename T>
using flatten_tie_types_t = typename flatten_tie_types<T>::type;

template <typename C, typename Ret, typename ArgList> struct prototype;

template <typename C, typename Ret, typename... Args>
struct prototype<C, Ret, list::mplist<Args...>> {
  using type = Ret (C::*)(Args...);
};

template <typename C, typename RetList, typename ArgList,
          size_t = list::size_v<RetList>>
struct prototype_builder
    : prototype<C, list::rebind_t<std::tuple, RetList>, ArgList> {};

template <typename C, typename RetList, typename ArgList>
struct prototype_builder<C, RetList, ArgList, 1>
    : prototype<C, list::front_t<RetList>, ArgList> {};

template <typename C, typename RetList, typename ArgList>
struct prototype_builder<C, RetList, ArgList, 0> : prototype<C, void, ArgList> {
};

template <typename Set, typename Producer, typename Consumer, bool HasIp>
struct desc_to_proto;

template <typename Set, typename Producer, typename Consumer>
struct desc_to_proto<Set, Producer, Consumer, false>
    : prototype_builder<Set, flatten_tie_types_t<Producer>,
                        flatten_tie_types_t<Consumer>> {};

template <typename Set, typename Producer, typename Consumer>
struct desc_to_proto<Set, Producer, Consumer, true>
    : prototype_builder<
          Set, flatten_tie_types_t<Producer>,
          list::push_front_t<ip &, flatten_tie_types_t<Consumer>>> {};

template <typename T> struct unwrap_type { using type = std::decay_t<T>; };

template <template <typename...> typename Container, typename T, typename... Ts>
struct unwrap_type<Container<T, Ts...>> {
  using type = std::decay_t<T>;
};

template <typename T> using unwrap_type_t = typename unwrap_type<T>::type;

template <typename Consumer> struct bytecode_types {
  using type = list::mplist<>;
};

template <typename... Ts>
struct bytecode_types<consumer<meta_bytecode, Ts...>> {
  using type = list::mplist<Ts...>;
};

template <template <typename> typename Meta, typename Iterable,
          template <typename> typename CounterMeta, typename CounterT>
struct bytecode_types<iterable_consumer<
    Meta, Iterable, count_from<meta_tie<CounterMeta, CounterT>>>> {
  using type = list::mplist<CounterT>;
};

template <typename Producer> struct value_stack_types {
  using type = list::mplist<>;
};

template <typename... Ts>
struct value_stack_types<producer<meta_value_stack, Ts...>> {
  using type = list::mplist<unwrap_type_t<Ts>...>;
};

template <template <typename...> typename Finder, typename List,
          bool = list::is_empty_v<List>>
struct types_aggregator;

template <template <typename...> typename Finder, typename List>
struct types_aggregator<Finder, List, false> {
  using partial_types = typename Finder<list::front_t<List>>::type;
  using type = list::merge_t<
      partial_types,
      typename types_aggregator<Finder, list::pop_front_t<List>>::type>;
};

template <template <typename...> typename Finder, typename List>
struct types_aggregator<Finder, List, true> {
  using type = list::mplist<>;
};

template <typename List, bool = list::is_empty_v<List>>
struct set_stack_types_aggregator;

template <typename List> struct set_stack_types_aggregator<List, false> {
  using type = list::merge_t<
      typename list::front_t<List>::value_stack_type,
      typename set_stack_types_aggregator<list::pop_front_t<List>>::type>;
};

template <typename List> struct set_stack_types_aggregator<List, true> {
  using type = list::mplist<>;
};

template <template <typename...> typename Meta, typename I>
struct is_instance_of : std::false_type {
  using type = nonsuch;
};

template <template <typename...> typename Meta, typename T>
struct is_instance_of<Meta, Meta<T>> : std::true_type {
  using type = T;
};

template <typename InstanceList, template <typename> typename Meta,
          bool = list::is_empty_v<InstanceList>>
struct instance_of_impl;

template <typename InstanceList, template <typename> typename Meta>
struct instance_of_impl<InstanceList, Meta, false> {
  using head_type = list::front_t<InstanceList>;
  using test_type = is_instance_of<Meta, head_type>;
  using type = std::conditional_t<
      test_type::value, typename test_type::type,
      typename instance_of_impl<list::pop_front_t<InstanceList>, Meta>::type>;
};

template <typename InstanceList, template <typename> typename Meta>
struct instance_of_impl<InstanceList, Meta, true> {
  // termination, no match found
  using type = nonsuch;
};

template <typename MetaTie, typename I>
struct is_instance_of_tie : std::false_type {
  using type = nonsuch;
};

template <template <typename> typename Meta, typename T, typename... Args>
struct is_instance_of_tie<meta_tie<Meta, Args...>, Meta<T>> : std::true_type {
  using type = T;
};

template <template <typename> typename Meta, typename Iterable,
          typename Counter, typename T>
struct is_instance_of_tie<iterable_consumer<Meta, Iterable, Counter>, Meta<T>>
    : std::true_type {
  using type = T;
};

template <typename InstanceList, typename MetaTie,
          bool = list::is_empty_v<InstanceList>>
struct instance_of_tie_impl;

template <typename InstanceList, typename MetaTie>
struct instance_of_tie_impl<InstanceList, MetaTie, false> {
  using head_type = list::front_t<InstanceList>;
  using test_type = is_instance_of_tie<MetaTie, head_type>;
  using type =
      std::conditional_t<test_type::value, typename test_type::type,
                         typename instance_of_tie_impl<
                             list::pop_front_t<InstanceList>, MetaTie>::type>;
};

template <typename InstanceList, typename MetaTie>
struct instance_of_tie_impl<InstanceList, MetaTie, true> {
  using type = nonsuch;
};
} // namespace details

////////////////////////////////
// type helpers public interface
/////////////////////////////////

// retrieve a type list of all stack types produced by a list of producers
// @note this retrieves only the type of the meta_value_stack meta concept
template <typename Producers>
using value_stack_types_t =
    typename details::types_aggregator<details::value_stack_types,
                                       Producers>::type;

// retrieve a type list of all stack types produced by an instruction set
// @note this retrieves only the type of the meta_value_stack meta concept
template <typename ITable>
using set_stack_types_t =
    typename details::set_stack_types_aggregator<ITable>::type;

// retrieve a type list of all types consumes in the bytecode for an instruction
// set
template <typename Consumers>
using bytecode_types_t =
    typename details::types_aggregator<details::bytecode_types,
                                       Consumers>::type;

// aggregates all instruction names of an instruction set
template <typename ISet, typename I> struct names_aggregator;

template <typename ISet, std::size_t... Is>
struct names_aggregator<ISet, std::index_sequence<Is...>> {
  static constexpr std::array<char const *const, sizeof...(Is)> value = {
      typestring::char_seq<typename list::at_t<Is, ISet>::name_type>::value...};
};

// check a meta concept is instantiated in an instance list
template <typename InstanceList, template <typename> typename Meta>
struct instance_of : details::instance_of_impl<InstanceList, Meta> {};

template <typename InstanceList, template <typename> typename Meta>
using instance_of_t = typename instance_of<InstanceList, Meta>::type;

// check a meta tie is instantiated in an instance list
template <typename InstanceList, typename MetaTie>
struct instance_of_tie : details::instance_of_tie_impl<InstanceList, MetaTie> {
  static_assert(
      !std::is_same_v<
          typename details::instance_of_tie_impl<InstanceList, MetaTie>::type,
          nonsuch>,
      "[-][mvm] instance not found");
};

template <typename InstanceList, typename MetaTie>
using instance_of_tie_t = typename instance_of_tie<InstanceList, MetaTie>::type;

// filter an instance list
template <typename InstanceList, typename T> struct filter_instance {
  using list_type = InstanceList;

  template <typename U> using pred_type = std::is_same<U, T>;

  using type = list::filter_t<pred_type, false, list_type>;
};

template <typename InstanceList, typename T>
using filter_instance_t = typename filter_instance<InstanceList, T>::type;

// transform a instance list type to an instantiable object
template <typename InstanceList> struct make_instances;

template <template <typename> typename... Meta, typename... Args>
struct make_instances<list::mplist<Meta<Args>...>> {
  using type = std::tuple<Args...>;
};

template <typename InstanceList>
using make_instances_t = typename make_instances<InstanceList>::type;
} // namespace mvm