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

#include "mvm/helpers/num_parse.h"

namespace mvm {

///
/// @brief default implementation of bytecode serializer
///
struct bytecode_serializer {
  template <typename T, std::size_t N, typename Endian>
  auto serial(std::string const &str) const {
    return num::serial<T, N, Endian>(str);
  }

  template <typename T, std::size_t N, typename Endian>
  auto parse(uint8_t const *ip) const {
    return num::parse<T, N, Endian>(ip);
  }
};
} // namespace mvm