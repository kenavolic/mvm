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

namespace mvm {

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

  template <bool DoUpdateIp, typename Consumers, typename Producers, typename S>
  struct base_instr {
    // all the following static types and values are just shortcuts
    // for easy access to instructions props
    using name_type = S;
    using value_stack_type = value_stack_types_t<Producers>;
    using bytecode_type = bytecode_types_t<Consumers>;
    using consumers_type = Consumers;
    using producers_type = Producers;

    static constexpr bool doUpdateIp = DoUpdateIp;
  };

  template <typename Consumers, typename Producers, bool DoUpdateIp,
            typename details::desc_to_proto<Set, Producers, Consumers,
                                            DoUpdateIp>::type Func,
            typename S>
  struct generic_instr : base_instr<DoUpdateIp, Consumers, Producers, S> {
    template <typename VM, typename... Args>
    static auto apply(VM &vm, Args &&... args) {
      return (vm.*Func)(std::forward<Args>(args)...);
    }
  };

public:
  template <typename Consumers, typename Producer, bool DoUpdateIp, auto Func,
            typename S>
  struct consumers_producer_instr
      : generic_instr<Consumers, producers<Producer>, DoUpdateIp, Func, S> {};

  template <typename Consumers, bool DoUpdateIp, auto Func, typename S>
  struct consumers_instr
      : generic_instr<Consumers, no_prod, DoUpdateIp, Func, S> {};

  template <typename Consumer, typename Producer, bool DoUpdateIp, auto Func,
            typename S>
  struct consumer_producer_instr
      : generic_instr<consumers<Consumer>, producers<Producer>, DoUpdateIp,
                      Func, S> {};

  template <typename Consumer, bool DoUpdateIp, auto Func, typename S>
  struct consumer_instr
      : generic_instr<consumers<Consumer>, no_prod, DoUpdateIp, Func, S> {};

  template <typename Producer, bool DoUpdateIp, auto Func, typename S>
  struct producer_instr
      : generic_instr<no_cons, producers<Producer>, DoUpdateIp, Func, S> {};

  template <typename Consumer, typename Producer, typename S>
  struct consumer_producer_pipe
      : base_instr<false, consumers<Consumer>, producers<Producer>, S> {
    template <typename VM, typename Arg> static auto apply(VM &vm, Arg arg) {
      return arg;
    }
  };

  template <typename Consumer, typename S>
  struct consumer_pipe : base_instr<false, consumers<Consumer>, no_prod, S> {
    template <typename VM, typename Arg> static void apply(VM &vm, Arg arg) {
      // sink
    }
  };

  ///
  /// @brief base struct to define value representation in byte codes
  ///
  /// Default behavior is to used the size of the c++ type as size in the
  /// byte code but specific behavior can be defined by specializing this
  /// template. Default endianness is the endianness defined for the instruction
  /// set.
  ///
  /// Expected format for signed integer: 2-complement
  /// Expected format for floating point number: IEE754
  ///
  template <typename T> struct code_value_repr {
    static constexpr std::size_t size = sizeof(T);
    using endian_type = typename instr_set_type::endian_type;
    // override endianness
  };
};

///
/// @brief Instruction set visitor used to call the right instruction callback
///  according to the instruction index.
///
/// @warning For small instruction set, the performance is ok, but macro
/// unrolling
///          is better when the set size increases.
///
template <typename TList, size_t N = list::size_v<TList> - 1>
struct instr_set_visitor {
  template <typename Callable> void operator()(size_t n, Callable &&c) const {
    if (N == n) {
      using instr_type = list::at_t<N, TList>;
      std::forward<Callable>(c)(instr_type{});
    } else {
      instr_set_visitor<TList, N - 1>()(n, std::forward<Callable>(c));
    }
  }
};

template <typename TList> struct instr_set_visitor<TList, 0> {
  template <typename Callable> void operator()(size_t n, Callable &&c) const {
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