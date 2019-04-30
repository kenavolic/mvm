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

#include <cstdint>
#include <limits>

#include "mvm/helpers/num_parse.h"

using namespace mvm::num;

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
using default_endian_type = little_endian_tag;
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
using default_endian_type = big_endian_tag;
#endif

TEST(num_parse_test, parse_unsigned) {
  uint32_t expected32 = 0xcafebabe;
  auto res32 = parse<uint32_t, sizeof(uint32_t), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&expected32));
  EXPECT_EQ(res32, expected32);

  uint16_t expected16 = 0xcafe;
  auto res16 = parse<uint16_t, sizeof(uint16_t), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&expected16));
  EXPECT_EQ(res16, expected16);

  uint8_t expected8 = 0xfe;
  auto res8 = parse<uint8_t, sizeof(uint8_t), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&expected8));
  EXPECT_EQ(res8, expected8);
}

TEST(num_parse_test, parse_unsigned_limits) {
  uint32_t max32 = std::numeric_limits<uint32_t>::max();
  auto res32 = parse<uint32_t, sizeof(uint32_t), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&max32));
  EXPECT_EQ(res32, max32);

  uint32_t min32 = std::numeric_limits<uint32_t>::min();
  res32 = parse<uint32_t, sizeof(uint32_t), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&min32));
  EXPECT_EQ(res32, min32);
}

TEST(num_parse_test, parse_signed) {
  int32_t expected32 = 32;
  auto res32 = parse<int32_t, sizeof(int32_t), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&expected32));
  EXPECT_EQ(res32, expected32);

  int32_t nexpected32 = -64;
  auto nres32 = parse<int32_t, sizeof(int32_t), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&nexpected32));
  EXPECT_EQ(nres32, nexpected32);
}

TEST(num_parse_test, parse_signed_limits) {
  int32_t max32 = std::numeric_limits<int32_t>::max();
  auto res32 = parse<int32_t, sizeof(int32_t), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&max32));
  EXPECT_EQ(res32, max32);

  int32_t min32 = std::numeric_limits<int32_t>::min();
  res32 = parse<int32_t, sizeof(int32_t), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&min32));
  EXPECT_EQ(res32, min32);
}

TEST(num_parse_test, parse_float) {
  uint32_t f_as_ui = 0x3fc00000; // 1.5
  auto resf32 = parse<float, sizeof(float), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&f_as_ui));
  EXPECT_EQ(resf32, 1.5);

  f_as_ui = 0x3f900000; // 1.125
  resf32 = parse<float, sizeof(float), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&f_as_ui));
  EXPECT_EQ(resf32, 1.125);

  f_as_ui = 0xc0100000; // -2.25
  resf32 = parse<float, sizeof(float), default_endian_type>(
      reinterpret_cast<uint8_t const *>(&f_as_ui));
  EXPECT_EQ(resf32, -2.25);
}

TEST(num_parse_test, parse_double) {
  uint8_t f_as_ui[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf8, 0x3f};
  auto resf64 = parse<double, sizeof(double), little_endian_tag>(
      reinterpret_cast<uint8_t const *>(&f_as_ui));
  EXPECT_EQ(resf64, 1.5);

  uint8_t f_as_ui2[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf2, 0x3f};
  resf64 = parse<double, sizeof(double), little_endian_tag>(
      reinterpret_cast<uint8_t const *>(&f_as_ui2));
  EXPECT_EQ(resf64, 1.125);

  uint8_t f_as_ui3[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xc0};
  resf64 = parse<double, sizeof(double), little_endian_tag>(
      reinterpret_cast<uint8_t const *>(&f_as_ui3));
  EXPECT_EQ(resf64, -2.25);
}

TEST(num_parse_test, serial_unsigned) {
  auto bytes = serial<uint32_t, 4, little_endian_tag>("666");
  EXPECT_EQ(bytes, decltype(bytes)({0x9a, 0x02, 0x0, 0x0}));

  bytes = serial<uint32_t, 4, big_endian_tag>("666");
  EXPECT_EQ(bytes, decltype(bytes)({0x0, 0x0, 0x02, 0x9a}));
}

TEST(num_parse_test, serial_signed) {
  auto bytes = serial<int32_t, 4, little_endian_tag>("4");
  EXPECT_EQ(bytes, decltype(bytes)({0x04, 0x0, 0x0, 0x0}));

  bytes = serial<int32_t, 4, little_endian_tag>("-4");
  EXPECT_EQ(bytes, decltype(bytes)({0xfc, 0xff, 0xff, 0xff}));
}

TEST(num_parse_test, serial_float) {
  auto bytes8 = serial<double, 8, little_endian_tag>("1.5");
  EXPECT_EQ(bytes8,
            decltype(bytes8)({0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf8, 0x3f}));

  auto bytes4 = serial<float, 4, little_endian_tag>("-1.25");
  EXPECT_EQ(bytes4, decltype(bytes4)({0x0, 0x0, 0xa0, 0xbf}));
}

TEST(num_parse_test, serial_parse_unsigned) {
  auto bytes = serial<uint32_t, 4, little_endian_tag>("666");
  auto res = parse<uint32_t, 4, little_endian_tag>(&bytes[0]);

  EXPECT_EQ(res, 666);

  bytes = serial<uint32_t, 4, big_endian_tag>("999");
  res = parse<uint32_t, 4, big_endian_tag>(&bytes[0]);

  EXPECT_EQ(res, 999);
}

TEST(num_parse_test, serial_parse_signed) {
  auto bytes = serial<int64_t, 8, big_endian_tag>("999");
  auto res = parse<int64_t, 8, big_endian_tag>(&bytes[0]);

  EXPECT_EQ(res, 999);

  bytes = serial<int64_t, 8, big_endian_tag>("-999");
  res = parse<int64_t, 8, big_endian_tag>(&bytes[0]);

  EXPECT_EQ(res, -999);
}

TEST(num_parse_test, serial_parse_floating) {
  auto bytes = serial<double, 8, big_endian_tag>("2.5");
  auto res = parse<double, 8, big_endian_tag>(&bytes[0]);

  EXPECT_EQ(res, 2.5);

  bytes = serial<double, 8, big_endian_tag>("-2.5");
  res = parse<double, 8, big_endian_tag>(&bytes[0]);

  EXPECT_EQ(res, -2.5);
}

int num_parse_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "num_parse_test*";

  return RUN_ALL_TESTS();
}