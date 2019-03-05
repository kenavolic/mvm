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
#include "mvm/instr.h"
#include "mvm/instr_set.h"
#include "mvm/macros.h"
#include "mvm/types.h"

namespace mvm::test {
struct test_instr_set : instr_set<test_instr_set> {

  std::vector<int> call_stack;
  ui32 res{};

  // a
  instr_ret_t<stack_producer<ui32>> spsccc(stack_consumer<ui32> const &arg1,
                                           code_consumer<ui32, ui32> const &arg2) {
    call_stack.push_back(0);
    return update_stack(stack_at<0>(arg1) + code_at<0>(arg2));
  }

  // b
  instr_ret_t<stack_producer<ui32>> spsc(stack_consumer<ui32> const &arg) {
    call_stack.push_back(1);
    // test update ip stack
    return update_ip_stack(1, stack_at<0>(arg));
  }

  // c
  instr_ret_t<stack_producer<ui32>> spcc(code_consumer<ui32> const &arg) {
    call_stack.push_back(2);
    return update_stack(code_at<0>(arg));
  }

  // d
  instr_ret_t<nonsuch> sccc(stack_consumer<ui32> const &arg1,
                            code_consumer<ui32> const &arg2) {
    call_stack.push_back(3);
    return update_none();
  }

  // e
  instr_ret_t<stack_producer<ui32>> sp() {
    call_stack.push_back(4);
    return update_stack(1);
  }

  // f
  instr_ret_t<nonsuch> sc(stack_consumer<ui32> const &arg) {
    call_stack.push_back(5);
    res = stack_at<0>(arg);
    return update_none();
  }

  // g
  instr_ret_t<nonsuch> cc(code_consumer<ui32> const &arg) {
    call_stack.push_back(6);
    return update_ip(code_at<0>(arg));
  }

  // byte code endianness
  using endian_type = num::little_endian_tag;

  // instruction table
  using instr_table = list::mplist<
      i_spsccc_row<stack_producer<ui32>, stack_consumer<ui32>,
                   code_consumer<ui32, ui32>, &test_instr_set::spsccc,
                   MVM_CHAR_LIST(2, a)>,
      i_spsc_row<stack_producer<ui32>, stack_consumer<ui32>,
                 &test_instr_set::spsc, MVM_CHAR_LIST(2, b)>,
      i_spcc_row<stack_producer<ui32>, code_consumer<ui32>,
                 &test_instr_set::spcc, MVM_CHAR_LIST(2, c)>,
      i_sccc_row<stack_consumer<ui32>, code_consumer<ui32>,
                 &test_instr_set::sccc, MVM_CHAR_LIST(2, d)>,
      i_sp_row<stack_producer<ui32>, &test_instr_set::sp, MVM_CHAR_LIST(2, e)>,
      i_sc_row<stack_consumer<ui32>, &test_instr_set::sc, MVM_CHAR_LIST(2, f)>,
      i_cc_row<code_consumer<ui32>, &test_instr_set::cc, MVM_CHAR_LIST(2, g)>>;
};

struct test_instr_set2 : instr_set<test_instr_set2> {
  instr_ret_t<stack_producer<ui32>> toint(stack_consumer<double> const &arg1) {
    return update_stack(static_cast<double>(stack_at<0>(arg1)));
  }

  instr_ret_t<stack_producer<double>> pushd(code_consumer<double> const &arg) {
    return update_stack(code_at<0>(arg));
  }

  // byte code endianness
  using endian_type = num::big_endian_tag;

  // instruction table
  using instr_table = list::mplist<
      i_spsc_row<stack_producer<ui32>, stack_consumer<double>,
                 &test_instr_set2::toint, MVM_CHAR_LIST(6, toint)>,
      i_spcc_row<stack_producer<double>, code_consumer<double>,
                 &test_instr_set2::pushd, MVM_CHAR_LIST(6, pushd)>>;
};
} // namespace mvm::test