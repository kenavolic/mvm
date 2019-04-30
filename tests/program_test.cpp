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

#include "mvm/program.h"

#include "gtest/gtest.h"

using namespace mvm;

TEST(program_test, ip_main) {
  rebasable_ip ptr;
  ip &ptr_ip = static_cast<ip &>(ptr);

  ASSERT_EQ(ptr, 0);
  ptr_ip = 666;
  EXPECT_EQ(ptr, 666);
  ++ptr_ip;
  EXPECT_EQ(ptr, 667);
  ptr_ip += 3;
  EXPECT_EQ(ptr, 670);
  ptr_ip -= 4;
  EXPECT_EQ(ptr, 666);

  ip ptr_ip2;
  EXPECT_TRUE(ptr_ip2 < ptr_ip);
  EXPECT_TRUE(ptr_ip2 <= ptr_ip);
  EXPECT_TRUE(ptr_ip2 <= ptr_ip2);
}

TEST(program_test, ip_limits) {
  rebasable_ip ptr;
  ip &ptr_ip = static_cast<ip &>(ptr);

  ASSERT_EQ(ptr, 0);

  ptr_ip = std::numeric_limits<uintptr_t>::max();
  EXPECT_EQ(ptr, std::numeric_limits<uintptr_t>::max());

  ptr.rebase(1, std::numeric_limits<uintptr_t>::max());
  EXPECT_THROW((ptr_ip = std::numeric_limits<uintptr_t>::max()), mexcept);

  ptr_ip += (std::numeric_limits<uintptr_t>::max() - 1);
  EXPECT_EQ(ptr, std::numeric_limits<uintptr_t>::max());

  EXPECT_THROW(++ptr_ip, mexcept);
  EXPECT_THROW((ptr_ip += 1), mexcept);

  ptr_ip = 1;
  EXPECT_THROW((ptr_ip -= 3), mexcept);
}

TEST(program_test, rebasable_main) {

  uint8_t bytes[4] = {0xde, 0xad, 0xbe, 0xef};
  rebasable_ip ptr;
  ptr.rebase(bytes, bytes + 3);

  EXPECT_EQ(*ptr, 0xde);
  ++ptr;
  EXPECT_EQ(*ptr, 0xad);
  ptr += 2;
  EXPECT_EQ(*ptr, 0xef);
  EXPECT_TRUE(ptr.assert_in_chunk());
  ++ptr;
  EXPECT_FALSE(ptr.assert_in_chunk());
}

int program_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "program_test*";

  return RUN_ALL_TESTS();
}