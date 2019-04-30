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

#include "mvm/helpers/list.h"

#include "gtest/gtest.h"

#include <tuple>
#include <type_traits>

using namespace mvm::list;

namespace {
template <typename T> using map_tuple = std::tuple<T>;

template <typename T> using int_pred = std::is_same<T, int>;
} // namespace

TEST(list_test, main) {
  // is empty
  static_assert(is_empty_v<mplist<>>, "[-][list_test] is_empty failed");
  static_assert(is_empty_v<std::tuple<>>, "[-][list_test] is_empty failed");
  static_assert(!is_empty_v<mplist<int>>, "[-][list_test] is_empty failed");

  // size
  static_assert(0 == size_v<mplist<>>, "[-][list_test] size failed");
  static_assert(1 == size_v<mplist<int>>, "[-][list_test] size failed");
  static_assert(2 == size_v<mplist<int, double>>, "[-][list_test] size failed");

  // front
  static_assert(std::is_same_v<int, front_t<mplist<int, double>>>,
                "[-][list_test] front failed");
  static_assert(std::is_same_v<int, front_t<std::tuple<int>>>,
                "[-][list_testtest] front failed");

  // pop front
  static_assert(
      std::is_same_v<pop_front_t<mplist<int, double>>, mplist<double>>,
      "[-][list_test] popfront failed");
  static_assert(std::is_same_v<pop_front_t<mplist<int>>, mplist<>>,
                "[-][list_test] popfront failed");

  // push front
  static_assert(
      std::is_same_v<push_front_t<int, mplist<double>>, mplist<int, double>>,
      "[-][list_test] pushfront failed");
  static_assert(std::is_same_v<push_front_t<int, mplist<>>, mplist<int>>,
                "[-][list_test] pushfront failed");

  // push back
  static_assert(std::is_same_v<push_back_t<int, mplist<>>, mplist<int>>,
                "[-][list_test] pushback failed");
  static_assert(
      std::is_same_v<push_back_t<double, mplist<int>>, mplist<int, double>>,
      "[-][list_test] pushback failed");

  // pop back
  static_assert(std::is_same_v<pop_back_t<mplist<int, double>>, mplist<int>>,
                "[-][list_test] popback failed");
  static_assert(std::is_same_v<pop_back_t<mplist<int>>, mplist<>>,
                "[-][list_test] popback failed");

  // has
  static_assert(has_v<int, mplist<double, int>>, "[-][list_test] has failed");
  static_assert(has_v<int, mplist<int, double>>, "[-][list_test] has failed");
  static_assert(!has_v<int, mplist<float, double>>,
                "[-][list_test] has failed");

  // at
  static_assert(std::is_same_v<at_t<0, mplist<int, double>>, int>,
                "[-][list_test] at failed");
  static_assert(std::is_same_v<at_t<1, mplist<int, double>>, double>,
                "[-][list_test] at failed");

  // fill
  static_assert(std::is_same_v<fill_t<3, double, mplist<int>>,
                               mplist<int, double, double>>,
                "[-][list_test] fill failed");

  // remove dup
  static_assert(std::is_same_v<remove_dup_t<mplist<int, int>>, mplist<int>>,
                "[-][list_test] remove_dup failed");
  static_assert(
      std::is_same_v<remove_dup_t<mplist<int, double>>, mplist<int, double>>,
      "[-][list_test] remove_dup failed");

  // check predicate
  static_assert(check_v<std::is_integral, mplist<int, int>>,
                "[-][list_test] check pred failed");
  static_assert(!check_v<std::is_integral, mplist<int, double>>,
                "[-][list_test] check pred failed");

  // map
  static_assert(std::is_same_v<mplist<std::tuple<int>, std::tuple<double>>,
                               map_t<map_tuple, mplist<int, double>>>,
                "[-][list_test] map failed");

  // concat
  static_assert(std::is_same_v<mplist<int, double, int>,
                               concat_t<mplist<int>, mplist<int, double>>>,
                "[-][list_test] concat failed");

  // concate all
  static_assert(std::is_same_v<mplist<float, int, double, int>,
                               concat_all_t<mplist<int>, mplist<int, double>,
                                            mplist<>, mplist<float>>>,
                "[-][list_test] concat_all_t failed");

  // merge
  static_assert(std::is_same_v<mplist<double, int>,
                               merge_t<mplist<int>, mplist<int, double>>>,
                "[-][list_test] merge failed");

  // filter
  static_assert(
      std::is_same_v<mplist<int, int>,
                     filter_t<int_pred, true, mplist<int, double, int>>>,
      "[-][list_test] filter failed");
  static_assert(
      std::is_same_v<mplist<double>,
                     filter_t<int_pred, false, mplist<int, double, int>>>,
      "[-][list_test] filter failed");

  // rebind
  static_assert(std::is_same_v<rebind_t<std::tuple, mplist<int, int>>,
                               std::tuple<int, int>>,
                "[-][list_test] rebind failed");

  // no runtime test
  EXPECT_TRUE(true);
}

int list_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "list_test*";

  return RUN_ALL_TESTS();
}