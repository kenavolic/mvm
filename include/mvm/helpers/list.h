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

#include <type_traits>

#include "utils.h"

namespace mvm::list {
// generic type list algorithms

template <typename List> struct is_empty;

template <template <typename...> typename List, typename... Items>
struct is_empty<List<Items...>> : std::false_type {};

template <template <typename...> typename List>
struct is_empty<List<>> : std::true_type {};

template <typename List> using is_empty_t = typename is_empty<List>::type;

template <typename List> constexpr bool is_empty_v = is_empty_t<List>::value;

template <typename List> struct size;

template <template <typename...> typename List, typename... Items>
struct size<List<Items...>> {
  static constexpr std::size_t value = sizeof...(Items);
};

template <template <typename...> typename List> struct size<List<>> {
  static constexpr std::size_t value = 0;
};

template <typename List> constexpr auto size_v = size<List>::value;

template <typename List> struct front;

template <template <typename...> typename List, typename Head, typename... Tail>
struct front<List<Head, Tail...>> {
  using type = Head;
};

template <typename List> using front_t = typename front<List>::type;

template <typename List> struct robust_front { using type = front_t<List>; };

template <template <typename...> typename List> struct robust_front<List<>> {
  using type = nonsuch;
};

template <typename List>
using robust_front_t = typename robust_front<List>::type;

template <typename List> struct pop_front;

template <template <typename...> typename List, typename Head, typename... Tail>
struct pop_front<List<Head, Tail...>> {
  using type = List<Tail...>;
};

template <template <typename...> typename List> struct pop_front<List<>> {
  using type = List<>;
};

template <typename List> using pop_front_t = typename pop_front<List>::type;

template <typename Item, typename List> struct push_front;

template <typename Item, template <typename...> typename List,
          typename... Items>
struct push_front<Item, List<Items...>> {
  using type = List<Item, Items...>;
};

template <typename Item, typename List>
using push_front_t = typename push_front<Item, List>::type;

template <typename List> struct pop_back;

template <template <typename...> typename List, typename Head, typename... Tail>
struct pop_back<List<Head, Tail...>> {
  using type = push_front_t<Head, typename pop_back<List<Tail...>>::type>;
};

template <template <typename...> typename List, typename Head>
struct pop_back<List<Head>> {
  using type = List<>;
};

template <typename List> using pop_back_t = typename pop_back<List>::type;

namespace details {
template <typename Item, typename List, bool = is_empty_v<List>>
struct push_back_impl;

template <typename Item, typename List>
struct push_back_impl<Item, List, false> {
  using type =
      push_front_t<front_t<List>,
                   typename push_back_impl<Item, pop_front_t<List>>::type>;
};

template <typename Item, typename List>
struct push_back_impl<Item, List, true> {
  using type = push_front_t<Item, List>;
};
} // namespace details

template <typename Item, typename List>
struct push_back : details::push_back_impl<Item, List> {};

template <typename Item, typename List>
using push_back_t = typename push_back<Item, List>::type;

template <typename Item, typename List, bool = is_empty_v<List>> struct has {
  static constexpr bool value =
      !is_empty_v<List> && (std::is_same_v<Item, robust_front_t<List>> ||
                            has<Item, pop_front_t<List>>::value);
};

template <typename Item, typename List> struct has<Item, List, false> {
  static constexpr bool value = std::is_same_v<Item, front_t<List>> ||
                                has<Item, pop_front_t<List>>::value;
};

template <typename Item, typename List> struct has<Item, List, true> {
  static constexpr bool value = false;
};

template <typename Item, typename List>
constexpr bool has_v = has<Item, List>::value;

template <typename List, bool = (size_v<List>> 1)> struct remove_dup;

template <typename List> struct remove_dup<List, true> {
  using Head = front_t<List>;
  using Tail = pop_front_t<List>;
  using type =
      std::conditional_t<has_v<Head, Tail>, typename remove_dup<Tail>::type,
                         push_front_t<Head, typename remove_dup<Tail>::type>>;
};

template <typename List> struct remove_dup<List, false> { using type = List; };

template <typename List> using remove_dup_t = typename remove_dup<List>::type;

template <std::size_t I, typename List>
struct at : at<I - 1, pop_front_t<List>> {};

template <typename List> struct at<0, List> {
  using type = robust_front_t<List>;
};

template <std::size_t I, typename List> using at_t = typename at<I, List>::type;

template <std::size_t N, typename Item, typename List,
          std::size_t Diff = N - size_v<List>>
struct fill : fill<N, Item, push_back_t<Item, List>> {};

template <std::size_t N, typename Item, typename List>
struct fill<N, Item, List, 0> {
  using type = List;
};

template <std::size_t N, typename Item, typename List>
using fill_t = typename fill<N, Item, List>::type;

template <template <typename> typename Pred, typename List,
          bool = is_empty_v<List>>
struct check;

template <template <typename> typename Pred, typename List>
struct check<Pred, List, false> {
  static constexpr bool value =
      Pred<front_t<List>>::value && check<Pred, pop_front_t<List>>::value;
};

template <template <typename> typename Pred, typename List>
struct check<Pred, List, true> {
  static constexpr bool value = true;
};

template <template <typename> typename Pred, typename List>
constexpr bool check_v = check<Pred, List>::value;

template <template <typename> typename Map, typename List,
          bool = is_empty_v<List>>
struct map;

template <template <typename> typename Map, typename List>
struct map<Map, List, false> {
  using type = push_front_t<Map<front_t<List>>,
                            typename map<Map, pop_front_t<List>>::type>;
};

template <template <typename> typename Map, typename List>
struct map<Map, List, true> {
  using type = List;
};

template <template <typename> typename Pred, typename List>
using map_t = typename map<Pred, List>::type;

template <typename List1, typename List2, bool = is_empty_v<List2>>
struct concat;

template <typename List1, typename List2> struct concat<List1, List2, false> {
  using type = push_front_t<front_t<List2>,
                            typename concat<List1, pop_front_t<List2>>::type>;
};

template <typename List1, typename List2> struct concat<List1, List2, true> {
  using type = List1;
};

template <typename List1, typename List2>
using concat_t = typename concat<List1, List2>::type;

template <typename List1, typename List2>
using merge_t = remove_dup_t<concat_t<List1, List2>>;

template <typename... Lists> struct concat_all;

template <typename L1, typename L2, typename... Ls>
struct concat_all<L1, L2, Ls...> {
  using type = concat_t<L1, typename concat_all<L2, Ls...>::type>;
};

template <typename L> struct concat_all<L> { using type = L; };

template <typename... Lists>
using concat_all_t = typename concat_all<Lists...>::type;

template <template <typename> typename Pred, bool PredValue, typename List,
          bool = is_empty_v<List>>
struct filter;

template <template <typename> typename Pred, bool PredValue, typename List>
struct filter<Pred, PredValue, List, false> {
  using head = front_t<List>;
  using tail = typename filter<Pred, PredValue, pop_front_t<List>>::type;
  using type = std::conditional_t<Pred<head>::value == PredValue,
                                  push_front_t<head, tail>, tail>;
};

template <template <typename> typename Pred, bool PredValue, typename List>
struct filter<Pred, PredValue, List, true> {
  using type = List;
};

template <template <typename> typename Pred, bool PredValue, typename List>
using filter_t = typename filter<Pred, PredValue, List>::type;

template <template <typename...> typename TList, typename SList> struct rebind;

template <template <typename...> typename TList,
          template <typename...> typename SList, typename... Args>
struct rebind<TList, SList<Args...>> {
  using type = TList<Args...>;
};

template <template <typename...> typename TList, typename SList>
using rebind_t = typename rebind<TList, SList>::type;

///
/// @brief Basic list that can be used through the mvm::list algorithms
///
template <typename... Ts> struct mplist {};
} // namespace mvm::list