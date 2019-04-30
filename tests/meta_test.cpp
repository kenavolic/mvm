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

#include "mvm/meta.h"

#include "gtest/gtest.h"

#include <tuple>
#include <type_traits>

using namespace mvm;
using namespace mvm::list;

namespace {
struct bytecode_impl {};
struct stack_impl {};
} // namespace

TEST(meta_test, main) {

  static_assert(!mvm::details::is_instance_of<std::tuple, int>::value,
                "[-][meta_test] is_instance_of failed");
  static_assert(
      mvm::details::is_instance_of<std::tuple, std::tuple<int>>::value,
      "[-][meta_test] is_instance_of failed");
  static_assert(
      std::is_same_v<bytecode_impl,
                     instance_of_t<mplist<meta_bytecode<bytecode_impl>,
                                          meta_value_stack<stack_impl>>,
                                   meta_bytecode>>,
      "[-][meta_test] instance_of failed");

  static_assert(
      std::is_same_v<bytecode_impl,
                     instance_of_tie_t<mplist<meta_bytecode<bytecode_impl>,
                                              meta_value_stack<stack_impl>>,
                                       meta_tie<meta_bytecode, int>>>,
      "[-][meta_test] instance_of failed");

  // no runtime test
  EXPECT_TRUE(true);
}

int meta_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "meta_test*";

  return RUN_ALL_TESTS();
}