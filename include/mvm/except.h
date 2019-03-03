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

#include <exception>
#include <optional>
#include <string>
#include <type_traits>

#include "mvm/status.h"

namespace mvm {

///
/// @brief Internal exception class
///
class mexcept : virtual public std::exception {
public:
  mexcept(std::string const &str, status_type s)
      : std::exception{}, m_status{s} {}

  status_type status() const noexcept { return m_status; }

private:
  status_type m_status;
};

namespace detail {
template <typename Callable, typename R = std::invoke_result_t<Callable>>
struct translate_imp {
  std::tuple<status_type, std::optional<R>> operator()(Callable &&f) {
    try {
      return std::make_tuple(status_type::SUCCESS, std::forward<Callable>(f)());
    } catch (const mexcept &ex) {
      return std::make_tuple(ex.status(), std::nullopt);
    } catch (...) {
      return std::make_tuple(status_type::INTERNAL_ERROR, std::nullopt);
    }
  }
};

template <typename Callable> struct translate_imp<Callable, void> {
  auto operator()(Callable &&f) {
    try {
      std::forward<Callable>(f)();
      return status_type::SUCCESS;
    } catch (const mexcept &ex) {
      return ex.status();
    } catch (...) {
      return status_type::INTERNAL_ERROR;
    }
  }
};
} // namespace detail

///
/// @brief Exception conversion utilities
///
template <typename Callable> auto translate(Callable &&f) {
  return detail::translate_imp<Callable>()(std::forward<Callable>(f));
}

} // namespace mvm