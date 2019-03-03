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

#include <optional>
#include <tuple>

#include "mvm/helpers/utils.h"

namespace mvm {

using ip_data = int;

///
/// @brief stack consumer type used by instruction as
///       parameters when they consume data from the stack
///
/// If an instruction consumes 2 integers from the stack
/// they will set their consumer type to stack_consumer<int, int>
///
template <typename... Args> using stack_consumer = std::tuple<Args...>;

///
/// @brief stack producer type used by instruction as
///       return type when they produce data to the stack
///
/// If an instruction produces 2 integers to the stack
/// they will set their producer type to stack_producer<int, int>
///
template <typename... Args> using stack_producer = std::tuple<Args...>;

///
/// @brief code consumer type used by instruction as
///       parameters when they need to retrieve data
///       from the byte code
///
/// If an instruction needs to retrieve an integer from
/// the code chunk, they will set their consumer type to
/// code_consumer<int>
///
/// For each data type consumed from the byte codes, a vm
/// definition must provide an instantiation of data_representation
/// to guide the intepreter in the bytecode parsing step
///
template <typename... Args> using code_consumer = std::tuple<Args...>;

///
/// @brief instruction return type
///
/// Instruction return type consists of the stack
/// producer part and the optional ip update part
///
template <typename Ret> struct instr_ret {
  using type = std::tuple<Ret, std::optional<ip_data>>;
};

///
/// @brief instruction return type
///
/// Specific return type whith no stack producer part
///
template <> struct instr_ret<nonsuch> { using type = std::optional<ip_data>; };

template <typename Ret> using instr_ret_t = typename instr_ret<Ret>::type;

///
///@brief Utility function to be used when an instruction
///       makes no production to the stack and does not
///       update the instruction pointer
///
inline auto update_none() { return instr_ret_t<nonsuch>(); }

///
///@brief Utility function to be used when an instruction
///       makes no production to the stack but updates
///       the instruction pointer
///
inline auto update_ip(ip_data ip) { return instr_ret_t<nonsuch>(ip); }

///
///@brief Utility function to be used when an instruction
///       makes production to the stack but does not
///       update the instruction pointer
///
template <typename... Args> auto update_stack(Args &&... args) {
  return instr_ret_t<stack_producer<Args...>>(
      stack_producer<Args...>(std::forward<Args>(args)...), std::nullopt);
}

///
///@brief Utility function to be used when an instruction
///       makes production to the stack and updates
///       the instruction pointer
///
template <typename... Args> auto update_ip_stack(ip_data ip, Args &&... args) {
  return instr_ret_t<stack_producer<Args...>>(
      stack_producer<Args...>(std::forward<Args>(args)...), ip);
}

///
/// @brief Utility function to hide dependency to std::tuple
///
template <std::size_t I, typename Arg> decltype(auto) stack_at(Arg &&arg) {
  return std::get<I>(std::forward<Arg>(arg));
}

///
/// @brief Utility function to hide dependency to std::tuple
///
template <std::size_t I, typename Arg> decltype(auto) code_at(Arg &&arg) {
  return std::get<I>(std::forward<Arg>(arg));
}
} // namespace mvm