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

#include "mvm/instr_set.h"
#include "mvm/traits.h"
#include "test_common.h"

#include "gtest/gtest.h"

#include <type_traits>

using namespace mvm;
using namespace mvm::traits;
using namespace mvm::list;
using namespace mvm::test;

namespace {
struct test_instr_set2 : instr_set<test_instr_set2> {
  std::tuple<ui32, ui32> kui2() { return std::make_tuple(0, 1); }

  std::vector<double> kdn() { return {0.0, 1.1, 2.2}; }

  using endian_type = num::little_endian_tag;
  using me = test_instr_set2;
  using instr_table = instr_set_desc<
      producer_instr<producer<meta_value_stack, ui32, ui32>, false, &me::kui2,
                     MVM_TSTRING("kui2")>,
      producer_instr<producer<meta_value_stack, std::vector<double>>, false,
                     &me::kdn, MVM_TSTRING("kdn")>>;
};
} // namespace

TEST(instr_set_test, main) {
  using test_traits = instr_set_traits<test_instr_set>;
  using test_traits2 = instr_set_traits<test_instr_set2>;

  static_assert(
      std::is_same_v<num::little_endian_tag, typename test_traits::endian_type>,
      "[-][instr_set_test] bad endianness");

  static_assert(std::is_same_v<test_traits::set_stack_type, list::mplist<ui32>>,
                "[-][instr_set_test] bad vstack types");
  static_assert(
      std::is_same_v<test_traits2::set_stack_type, list::mplist<double, ui32>>,
      "[-][instr_set_test] bad vstack types");

  static_assert(
      std::is_same_v<list::at_t<0, test_instr_set::instr_table>::bytecode_type,
                     list::mplist<>>,
      "[-][instr_set_test] bad code types");
  static_assert(
      std::is_same_v<list::at_t<1, test_instr_set::instr_table>::bytecode_type,
                     list::mplist<ui32>>,
      "[-][instr_set_test] bad code types");

  EXPECT_STREQ(test_traits::instr_names[0], "zero");
  EXPECT_STREQ(test_traits::instr_names[1], "jump");
  EXPECT_STREQ(test_traits::instr_names[2], "dup");
  EXPECT_STREQ(test_traits::instr_names[3], "push");
  EXPECT_STREQ(test_traits::instr_names[4], "randn");
  EXPECT_STREQ(test_traits::instr_names[5], "rotln");
  EXPECT_STREQ(test_traits::instr_names[6], "add");
}

int instr_set_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "instr_set_test*";

  return RUN_ALL_TESTS();
}