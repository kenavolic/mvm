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
#include "mvm/vm.h"
#include "test_common.h"

#include "gtest/gtest.h"

#include <sstream>

using namespace mvm;
using namespace mvm::test;

namespace {
class vm_test : public ::testing::Test {
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

  using vm1_type = vm<test_instr_set>;

  using extended_instances_list =
      list::mplist<meta_bytecode<bytecode_serializer>,
                   meta_value_stack<value_stack<list::mplist<ui32>>>,
                   meta_double_stack<value_stack<list::mplist<double>>>>;
  using vm2_type = vm<test_instr_set_extra, extended_instances_list>;
  test_instr_set iset1;
  vm1_type vm1{iset1};
  test_instr_set_extra iset2;
  vm2_type vm2{iset2};

  using vm3_type = vm<test_instr_set_mixed>;
  test_instr_set_mixed iset3;
  vm3_type vm3{iset3};
};
} // namespace

TEST_F(vm_test, assemble_single_instr) {
  check_assembler_ok(vm1, "push 1", {{0x3, 0x1, 0x0, 0x0, 0x0}});
  check_assembler_ok(vm1, "dup", {0x2});

  check_assembler_ok(vm2, "kui2", {0x0});
}

TEST_F(vm_test, assemble_prog) {
  check_assembler_ok(vm1, "push 1\ndup\nzero\nrandn 2\nrotln 3\njump 0",
                     {{0x3, 0x1, 0x0, 0x0, 0x0, 0x2, 0x0, 0x4, 0x2, 0x0, 0x0,
                       0x0, 0x5, 0x3, 0x0, 0x0, 0x0, 0x1, 0x0, 0x0, 0x0, 0x0}});
}

TEST_F(vm_test, assemble_bad) {
  check_assembler_nok(vm1, "bad", status_type::BAD_INSTR_NAME);
  check_assembler_nok(vm1, "push 1 2", status_type::BAD_INSTR_OPERAND);
  check_assembler_nok(vm1, "push", status_type::BAD_INSTR_OPERAND);
  check_assembler_nok(vm1, "zero 1", status_type::BAD_INSTR_OPERAND);
}

TEST_F(vm_test, interpret_instr_default) {

  // test language using default instances and concept

  // zero
  EXPECT_EQ(vm1.interpret(prog_chunk({0x0})), status_type::SUCCESS);

  // jump of 4 and execute zero
  EXPECT_EQ(vm1.interpret(prog_chunk({0x1, 0x5, 0x0, 0x0, 0x0, 0x0})),
            status_type::SUCCESS);

  // push 0 and dup
  EXPECT_EQ(vm1.interpret(prog_chunk({0x0, 0x2})), status_type::SUCCESS);

  // pop 4 from code and push to stack
  EXPECT_EQ(vm1.interpret(prog_chunk({0x3, 0x4, 0x0, 0x0, 0x0})),
            status_type::SUCCESS);

  // pop 4 from code and push 4 elements
  EXPECT_EQ(vm1.interpret(prog_chunk({0x4, 0x4, 0x0, 0x0, 0x0})),
            status_type::SUCCESS);

  // push 0 twice and then pop all
  EXPECT_EQ(vm1.interpret(prog_chunk({0x0, 0x0, 0x5, 0x2, 0x0, 0x0, 0x0})),
            status_type::SUCCESS);

  // push 1 and 2 then add
  EXPECT_EQ(vm1.interpret(prog_chunk(
                {0x3, 0x1, 0x0, 0x0, 0x0, 0x3, 0x2, 0x0, 0x0, 0x0, 0x6})),
            status_type::SUCCESS);
}

TEST_F(vm_test, interpret_instr_extended) {

  // test language with an extra concept

  // push on unsigned stack
  EXPECT_EQ(vm2.interpret(prog_chunk({0x0})), status_type::SUCCESS);

  // push on double stack
  EXPECT_EQ(vm2.interpret(prog_chunk({0x1})), status_type::SUCCESS);

  // push ui32, push double, then add
  EXPECT_EQ(vm2.interpret(prog_chunk({0x2, 0x3, 0x4})), status_type::SUCCESS);
}

TEST_F(vm_test, interpret_instr_mixed) {

  // test language with variant stack

  // push on unsigned stack
  EXPECT_EQ(vm3.interpret(prog_chunk({0x0})), status_type::SUCCESS);

  // push on double stack
  EXPECT_EQ(vm3.interpret(prog_chunk({0x1})), status_type::SUCCESS);

  // push ui32, push double, then add
  EXPECT_EQ(vm3.interpret(prog_chunk({0x2, 0x3, 0x4})), status_type::SUCCESS);
}

TEST_F(vm_test, interpret_prog) {
  // push 1
  // dup
  // zero
  // randn 2
  // rotln 3
  // jump 0x17 (last instruction)
  EXPECT_EQ(
      vm1.interpret(prog_chunk({0x3, 0x1, 0x0,  0x0, 0x0, 0x2, 0x0,  0x4,
                                0x2, 0x0, 0x0,  0x0, 0x5, 0x3, 0x0,  0x0,
                                0x0, 0x1, 0x17, 0x0, 0x0, 0x0, 0x90, 0x0})),
      status_type::SUCCESS);

  // 3 not in the stack as it is a direct pipe instruction
  std::vector<unsigned> exp_stack = {2, 0, 4, 5, 1, 0};
  EXPECT_EQ(iset1.call_stack, exp_stack);
}

TEST_F(vm_test, interpret_bad) {
  EXPECT_EQ(vm1.interpret(prog_chunk({0x1, 0x0})), status_type::CODE_OVERFLOW);
  EXPECT_EQ(vm1.interpret(prog_chunk({0x9, 0x0})),
            status_type::INVALID_INSTR_OPCODE);
  EXPECT_EQ(vm1.interpret(prog_chunk({0x2, 0x0})),
            status_type::POP_EMPTY_STACK);
}

TEST_F(vm_test, disassemble_prog) {
  std::string prog = "push 1\ndup\nzero\nrandn 2\nrotln 3\njump 23";
  std::istringstream sstr(prog);

  auto res = vm1.disassemble(std::get<1>(vm1.assemble(sstr)).value());
  EXPECT_EQ(std::get<0>(res), status_type::SUCCESS);
  EXPECT_TRUE(std::get<1>(res));
  EXPECT_EQ(std::get<1>(res).value(), prog + "\n");
}

TEST_F(vm_test, disassemble_bad) {
  check_diassembler_nok(vm1, prog_chunk({0x8}),
                        status_type::INVALID_INSTR_OPCODE);
  check_diassembler_nok(vm1, prog_chunk({0x1, 0x1}),
                        status_type::CODE_OVERFLOW);
}

int vm_test(int argc, char *argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::FLAGS_gtest_filter = "vm_test*";

  return RUN_ALL_TESTS();
}