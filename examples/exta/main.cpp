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
#include <stack>
#include <string>

using namespace mvm;

namespace {
// We decide that there will be a stack for ui32 and
// a stack for double. We will use mvm default stack
// meta concept for the ui32 stack and we create a new
// meta_double_stack concept for double handling
//
// For now, you can only add stack concepts
//
template <typename T> struct meta_double_stack {};

// Instance for the concept (we could use the default value stack)
template <typename T> class double_value_stack {
  using value_type = T;
  std::stack<T> m_stack;

public:
  template <typename U> void push(U val) {
    LOG_INFO("double_value_stack -> push " << val << " on stack[" << this
                                           << "]");
    m_stack.push(val);
  }

  template <typename U> U pop() {
    auto val = m_stack.top();
    m_stack.pop();
    LOG_INFO("double_value_stack -> pop from stack[" << this << "]");
    return val;
  }
};

///
/// @brief This is an example of fake instruction
///        set that extends default mvm concept
///
struct exta_set : instr_set<exta_set> {
  // First define the instructions callback
  ui32 add(ui32 a, ui32 b) { return a + b; }

  double fadd(double a, double b) { return a + b; }

  void write(ui32 val) { std::cout << "ui32 result: " << val << std::endl; }

  void fwrite(double val) {
    std::cout << "double result: " << val << std::endl;
  }

  // define the default endian type
  // if you need a specific value representation for a
  // type, specialize code_value_repr in your class
  using endian_type = num::little_endian_tag;

  // instr set description
  using me = exta_set;
  using instr_table = instr_set_desc<
      consumer_producer_pipe<consumer<meta_bytecode, ui32>,
                             producer<meta_value_stack, ui32>,
                             MVM_TSTRING("push")>,
      consumer_producer_pipe<consumer<meta_bytecode, double>,
                             producer<meta_double_stack, double>,
                             MVM_TSTRING("fpush")>,
      consumer_producer_instr<consumer<meta_value_stack, ui32, ui32>,
                              producer<meta_value_stack, ui32>, false, &me::add,
                              MVM_TSTRING("add")>,
      consumer_producer_instr<consumer<meta_double_stack, double, double>,
                              producer<meta_double_stack, double>, false,
                              &me::fadd, MVM_TSTRING("fadd")>,
      consumer_instr<consumer<meta_value_stack, ui32>, false, &me::write,
                     MVM_TSTRING("write")>,
      consumer_instr<consumer<meta_double_stack, double>, false, &me::fwrite,
                     MVM_TSTRING("fwrite")>>;
};
} // namespace

int main(int argc, char *argv[]) {
  std::cout << "------ running exta example ------\n" << std::endl;

  // explicit definition of the instances list:
  // - each meta concept must be instantiated
  // - meta_bytecode concept is mandatory
  // You can provide your own implementation but they must respect the same
  // interface as value_stack (for stack like concept) and bytecode_serializer
  using extended_instances_list =
      list::mplist<meta_bytecode<bytecode_serializer>,
                   meta_value_stack<value_stack<list::mplist<ui32>>>,
                   meta_double_stack<double_value_stack<double>>>;
  using vm_type = vm<exta_set, extended_instances_list>;
  exta_set iset;
  vm_type vm(iset);

  std::stringstream prog;
  prog << "push 1\npush 4\nadd\nwrite\nfpush 2.0\nfpush 3.0\nfadd\nfwrite\n";
  auto res = vm.assemble(prog);
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