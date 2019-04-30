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

#include "mvm/except.h"
#include "mvm/trace.h"

#include <cstdint>
#include <limits>
#include <vector>

namespace mvm {

///
/// @brief base class used by instruction sets to update instruction pointer
///
class ip {
protected:
  uintptr_t m_val{0};
  uintptr_t m_base{0};

  void to_base() { m_val = m_base; }

public:
  ip() = default;
  virtual ~ip() {}
  ip &operator=(ip const &) = delete;
  ip(ip const &) = delete;

  template <typename T,
            typename std::enable_if_t<!std::is_base_of_v<T, ip>> * = nullptr>
  ip &operator=(T v) {
    this->to_base();
    this->increment(v);
    return *this;
  }

  ip &operator++() {
    this->increment(1);
    return *this;
  }

  template <typename T> ip &operator+=(T inc) {
    this->increment(inc);
    return *this;
  }

  template <typename T> ip &operator-=(T inc) {
    this->decrement(inc);
    return *this;
  }

  friend bool operator<(ip const &lhs, ip const &rhs) {
    return lhs.m_val < rhs.m_val;
  }

private:
  void increment(uintptr_t inc) {
    if (m_val > (std::numeric_limits<uintptr_t>::max() - inc)) {
      throw mexcept{"[-][mvm] bytecode overflow", status_type::CODE_OVERFLOW};
    }

    m_val += inc;
    LOG_INFO("ip -> update ip, (base, val) = (" << m_base << "," << m_val
                                                << ")");
  }

  void decrement(uintptr_t dec) {
    if (m_val < dec) {
      throw mexcept{"[-][mvm] bytecode overflow", status_type::CODE_OVERFLOW};
    }

    m_val -= dec;
    LOG_INFO("ip -> update ip, (base, val) = (" << m_base << "," << m_val
                                                << ")");
  }
};

inline bool operator>(ip const &lhs, ip const &rhs) { return rhs < lhs; }

inline bool operator<=(ip const &lhs, ip const &rhs) { return !(lhs > rhs); }

inline bool operator>=(ip const &lhs, ip const &rhs) { return !(rhs > lhs); }

///
/// @brief enhanced ip class used by vm
/// @warning should not be used in instruction set definition
///
class rebasable_ip final : public ip {
  uintptr_t m_max{0};

public:
  rebasable_ip() = default;

  void rebase(uintptr_t base, uintptr_t max) {
    m_max = max;
    m_base = base;
    this->to_base();
  }

  void rebase(uint8_t const *base, uint8_t const *max) {
    this->rebase(reinterpret_cast<uintptr_t>(base),
                 reinterpret_cast<uintptr_t>(max));
  }

  bool assert_in_chunk() const { return m_val >= m_base && m_val <= m_max; }

  uint8_t operator*() const {
    return *(reinterpret_cast<uint8_t const *>(m_val));
  }

  operator uint8_t *() const { return reinterpret_cast<uint8_t *>(m_val); }

  operator uintptr_t() const { return m_val; }
};

///
/// @brief Program data
///
struct prog_chunk {
  prog_chunk() = default;
  explicit prog_chunk(std::vector<uint8_t> &&c) : code{std::move(c)} {};
  std::vector<uint8_t> code;
};
} // namespace mvm
