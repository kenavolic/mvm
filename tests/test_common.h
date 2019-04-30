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

#include "mvm/helpers/num_parse.h"
#include "mvm/instr_set.h"
#include "mvm/macros.h"
#include "mvm/types.h"

namespace mvm::test {

struct test_instr_set : instr_set<test_instr_set> {
  std::vector<unsigned> call_stack;

  ui32 zero() {
    call_stack.push_back(0);
    return 0;
  }

  // code consumer, eip modifier
  void jump(ip &eip, ui32 val) {
    call_stack.push_back(1);
    eip = val;
  }

  // stack producer, consumer
  std::tuple<ui32, ui32> dup(ui32 val) {
    call_stack.push_back(2);
    return std::make_tuple(val, val);
  }

  // pop from code and push to stack = pipe
  /*ui32 push(ui32 val)
  {
      return val;
  }*/

  // piping (pop n from code and push n value to the stack)
  std::vector<ui32> randn(ui32 n) {
    call_stack.push_back(4);
    return std::vector<ui32>(n, 1);
  }

  // pop n from the code <-> pop n value from the stack, push n modified to the
  // stack
  std::vector<ui32> rotln(std::vector<ui32> &&vec) {
    std::cout << "call_stack.push_back(5)" << std::endl;
    call_stack.push_back(5);
    // Does not anything ;)
    return vec;
  }

  ui32 add(ui32 a, ui32 b) {
    call_stack.push_back(6);
    return a + b;
  }

  using endian_type = num::little_endian_tag;

  using me = test_instr_set;
  using instr_table = instr_set_desc<
      producer_instr<producer<meta_value_stack, ui32>, false, &me::zero,
                     MVM_TSTRING("zero")>,
      consumer_instr<consumer<meta_bytecode, ui32>, true, &me::jump,
                     MVM_TSTRING("jump")>,
      consumer_producer_instr<consumer<meta_value_stack, ui32>,
                              producer<meta_value_stack, ui32, ui32>, false,
                              &me::dup, MVM_TSTRING("dup")>,
      consumer_producer_pipe<consumer<meta_bytecode, ui32>,
                             producer<meta_value_stack, ui32>,
                             MVM_TSTRING("push")>,
      consumer_producer_instr<consumer<meta_bytecode, ui32>,
                              producer<meta_value_stack, std::vector<ui32>>,
                              false, &me::randn, MVM_TSTRING("randn")>,
      consumer_producer_instr<
          iterable_consumer<meta_value_stack, std::vector<ui32> &&,
                            count_from<consumer<meta_bytecode, ui32>>>,
          producer<meta_value_stack, std::vector<ui32>>, false, &me::rotln,
          MVM_TSTRING("rotln")>,
      consumer_producer_instr<consumer<meta_value_stack, ui32, ui32>,
                              producer<meta_value_stack, ui32>, false, &me::add,
                              MVM_TSTRING("add")>>;
};

// new concept to handle stack for double
template <typename T> struct meta_double_stack {};

struct test_common_mixed_arithmetic : instr_set<test_common_mixed_arithmetic> {
  std::tuple<ui32, ui32> kui2() { return std::make_tuple(0, 1); }

  ui32 kui() { return 1; }

  std::vector<double> kdn() { return {0.0, 1.1, 2.2}; }

  double kd1() { return 1; }

  double ufadd(ui32 a, double b) { return static_cast<double>(a) + b; }

  using endian_type = num::little_endian_tag;
};

struct test_instr_set_extra : test_common_mixed_arithmetic {

  using me = test_instr_set_extra;
  using instr_table = instr_set_desc<
      producer_instr<producer<meta_value_stack, ui32, ui32>, false, &me::kui2,
                     MVM_TSTRING("kui2")>,
      producer_instr<producer<meta_double_stack, std::vector<double>>, false,
                     &me::kdn, MVM_TSTRING("kdn")>,
      producer_instr<producer<meta_value_stack, ui32>, false, &me::kui,
                     MVM_TSTRING("kui")>,
      producer_instr<producer<meta_double_stack, double>, false, &me::kd1,
                     MVM_TSTRING("kd1")>,
      // from right to left
      consumers_producer_instr<consumers<consumer<meta_double_stack, double>,
                                         consumer<meta_value_stack, ui32>>,
                               producer<meta_double_stack, double>, false,
                               &me::ufadd, MVM_TSTRING("ufadd")>>;
};

struct test_instr_set_mixed : test_common_mixed_arithmetic {
  using me = test_instr_set_extra;
  using instr_table = instr_set_desc<
      producer_instr<producer<meta_value_stack, ui32, ui32>, false, &me::kui2,
                     MVM_TSTRING("kui2")>,
      producer_instr<producer<meta_value_stack, std::vector<double>>, false,
                     &me::kdn, MVM_TSTRING("kdn")>,
      producer_instr<producer<meta_value_stack, ui32>, false, &me::kui,
                     MVM_TSTRING("kui")>,
      producer_instr<producer<meta_value_stack, double>, false, &me::kd1,
                     MVM_TSTRING("kd1")>,
      // from right to left
      consumers_producer_instr<consumers<consumer<meta_value_stack, double>,
                                         consumer<meta_value_stack, ui32>>,
                               producer<meta_value_stack, double>, false,
                               &me::ufadd, MVM_TSTRING("ufadd")>>;
};
} // namespace mvm::test