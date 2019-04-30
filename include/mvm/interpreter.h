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

#pragma once

#include "mvm/concept.h"
#include "mvm/except.h"
#include "mvm/instr_set.h"
#include "mvm/macros.h"
#include "mvm/meta.h"
#include "mvm/program.h"
#include "mvm/trace.h"
#include "mvm/traits.h"

#include <tuple>

namespace mvm {

///
/// @brief Basic interpreter
///
/// The implementation is not focused on performance but
/// on educational purpose.
///
template <typename Set, typename InstanceList> class interpreter {
  using instr_set_type = Set;
  using instr_set_traits_type =
      typename traits::instr_set_traits<instr_set_type>;
  using instr_set_desc_type =
      typename instr_set_traits_type::instr_set_desc_type;
  using instance_list_type = InstanceList;
  using instances_container_type = make_instances_t<instance_list_type>;

  instr_set_type &m_iset;
  instances_container_type m_instances;
  rebasable_ip m_rebased_ip;

public:
  explicit interpreter(instr_set_type &iset) : m_iset{iset} {}

  ///
  /// @brief Interpret code chunk
  ///
  void interpret(prog_chunk const &c);

private:
  // run interpreter loop
  void run();

  // interpret single instruction
  template <typename I> void interpret_instr();

  // produce data to producer instance
  template <typename IS, typename V, typename T> void produce(T &&arg);

  // helper function for tuple parameter
  template <typename IS, typename T, std::size_t... Is>
  void produce_unroll(T &&arg, std::index_sequence<Is...>);

  // consume data from consumer instances
  template <typename I> auto consume() {
    return consume_all<I, typename I::consumers_type>();
  }

  // consume all consumers
  // @note this is used in sync with consume_one to
  //       to retrieve all data from all consumers
  //       and forward them in a flatten manner to the specific
  //       instruction cb function
  //
  // FIXME: This back and forth implementation works but is
  //        very inefficient
  template <typename I, typename Consumers, typename... Args>
  auto consume_all(Args &&... args) {
    if constexpr (list::is_empty_v<Consumers>) {
      // all consumers have been consumed, we can call
      // the instruction callback with all its args ready
      return this->apply<I>(std::forward<Args>(args)...);
    } else {
      // consume consumers one after one
      // @note we pass the rest of the consumers list
      //       as consume_one we call consume_all with the
      //       rest of the list
      return this->consume_one<
          I, typename traits::consumers_traits<Consumers>::consumers_minus_one,
          typename traits::consumers_traits<Consumers>::front_consumer_type,
          typename traits::consumers_traits<
              Consumers>::front_consumer_data_type>(
          std::forward<Args>(args)...);
    }
  }

  // consume all data list from a single consumer
  template <typename I, typename Consumers, typename Consumer,
            typename DataList, typename... Args>
  auto consume_one(Args &&... args) {
    using instance_type = instance_of_tie_t<instance_list_type, Consumer>;

    if constexpr (list::is_empty_v<DataList>) {
      // all data has been consumed
      // call consume all back
      return this->consume_all<I, Consumers>(std::forward<Args>(args)...);
    } else if constexpr (concept ::is_meta_bytecode_v<
                             Consumer::template meta_type>) {
      // bytecode parsing specific case
      return consume_one<I, Consumers, Consumer, list::pop_front_t<DataList>>(
          this->consume_bytecode<instance_type, list::front_t<DataList>>(),
          std::forward<Args>(args)...);
    } else if constexpr (concept ::is_iterable_consumer_v<Consumer>) {
      using counter_type = typename Consumer::counter_type;
      auto code = this->consume_bytecode<
          instance_of_tie_t<instance_list_type,
                            typename counter_type::meta_type>,
          typename counter_type::type>();

      using current_data_type = std::decay_t<list::front_t<DataList>>;

      current_data_type iter;
      for (std::size_t i = 0; i < code; ++i) {
        iter.push_back(
            std::get<instance_type>(m_instances)
                .template pop<typename current_data_type::value_type>());
      }

      return consume_one<I, Consumers, Consumer, list::pop_front_t<DataList>>(
          std::move(iter), std::forward<Args>(args)...);

    } else {
      return consume_one<I, Consumers, Consumer, list::pop_front_t<DataList>>(
          std::get<instance_type>(m_instances)
              .template pop<list::front_t<DataList>>(),
          std::forward<Args>(args)...);
    }
  }

  // parse bytecode
  template <typename IS, typename DataType> auto consume_bytecode() {
    uint8_t *ip = m_rebased_ip;
    m_rebased_ip += instr_set_traits_type::template type_size<DataType>;

    if (!m_rebased_ip.assert_in_chunk()) {
      throw mexcept("[-][mvm] bytecode overflow", status_type::CODE_OVERFLOW);
    }

    return std::get<IS>(m_instances)
        .template parse<
            DataType, instr_set_type::template code_value_repr<DataType>::size,
            typename instr_set_type::template code_value_repr<
                DataType>::endian_type>(ip + 1);
  }

  // Call instruction apply function
  template <typename I, typename... Args> auto apply(Args &&... args) {
    if constexpr (concept ::is_ip_udpater_v<I>) {
      return I::apply(m_iset, m_rebased_ip, std::forward<Args>(args)...);
    } else {
      ++m_rebased_ip;
      return I::apply(m_iset, std::forward<Args>(args)...);
    }
  }
};

///////////////////////////////////////////////////////////////
// Implementation
///////////////////////////////////////////////////////////////

// impl
template <typename Set, typename InstancesList>
void interpreter<Set, InstancesList>::interpret(prog_chunk const &chunk) {
  m_rebased_ip.rebase(&(chunk.code[0]),
                      &(chunk.code[0]) + chunk.code.size() - 1);

  this->run();
}

template <typename Set, typename InstancesList>
void interpreter<Set, InstancesList>::run() {
  while (m_rebased_ip.assert_in_chunk()) {
    LOG_INFO("interpreter -> process instruction opcode "
             << static_cast<int>(*m_rebased_ip));
    instr_set_visitor<instr_set_desc_type>()(*m_rebased_ip, [this](auto &&arg) {
      using instr_type = std::decay_t<decltype(arg)>;
      LOG_INFO("interpreter -> process instruction "
               << typestring::char_seq<typename instr_type::name_type>::value);
      this->interpret_instr<instr_type>();
    });
  }
}

template <typename Set, typename InstancesList>
template <typename I>
void interpreter<Set, InstancesList>::interpret_instr() {
  if constexpr (concept ::is_producer_v<I>) {
    this->produce<
        instance_of_tie_t<instance_list_type,
                          typename traits::producers_traits<I>::producer_type>,
        typename traits::producers_traits<I>::data_type>(this->consume<I>());
  } else {
    this->consume<I>();
  }
}

template <typename Set, typename InstancesList>
template <typename IS, typename V, typename T>
void interpreter<Set, InstancesList>::produce(T &&arg) {
  if constexpr (concept ::is_tuple_v<std::decay_t<T>>) {
    this->produce_unroll<IS>(
        std::forward<T>(arg),
        std::make_index_sequence<std::tuple_size<T>::value>());
  } else if constexpr (concept ::is_container_valid_v<std::decay_t<T>>) {
    for (auto sub : std::forward<T>(arg)) {
      std::get<IS>(m_instances)
          .template push<typename V::value_type>(std::move(sub));
    }
  } else {
    std::get<IS>(m_instances).template push<V>(std::forward<T>(arg));
  }
}

template <typename Set, typename InstancesList>
template <typename IS, typename T, std::size_t... Is>
void interpreter<Set, InstancesList>::produce_unroll(
    T &&arg, std::index_sequence<Is...>) {

  (std::get<IS>(m_instances)
       .template push<typename std::tuple_element<Is, std::decay_t<T>>::type>(
           std::get<Is>(std::forward<T>(arg))),
   ...);
}

} // namespace mvm