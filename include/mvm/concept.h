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
#include "mvm/helpers/reflect.h"
#include "mvm/meta.h"

// TODO: To be removed in the future
namespace mvm::concept {
  template <typename T> struct requires_t {
    static_assert(T::value, "[-][mvm] requirement not met");
  };

  template <bool B> struct requires_v {
    static_assert(B, "[-][mvm] requirement not met");
  };

  // requirements
  template <typename I> inline constexpr bool is_code_consumer() noexcept {
    return !list::is_empty_v<typename I::bytecode_type>;
  }

  template <typename T>
  inline constexpr bool is_container_valid_v =
      reflect::has_iterator(reflect::type<T>) &&
      reflect::has_value_type(reflect::type<T>);

  template <typename T>
  inline constexpr bool is_iterable_consumer_v =
      reflect::has_counter_type(reflect::type<T>);

  template <typename T>
  inline constexpr bool is_tuple_v =
      reflect::is_specialization_of_v<std::tuple, T>;

  template <typename I>
  inline constexpr bool is_consumer_v =
      !std::is_same_v<typename I::consumers_type, no_cons>;

  template <typename I>
  inline constexpr bool is_code_consumer_v =
      !list::is_empty_v<typename I::bytecode_type>;

  template <typename I>
  inline constexpr bool is_producer_v =
      !std::is_same_v<typename I::producers_type, no_prod>;

  template <typename I> inline constexpr bool is_ip_udpater_v = I::doUpdateIp;

  template <template <typename> typename Meta>
  inline constexpr bool is_meta_bytecode_v =
      reflect::is_same_meta_v<Meta, meta_bytecode>;
} // namespace mvm::concept