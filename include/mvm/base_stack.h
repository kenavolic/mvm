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

#include "mvm/except.h"
#include "mvm/helpers/list.h"

namespace mvm {
namespace traits {
  
template <typename TypeList, std::size_t = list::size_v<TypeList>>
struct vstack_traits {

  using value_type = list::rebind_t<std::variant, TypeList>;
  using stack_type = std::vector<value_type>;

  template <typename T>
  static auto get_val(value_type&& val)
  {
    return std::get<T>(std::forward<value_type>(val));
  }
};

template <typename TypeList> 
struct vstack_traits<TypeList, 1> {
  using value_type = list::front_t<TypeList>;
  using stack_type = std::vector<value_type>;

  template <typename T>
  static auto get_val(T val)
  {
    return val;
  }
};

} // namespace detail

///
/// @brief Heterogenerous value stack
///
template <typename TypeList>
class vstack 
{
  using vstack_traits = traits::vstack_traits<TypeList>;
  using value_type = typename vstack_traits::value_type;
  using stack_type = typename vstack_traits::stack_type;
  stack_type m_stack;
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
      if (m_stack.empty())
      {
        throw mexcept("[-][mvm] try to pop from empty stack", status_type::POP_EMPTY_STACK);
      }
      auto val = m_stack.back();
      m_stack.pop_back();
      return vstack_traits::template get_val<T>(std::move(val));
    }
};
} // namespace mvm