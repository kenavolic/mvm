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

#include <sstream>

#include "mvm/base_vm.h"
#include "mvm/program_chunk.h"
#include "test_common.h"

using namespace mvm;
using namespace mvm::test;

namespace {
class base_vm_test : public ::testing::Test {
protected:
  template <typename VM>
  void check_assembler_ok(VM &vm, std::string const &str,
                          std::vector<uint8_t> &&exp_bytes) {
    std::istringstream sstr(str);
    auto res = vm.assemble(sstr);
    EXPECT_EQ(std::get<0>(res), status_type::SUCCESS);
    EXPECT_TRUE(std::get<1>(res));
    EXPECT_EQ(std::get<1>(res).value().code, std::move(exp_bytes));
  }

  template <typename VM>
  void check_assembler_nok(VM &vm, std::string const &str,
                           status_type expected) {
    std::istringstream sstr(str);
    auto res = vm.assemble(sstr);
    EXPECT_EQ(std::get<0>(res), expected);
    EXPECT_FALSE(std::get<1>(res));
  }

  template <typename VM>
  void check_diassembler_nok(VM &vm, prog_chunk &&c, status_type expected) {
    auto res = vm.disassemble(std::move(c));
    EXPECT_EQ(std::get<0>(res), expected);
    EXPECT_FALSE(std::get<1>(res));
  }

  using vm1_type = base_vm<test_instr_set>;
  using vm2_type = base_vm<test_instr_set2>;
  test_instr_set iset1;
  vm1_type vm1{iset1};
  test_instr_set2 iset2;
  vm2_type vm2{iset2};
};
} // namespace

TEST_F(base_vm_test, assemble_single_instr) {
  check_assembler_ok(vm1, "a 1 2",
                     {{0x0, 0x1, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0}});
  check_assembler_ok(vm1, "b", {0x1});

  check_assembler_ok(vm2, "pushd 2.0",
                     {{0x1, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}});
}

TEST_F(base_vm_test, assemble_prog) {
  check_assembler_ok(
      vm1, "a 1 2\nb\nc 256\nd 1024\ne\nf\ng 1",
      {{0x0, 0x1, 0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x1, 0x2, 0x0, 0x1, 0x0,
        0x0, 0x3, 0x0, 0x4, 0x0, 0x0, 0x4, 0x5, 0x6, 0x1, 0x0, 0x0, 0x0}});
}

TEST_F(base_vm_test, assemble_bad) {
  check_assembler_nok(vm1, "bad", status_type::BAD_INSTR_NAME);
  check_assembler_nok(vm1, "a 1 2 3", status_type::BAD_INSTR_OPERAND);
  check_assembler_nok(vm1, "a 1", status_type::BAD_INSTR_OPERAND);
  check_assembler_nok(vm1, "a", status_type::BAD_INSTR_OPERAND);
  check_assembler_nok(vm1, "b 1", status_type::BAD_INSTR_OPERAND);
}

TEST_F(base_vm_test, disassemble_prog) {
  std::string prog = "a 1 2\nb\nc 256\nd 1024\ne\nf\ng 1";
  std::istringstream sstr(prog);

  auto res = vm1.disassemble(std::get<1>(vm1.assemble(sstr)).value());
  EXPECT_EQ(std::get<0>(res), status_type::SUCCESS);
  EXPECT_TRUE(std::get<1>(res));
  EXPECT_EQ(std::get<1>(res).value(), prog + "\n");
}

TEST_F(base_vm_test, disassemble_bad) {
  check_diassembler_nok(vm1, prog_chunk({0x8}),
                        status_type::INVALID_INSTR_OPCODE);
  check_diassembler_nok(vm1, prog_chunk({0x0, 0x1}),
                        status_type::CODE_OVERFLOW);
}

TEST_F(base_vm_test, interpret_prog) {
  // e, g 1, nop, a 1 2, f;
  EXPECT_EQ(
      vm1.interpret(prog_chunk({0x4, 0x6, 0x1, 0x0, 0x0, 0x0, 0x90, 0x0, 0x1,
                                0x0, 0x0, 0x0, 0x2, 0x0, 0x0, 0x0, 0x5})),
      status_type::SUCCESS);

  std::vector<int> exp = {4, 6, 0, 5};
  EXPECT_EQ(iset1.call_stack, exp);
  EXPECT_EQ(iset1.res, 2);
}

TEST_F(base_vm_test, interpret_bad) {
  EXPECT_EQ(vm1.interpret(prog_chunk({0x0, 0x0})), status_type::CODE_OVERFLOW);
  EXPECT_EQ(vm1.interpret(prog_chunk({0x9, 0x0})),
            status_type::INVALID_INSTR_OPCODE);
  EXPECT_EQ(vm1.interpret(prog_chunk({0x1, 0x0})),
            status_type::POP_EMPTY_STACK);
}

int base_vm_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "base_vm_test*";

  return RUN_ALL_TESTS();
}