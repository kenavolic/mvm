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

#include "mvm/except.h"
#include "mvm/helpers/list.h"
#include "mvm/helpers/utils.h"
#include "mvm/instr.h"

namespace mvm {
namespace detail {
// different types of instruction

// sc: stack consumer (needs value from stack)
template <typename Arg> struct i_sc_impl { using stack_consumer_type = Arg; };

// sp: stack producer (produce value to stack)
template <typename Arg> struct i_sp_impl { using stack_producer_type = Arg; };

// cc: code consumer (needs value from bytecode)
template <typename Arg> struct i_cc_impl { using code_consumer_type = Arg; };

// base instr
template <char... Chars> struct i_base {
  static constexpr char const name[sizeof...(Chars)] = {Chars...};
};
} // namespace detail

template <typename... Ts> using instr_set_desc = list::mplist<Ts...>;

///
/// @brief Base class used to define an instruction set
///
/// This is the front end of a vm implementation that
/// describes the set.
///
template <typename Set> struct instr_set {
private:
  using instr_set_type = Set;

  // utility internal struct used to gather code
  // for handling of instruction callback
  template <typename T, T Func> struct i_apply;

  template <typename Ret, typename... Args,
            Ret (instr_set_type::*Func)(Args...)>
  struct i_apply<Ret (instr_set_type::*)(Args...), Func> {
    template <typename VM> static Ret apply(VM &vm, Args &&... args) {
      return (vm.*Func)(std::forward<Args>(args)...);
    }
  };

public:
  ///
  /// @brief stack-producer, stack-consumer, code-consumer instr
  ///
  template <typename SP, typename SC, typename CC,
            instr_ret_t<SP> (instr_set_type::*Func)(SC const &, CC const &),
            char... Chars>
  struct i_spsccc_row
      : detail::i_sp_impl<SP>,
        detail::i_sc_impl<SC>,
        detail::i_cc_impl<CC>,
        detail::i_base<Chars...>,
        i_apply<instr_ret_t<SP> (instr_set_type::*)(SC const &, CC const &),
                Func> {};

  ///
  /// @brief stack-producer, stack-consumer instr
  ///
  template <typename SP, typename SC,
            instr_ret_t<SP> (instr_set_type::*Func)(SC const &), char... Chars>
  struct i_spsc_row
      : detail::i_sp_impl<SP>,
        detail::i_sc_impl<SC>,
        detail::i_base<Chars...>,
        i_apply<instr_ret_t<SP> (instr_set_type::*)(SC const &), Func> {};

  ///
  /// @brief stack-producer, code-consumer instr
  ///
  template <typename SP, typename CC,
            instr_ret_t<SP> (instr_set_type::*Func)(CC const &), char... Chars>
  struct i_spcc_row
      : detail::i_sp_impl<SP>,
        detail::i_cc_impl<CC>,
        detail::i_base<Chars...>,
        i_apply<instr_ret_t<SP> (instr_set_type::*)(CC const &), Func> {};

  ///
  /// @brief stack-consumer, code-consumer instr
  ///
  template <typename SC, typename CC,
            instr_ret_t<nonsuch> (instr_set_type::*Func)(SC const &,
                                                         CC const &),
            char... Chars>
  struct i_sccc_row : detail::i_sc_impl<SC>,
                      detail::i_cc_impl<CC>,
                      detail::i_base<Chars...>,
                      i_apply<instr_ret_t<nonsuch> (instr_set_type::*)(
                                  SC const &, CC const &),
                              Func> {};

  ///
  /// @brief stack-producer
  ///
  template <typename SP, instr_ret_t<SP> (instr_set_type::*Func)(),
            char... Chars>
  struct i_sp_row : detail::i_sp_impl<SP>,
                    detail::i_base<Chars...>,
                    i_apply<instr_ret_t<SP> (instr_set_type::*)(), Func> {};

  ///
  /// @brief stack-consumer
  ///
  template <typename SC,
            instr_ret_t<nonsuch> (instr_set_type::*Func)(SC const &),
            char... Chars>
  struct i_sc_row
      : detail::i_sc_impl<SC>,
        detail::i_base<Chars...>,
        i_apply<instr_ret_t<nonsuch> (instr_set_type::*)(SC const &), Func> {};

  ///
  /// @brief code-consumer instr
  ///
  template <typename CC,
            instr_ret_t<nonsuch> (instr_set_type::*Func)(CC const &),
            char... Chars>
  struct i_cc_row
      : detail::i_cc_impl<CC>,
        detail::i_base<Chars...>,
        i_apply<instr_ret_t<nonsuch> (instr_set_type::*)(CC const &), Func> {};

  ///
  /// @brief base struct to define value representation in byte codes
  ///
  /// Default behavior is to used the size of the c++ type as size in the
  /// byte code but specific behavior can be defined by specializing this
  /// template
  ///
  /// Expected format for signed integer: 2-complement
  /// Expected format for floating point number: IEE754
  ///
  template <typename T> struct code_value_repr {
    static constexpr std::size_t size = sizeof(T);
  };
};

template <typename TList, size_t N = list::size_v<TList> - 1>
struct instr_set_visitor {
  template <typename Callable> void operator()(size_t n, Callable &&c) {
    if (N == n) {
      using instr_type = list::at_t<N, TList>;
      std::forward<Callable>(c)(instr_type{});
    } else {
      instr_set_visitor<TList, N - 1>()(n, std::forward<Callable>(c));
    }
  }
};

template <typename TList> struct instr_set_visitor<TList, 0> {
  template <typename Callable> void operator()(size_t n, Callable &&c) {
    if (n == 0) {
      using instr_type = list::at_t<0, TList>;
      std::forward<Callable>(c)(instr_type{});
    } else {
      throw mexcept("[-][mvm] instruction opcode overflow",
                    status_type::INVALID_INSTR_OPCODE);
    }
  }
};
} // namespace mvm