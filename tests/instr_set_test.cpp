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

#include "gtest/gtest.h"
#include <type_traits>

#include "mvm/instr_set.h"
#include "mvm/instr_set_traits.h"
#include "test_common.h"

using namespace mvm;
using namespace mvm::traits;
using namespace mvm::list;
using namespace mvm::test;

TEST(instr_set_test, main) {
  using test_traits = instr_set_traits<test_instr_set>;
  using test2_traits = instr_set_traits<test_instr_set2>;

  // set 1
  static_assert(
      std::is_same_v<num::little_endian_tag, typename test_traits::endian_type>,
      "[-][instr_set_test] bad endianness");
  static_assert(
      std::is_same_v<mplist<ui32>, typename test_traits::stack_list_type>,
      "[-][instr_set_test] bad stack list type");

  // set 2
  static_assert(
      std::is_same_v<num::big_endian_tag, typename test2_traits::endian_type>,
      "[-][instr_set_test] bad endianness");
  static_assert(std::is_same_v<mplist<double, ui32>,
                               typename test2_traits::stack_list_type>,
                "[-][instr_set_test] bad stack list type");

  // no runtime test
  EXPECT_TRUE(true);
}

int instr_set_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "instr_set_test*";

  return RUN_ALL_TESTS();
}