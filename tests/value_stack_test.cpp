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

#include "mvm/helpers/list.h"
#include "mvm/value_stack.h"

using namespace mvm;
using namespace list;

TEST(value_stack_test, single_type) {
  using stack = value_stack<mplist<int>>;

  stack s;
  s.push(1);

  EXPECT_EQ(1, s.template pop<int>());
}

TEST(value_stack_test, multiple_type) {
  using stack = value_stack<mplist<int, double>>;

  stack s;
  s.push(1);

  EXPECT_EQ(1, s.template pop<int>());

  s.push(1);
  s.push(2.0);

  EXPECT_EQ(2.0, s.template pop<double>());
  EXPECT_EQ(1, s.template pop<int>());
}

TEST(value_stack_test, multiple_type_bad_type) {
  using stack = value_stack<mplist<int, double>>;

  stack s;

  s.push(1);
  s.push(2.0);

  EXPECT_ANY_THROW(s.template pop<int>());
}

int value_stack_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "value_stack_test*";

  return RUN_ALL_TESTS();
}