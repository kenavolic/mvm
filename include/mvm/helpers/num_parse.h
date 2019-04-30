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

#include <array>
#include <cmath>
#include <cstdint>
#include <string>
#include <type_traits>

namespace mvm::num {
struct little_endian_tag {};
struct big_endian_tag {};

namespace details {
// utility struct used to bytify and set endianness for input and output data
// note that internal endiannes is le
template <size_t N, typename E1, typename E2> struct to_endian {
  template <std::size_t... Is>
  constexpr auto bytify_swap(uint8_t const *bytes, std::index_sequence<Is...>) {
    return std::array<uint8_t, sizeof...(Is)>{bytes[sizeof...(Is) - Is - 1]...};
  }

  constexpr auto operator()(uint8_t const *bytes) {
    return bytify_swap(bytes, std::make_index_sequence<N>());
  }
};

template <size_t N, typename E> struct to_endian<N, E, E> {
  template <std::size_t... Is>
  constexpr auto bytify(uint8_t const *bytes, std::index_sequence<Is...>) {
    return std::array<uint8_t, sizeof...(Is)>{bytes[Is]...};
  }

  constexpr auto operator()(uint8_t const *bytes) {
    return bytify(bytes, std::make_index_sequence<N>());
  }
};

inline int base(std::string const str) {
  if (str.find_first_of("0x") == 0) {
    return 16;
  } else {
    return 10;
  }
}
} // namespace details

namespace traits {
template <std::size_t N> struct ieee754_traits;

template <> struct ieee754_traits<4> {
  using unsigned_type = uint32_t;
  static constexpr std::size_t sign_size = 1;
  static constexpr std::size_t exp_size = 8;
  static constexpr std::size_t mant_size = 23;
  static constexpr std::size_t exp_shift = 127;
  static constexpr int32_t exp_mask = 0xFF;
  static constexpr unsigned_type mant_mask = 0x7FFFFF;
};

template <> struct ieee754_traits<8> {
  using unsigned_type = uint64_t;
  static constexpr std::size_t sign_size = 1;
  static constexpr std::size_t exp_size = 11;
  static constexpr std::size_t mant_size = 52;
  static constexpr std::size_t exp_shift = 1023;
  static constexpr int32_t exp_mask = 0x7FF;
  static constexpr unsigned_type mant_mask = 0xFFFFFFFFFFFFF;
};
} // namespace traits

template <typename T, typename Array, size_t... Is>
T parse_unsigned(Array const &arr, std::index_sequence<Is...>) {
  return ((static_cast<T>(arr[Is]) << (Is * 8)) + ...);
}

template <typename T, std::size_t N>
auto parse_unsigned(std::array<uint8_t, N> const &bytes) {
  return parse_unsigned<T>(bytes, std::make_index_sequence<N>());
}

template <size_t... Is>
auto serial_unsigned(uint64_t val, std::index_sequence<Is...>) {
  return std::array<uint8_t, sizeof...(Is)>{
      (static_cast<uint8_t>(val >> (Is * 8) & 0xFF))...};
}

template <std::size_t N> auto serial_unsigned(uint64_t val) {
  return serial_unsigned(val, std::make_index_sequence<N>());
}

template <typename T, std::size_t N>
T parse_signed(std::array<uint8_t, N> const &bytes) {
  using unsigned_type = std::make_unsigned_t<T>;

  if (bytes[N - 1] & 0x80) {
    return -1 * (~(parse_unsigned<unsigned_type>(bytes) - 1));
  }

  return parse_unsigned<unsigned_type>(bytes);
}

template <typename T, std::size_t N> auto serial_signed(int64_t val) {
  T tval = static_cast<T>(val);

  if (tval >= 0) {
    return serial_unsigned<N>(tval);
  } else {
    using unsigned_type = std::make_unsigned_t<T>;
    unsigned_type uval = std::abs(tval);
    uval = ~uval + 1;

    return serial_unsigned<N>(uval);
  }
}

template <typename T, std::size_t N>
T parse_floating(std::array<uint8_t, N> const &bytes) {
  if constexpr (N == 4 || N == 8) {
    using ieee754_type = traits::ieee754_traits<N>;
    using unsigned_type = typename ieee754_type::unsigned_type;

    auto f_as_ui = parse_unsigned<unsigned_type>(bytes);
    uint16_t exp =
        (f_as_ui >> ieee754_type::mant_size) & ieee754_type::exp_mask;
    unsigned_type mant =
        (f_as_ui & ieee754_type::mant_mask) + (1ULL << ieee754_type::mant_size);
    bool neg =
        (f_as_ui >> (ieee754_type::mant_size + ieee754_type::exp_size)) & 0x1;

    T output = static_cast<T>(std::ldexp(mant, static_cast<int32_t>(exp) -
                                                   ieee754_type::exp_shift -
                                                   ieee754_type::mant_size));

    if (neg) {
      output = -output;
    }

    return output;
  }
}

template <typename T, std::size_t N> auto serial_floating(double val) {
  T tval = static_cast<T>(val);

  if constexpr (N == 4 || N == 8) {
    using ieee754_type = traits::ieee754_traits<N>;
    using unsigned_type = typename ieee754_type::unsigned_type;

    int32_t exp;
    auto d_mant = std::frexp(tval, &exp);

    // exp to store
    exp = exp - 1 + ieee754_type::exp_shift; // -1 because the radix point is
                                             // shifted in frexp

    unsigned_type neg = std::signbit(tval);
    d_mant = fabs(d_mant) * 2 - 1; // again the shift is the radix point

    unsigned_type u_exp = exp;
    unsigned_type u_mant =
        static_cast<unsigned_type>(std::ldexp(d_mant, ieee754_type::mant_size));

    unsigned_type u_bytes;
    u_bytes = (u_mant & ieee754_type::mant_mask);
    u_bytes |= ((u_exp & ieee754_type::exp_mask) << ieee754_type::mant_size);
    u_bytes |=
        ((neg & 0x1) << (ieee754_type::mant_size + ieee754_type::exp_size));

    return serial_unsigned<N>(u_bytes);
  }
}

template <typename T, std::size_t N, typename Endian>
auto parse(uint8_t const *bytes) {
  auto bytes_arr = details::to_endian<N, Endian, little_endian_tag>()(bytes);

  if constexpr (std::is_floating_point_v<T>) {
    return parse_floating<T>(bytes_arr);
  } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
    return parse_unsigned<T>(bytes_arr);
  } else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
    return parse_signed<T>(bytes_arr);
  }
}

template <typename T, std::size_t N, typename Endian>
std::array<uint8_t, N> serial(std::string const &str) {
  std::array<uint8_t, N> res;
  if constexpr (std::is_floating_point_v<T>) {
    res = serial_floating<T, N>(std::stod(str));
  } else if constexpr (std::is_integral_v<T> && std::is_unsigned_v<T>) {
    res = serial_unsigned<N>(std::stoull(str, nullptr, details::base(str)));
  } else if constexpr (std::is_integral_v<T> && std::is_signed_v<T>) {
    res = serial_signed<T, N>(std::stoll(str, nullptr, details::base(str)));
  }

  return details::to_endian<N, little_endian_tag, Endian>()(&res[0]);
}
} // namespace mvm::num