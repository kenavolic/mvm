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

#include "mvm/except.h"
#include "mvm/trace.h"
#include "mvm/traits.h"

namespace mvm {

///
/// @brief Heterogenerous value stack
///
template <typename TypeList> class value_stack {
  using value_stack_traits = traits::value_stack_traits<TypeList>;
  using value_type = typename value_stack_traits::value_type;
  using stack_type = typename value_stack_traits::stack_type;
  stack_type m_stack;

public:
  ///
  /// @brief Push data to the stack
  ///
  template <typename T> void push(T &&val) {
    LOG_INFO("value_stack -> push " << val << " on stack[" << this << "]");
    m_stack.push_back(value_type{std::forward<T>(val)});
  }

  ///
  /// @brief Pop data from the stack
  ///
  template <typename T> T pop() {
    if (m_stack.empty()) {
      throw mexcept("[-][mvm] try to pop from empty stack",
                    status_type::POP_EMPTY_STACK);
    }
    auto val = m_stack.back();
    m_stack.pop_back();
    LOG_INFO("value_stack -> pop from stack[" << this << "]");
    return value_stack_traits::template get_val<T>(std::move(val));
  }
};
} // namespace mvm