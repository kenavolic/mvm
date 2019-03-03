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

#include <variant>
#include <vector>

#include "mvm/helpers/list.h"

namespace mvm {
namespace detail {
template <typename TypeList, std::size_t = list::size_v<TypeList>>
class vstack_impl {
  using value_type = list::rebind_t<std::variant, TypeList>;
  std::vector<value_type> m_stack;

public:
  ///
  /// @brief Push data to the stack
  ///
  template <typename T> void push(T &&val) {
    m_stack.push_back(value_type{std::forward<T>(val)});
  }

  ///
  /// @brief Pop data from the stack
  ///
  template <typename T> T pop() {
    auto val = m_stack.back();
    m_stack.pop_back();
    return std::get<T>(val);
  }
};

template <typename TypeList> class vstack_impl<TypeList, 1> {
  std::vector<list::front_t<TypeList>> m_stack;

public:
  ///
  /// @brief Push data to the stack
  ///
  template <typename T> void push(T &&val) {
    m_stack.push_back(std::forward<T>(val));
  }

  ///
  /// @brief Pop data from the stack
  ///
  template <typename T> T pop() {
    auto val = m_stack.back();
    m_stack.pop_back();
    return val;
  }
};

} // namespace detail

///
/// @brief Heterogenerous value stack
///
template <typename TypeList>
class vstack : public detail::vstack_impl<TypeList> {};
} // namespace mvm