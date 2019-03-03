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
#include <utility>

namespace mvm::reflect {
namespace detail {
// is detected idiom implementation details

template <typename Dummy, template <typename...> typename Op, typename... Args>
struct is_detected_impl : std::false_type {};

template <template <typename...> typename Op, typename... Args>
struct is_detected_impl<std::void_t<Op<Args...>>, Op, Args...>
    : std::true_type {};

// reflection implementation details

template <typename F, typename... Args,
          typename = decltype(std::declval<F>()(std::declval<Args &&>()...))>
auto is_valid_impl(void *) -> std::true_type;

template <typename F, typename... Args>
auto is_valid_impl(...) -> std::false_type;
} // namespace detail

// is detected custom implementation

template <template <typename...> typename Op, typename... Args>
struct is_detected : detail::is_detected_impl<void, Op, Args...> {};

template <template <typename...> typename Op, typename... Args>
using is_detected_t = typename is_detected<Op, Args...>::type;

template <template <typename...> typename Op, typename... Args>
constexpr bool is_detected_v = is_detected_t<Op, Args...>::value;

// general compile-time validator (ref: C++ template the complete guide)

inline constexpr auto is_valid = [](auto f) {
  using input_type = decltype(f);
  return [](auto &&... args) {
    return decltype(
        detail::is_valid_impl<input_type, decltype(args) &&...>(nullptr)){};
  };
};

template <typename T> struct type_t { using type = T; };

template <typename T> constexpr auto type = type_t<T>{};

template <typename T> T value_t(type_t<T>);
} // namespace mvm::reflect
