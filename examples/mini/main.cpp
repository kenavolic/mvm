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

#include "mvm/mvm.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace mvm;

namespace {
///
/// @brief This is an example of minimal instruction
///        set using mvm default meta concepts and instances
///
struct mini_set : instr_set<mini_set> {
  // First define the instructions callback

  // push (implicitly defined in instruction table below)
  // pop from code and push to stack = pipe
  /*ui32 push(ui32 val)
  {
      return val;
  }*/

  // pop (implicitly defined in instruction table below)
  // pop from code
  /*void pop(ui32 val)
  {
      // skip
  }*/

  //
  // @brief read and push to stack
  //
  ui32 read() {
    std::cout << "enter value:" << std::endl;
    std::string val;
    std::cin >> val;
    return std::stoul(val);
  }

  //
  // @brief pop from stack and write
  //
  void write(ui32 val) { std::cout << "result: " << val << std::endl; }

  //
  // @brief duplicate value on top of stack
  //
  std::tuple<ui32, ui32> dup(ui32 val) { return std::make_tuple(val, val); }

  //
  // @brief pop 2 values from stack and push 0/1 if equal or not
  //
  ui32 eq(ui32 val1, ui32 val2) { return (val1 != val2) ? 1 : 0; }

  //
  // @brief jump if zero on stack
  //
  // This kind of instruction can modify instruction pointer
  //
  void jz(ip &eip, ui32 new_ip, ui32 val) {
    if (val == 0) {
      eip = new_ip;
    } else {
      ++eip;
    }
  }

  //
  // @brief unconditional jump
  //
  // This kind of instruction can modify instruction pointer
  //
  void jump(ip &eip, ui32 val) { eip = val; }

  //
  // @brief arithmetic instruction
  //
  ui32 add(ui32 a, ui32 b) { return a + b; }

  //
  // @brief arithmetic instruction
  //
  ui32 sub(ui32 a, ui32 b) { return a - b; }

  //
  // @brief arithmetic instruction
  //
  ui32 mul(ui32 a, ui32 b) { return a * b; }

  //
  // @brief swap top elements of stack
  //
  std::tuple<ui32, ui32> swap(ui32 a, ui32 b) { return std::make_tuple(b, a); }

  //
  // @brief pop n elements from the stack, rotates them,
  //        and push them to the stack
  //
  std::vector<ui32> rotln(std::vector<ui32> &&vec) {
    std::rotate(vec.begin(), vec.begin() + 1, vec.end());
    return vec;
  }

  // define the default endian type
  // if you need a specific value representation for a
  // type, specialize code_value_repr in your class
  using endian_type = num::little_endian_tag;

  // instr set description:
  // - meta_value_stack is a meta concept representing a stack
  // - meta_bytecode is a meta concept representing the bytecode (mandatory
  // concept) each meta concept must be instantiated with a concrete class
  // - value_stack is the default stack implementation
  // - bytecode_serializer (bytecode_serializer.h) is the default bytecode
  // serializer
  //
  // You can provide your own implementation but they must respect the same
  // interface as value_stack and bytecode_serializer
  //
  using me = mini_set;
  using instr_table = instr_set_desc<
      // push implicitly defined with a pipe
      consumer_producer_pipe<consumer<meta_bytecode, ui32>,
                             producer<meta_value_stack, ui32>,
                             MVM_TSTRING("push")>,
      // pop implicitly defined with a pipe
      consumer_pipe<consumer<meta_value_stack, ui32>, MVM_TSTRING("pop")>,
      // read is a producer instruction, that produces on the stack
      producer_instr<producer<meta_value_stack, ui32>, false, &me::read,
                     MVM_TSTRING("read")>,
      // write is a consumer instruction, that consumes from the stack
      consumer_instr<consumer<meta_value_stack, ui32>, false, &me::write,
                     MVM_TSTRING("write")>,
      // dup is a consumer producer instructions, that consumes 1 and produces 2
      consumer_producer_instr<consumer<meta_value_stack, ui32>,
                              producer<meta_value_stack, ui32, ui32>, false,
                              &me::dup, MVM_TSTRING("dup")>,
      // eq is a consumer producer instructions, that consumes 2 and produces
      // one
      consumer_producer_instr<consumer<meta_value_stack, ui32, ui32>,
                              producer<meta_value_stack, ui32>, false, &me::eq,
                              MVM_TSTRING("eq")>,
      // jz is a multiple consumer instruction that consumes from the code and
      // stack, and modifies ip consumers are defined from right to left
      consumers_instr<consumers<consumer<meta_value_stack, ui32>,
                                consumer<meta_bytecode, ui32>>,
                      true, &me::jz, MVM_TSTRING("jz")>,
      // jump is a consumer instruction that consumes from the code and modifies
      // ip
      consumer_instr<consumer<meta_bytecode, ui32>, true, &me::jump,
                     MVM_TSTRING("jump")>,
      // add, sub, mul are consumer producer instructions
      consumer_producer_instr<consumer<meta_value_stack, ui32, ui32>,
                              producer<meta_value_stack, ui32>, false, &me::add,
                              MVM_TSTRING("add")>,
      consumer_producer_instr<consumer<meta_value_stack, ui32, ui32>,
                              producer<meta_value_stack, ui32>, false, &me::sub,
                              MVM_TSTRING("sub")>,
      consumer_producer_instr<consumer<meta_value_stack, ui32, ui32>,
                              producer<meta_value_stack, ui32>, false, &me::mul,
                              MVM_TSTRING("mul")>,
      // swap is a consumer producer instruction, that consumes 2 and produces 2
      consumer_producer_instr<consumer<meta_value_stack, ui32, ui32>,
                              producer<meta_value_stack, ui32, ui32>, false,
                              &me::swap, MVM_TSTRING("swap")>,
      // rotln consumes 1 element for the code to count the number of elements
      // it consumes from the stack and produces n elements to the stack
      consumer_producer_instr<
          iterable_consumer<meta_value_stack, std::vector<ui32> &&,
                            count_from<consumer<meta_bytecode, ui32>>>,
          producer<meta_value_stack, std::vector<ui32>>, false, &me::rotln,
          MVM_TSTRING("rotln")>>;
};
} // namespace

int main(int argc, char *argv[]) {
  std::cout << "------ running mini example ------\n" << std::endl;

  if (argc != 2) {
    std::cerr << "invalid usage: " << argv[0] << " filename" << std::endl;
    return 1;
  }

  std::fstream file{argv[1]};
  if (!file) {
    std::cerr << "failed to open file " << argv[1] << std::endl;
    return 1;
  }

  // declare a vm type with default instances
  // @see exta example for a vm declaration with custom instances
  using vm_type = vm<mini_set>;
  mini_set iset;
  vm_type vm(iset);

  auto res = vm.assemble(file);
  if (std::get<0>(res) != status_type::SUCCESS || !std::get<1>(res)) {
    std::cerr << "oups, assembler failed with error code "
              << static_cast<unsigned>(std::get<0>(res)) << std::endl;
    return 1;
  }

  auto res2 = vm.interpret(std::get<1>(res).value());

  if (res2 != status_type::SUCCESS) {
    std::cerr << "oups, interpreter failed with error code "
              << static_cast<unsigned>(res2) << std::endl;
    return 1;
  }

  std::cout << "\nyou have just executed:\n" << std::endl;
  std::cout << std::get<1>(vm.disassemble(std::get<1>(res).value())).value()
            << std::endl;

  return 0;
}