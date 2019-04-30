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

#include <cstddef>
#include <type_traits>

namespace mvm::typestring {
namespace details {
template <std::size_t N> constexpr char get_c(const char *const str, int n) {
  return (n < N) ? str[n] : '\0';
}

// append
template <typename TS, char C> struct append;

template <template <char...> typename TS, char C, char... Chars>
struct append<TS<Chars...>, C> {
  using type = TS<Chars..., C>;
};

// reverse
template <typename TS> struct reverse;

template <template <char...> typename TS, char Head, char... Tail>
struct reverse<TS<Head, Tail...>> {
  using type = typename append<typename reverse<TS<Tail...>>::type, Head>::type;
};

template <template <char...> typename TS> struct reverse<TS<>> {
  using type = TS<>;
};

// strip a reversed string
template <typename TS> struct strip_reversed;

template <template <char...> typename TS, char Head, char... Tail>
struct strip_reversed<TS<Head, Tail...>> {
  using type = std::conditional_t<Head != '\0', TS<Head, Tail...>,
                                  typename strip_reversed<TS<Tail...>>::type>;
};

template <template <char...> typename TS> struct strip_reversed<TS<>> {
  using type = TS<>;
};

// full strip pipeline
template <typename TS>
using strip_t = typename append<typename reverse<typename strip_reversed<
                                    typename reverse<TS>::type>::type>::type,
                                '\0'>::type;
} // namespace details

template <char...> struct type_string {};

template <typename TString> struct char_seq;

template <template <char...> typename TTString, char... Chars>
struct char_seq<TTString<Chars...>> {
  static constexpr char value[sizeof...(Chars)] = {Chars...};
};
}; // namespace mvm::typestring

#define MVM_GET_CHAR(x, n) mvm::typestring::details::get_c<sizeof(x)>(x, n)

#define MVM_TSTRING_16(x)                                                      \
  MVM_GET_CHAR(x, 0), MVM_GET_CHAR(x, 1), MVM_GET_CHAR(x, 2),                  \
      MVM_GET_CHAR(x, 3), MVM_GET_CHAR(x, 4), MVM_GET_CHAR(x, 5),              \
      MVM_GET_CHAR(x, 6), MVM_GET_CHAR(x, 7), MVM_GET_CHAR(x, 8),              \
      MVM_GET_CHAR(x, 9), MVM_GET_CHAR(x, 10), MVM_GET_CHAR(x, 11),            \
      MVM_GET_CHAR(x, 12), MVM_GET_CHAR(x, 13), MVM_GET_CHAR(x, 14),           \
      MVM_GET_CHAR(x, 15)

#define MVM_TSTRING_32(x)                                                      \
  MVM_TSTRING_16(x), MVM_GET_CHAR(x, 16), MVM_GET_CHAR(x, 17),                 \
      MVM_GET_CHAR(x, 18), MVM_GET_CHAR(x, 19), MVM_GET_CHAR(x, 20),           \
      MVM_GET_CHAR(x, 21), MVM_GET_CHAR(x, 22), MVM_GET_CHAR(x, 23),           \
      MVM_GET_CHAR(x, 24), MVM_GET_CHAR(x, 25), MVM_GET_CHAR(x, 26),           \
      MVM_GET_CHAR(x, 27), MVM_GET_CHAR(x, 28), MVM_GET_CHAR(x, 29),           \
      MVM_GET_CHAR(x, 30), MVM_GET_CHAR(x, 31)

#define MVM_TSTRING(x)                                                         \
  mvm::typestring::details::strip_t<                                           \
      mvm::typestring::type_string<MVM_TSTRING_32(x)>>
