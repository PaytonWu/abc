// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_BIGINT)
#define ABC_BIGINT

#pragma once

#include "abc/bytes.h"

#include <boost/multiprecision/cpp_int.hpp>

namespace abc {

using xuint128_t = boost::multiprecision::uint128_t;
using xuint256_t = boost::multiprecision::uint256_t;
using xuint512_t = boost::multiprecision::uint512_t;
using xuint1024_t = boost::multiprecision::uint1024_t;

using xint128_t = boost::multiprecision::int128_t;
using xint256_t = boost::multiprecision::int256_t;
using xint512_t = boost::multiprecision::int256_t;
using xint1024_t = boost::multiprecision::int1024_t;

using xuint160_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<160, 160, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;
using xint160_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<160, 160, boost::multiprecision::unsigned_magnitude, boost::multiprecision::unchecked, void>>;

using xbigint_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<>>;

template <typename InputT, typename OutputT> requires std::same_as<InputT, xbigint_t> ||
std::same_as<InputT, xuint128_t> ||
std::same_as<InputT, xuint256_t> ||
std::same_as<InputT, xuint512_t> ||
std::same_as<InputT, xuint160_t> ||
std::unsigned_integral<InputT>
auto to_big_endian(InputT input, OutputT & output) -> void {
    for (auto i = output.size(); i != 0; input >>= 8, --i) {
        InputT v = input & static_cast<InputT>(0xff);
        output[i - 1] = static_cast<typename OutputT::value_type>(static_cast<uint8_t>(v));
    }
}

template <typename OutputT, typename InputT>
[[nodiscard]] auto from_big_endian(InputT const & bytes) -> OutputT {
    OutputT ret = static_cast<OutputT>(0);
    for (auto i : bytes) {
        ret = static_cast<OutputT>((ret << 8) | static_cast<uint8_t>(static_cast<std::make_unsigned_t<decltype(i)>>(i)));
    }
    return ret;
}

inline auto to_big_endian(xuint256_t const & val) -> xbytes_t {
    xbytes_t ret(32);
    to_big_endian(val, ret);
    return ret;
}

inline auto to_big_endian(xuint160_t const & val) -> xbytes_t {
    xbytes_t ret(20);
    to_big_endian(val, ret);
    return ret;
}

}

#endif
