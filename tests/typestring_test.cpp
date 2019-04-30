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

#include "mvm/helpers/typestring.h"

#include "gtest/gtest.h"

#include <type_traits>

using namespace mvm::typestring;

TEST(typestring_test, main) {

  static_assert(std::is_same_v<MVM_TSTRING("test"),
                               type_string<'t', 'e', 's', 't', '\0'>>,
                "[-][typestring_test] is_empty failed");

  using test_str_as_type = MVM_TSTRING("test");

  EXPECT_STREQ(char_seq<test_str_as_type>::value, "test");
}

int typestring_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "typestring_test*";

  return RUN_ALL_TESTS();
}