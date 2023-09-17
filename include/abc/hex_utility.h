// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_UTILITY)
#define ABC_HEX_UTILITY

#pragma once

#include <abc/bytes.h>
#include <abc/error.h>
#include <abc/expected.h>

#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/chunk.hpp>
#include <range/v3/view/enumerate.hpp>
#include <range/v3/view/reverse.hpp>

#include <algorithm>
#include <bitset>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <string_view>
#include <system_error>

namespace abc::hex_utility {

constexpr inline char lower_case_hex_digits[] = "0123456789abcdef";
constexpr inline char upper_case_hex_digits[] = "0123456789ABCDEF";
#if defined(ABC_CPP23)
constexpr
#endif
inline std::bitset<256> const hex_flag{
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000001111110"  // abcdef
    "00000000000000000000000001111110"  // ABCDEF
    "00000011111111110000000000000000"  // 0123456789
    "00000000000000000000000000000000"
};

[[nodiscard]] constexpr auto is_hex(unsigned char const ch) noexcept -> bool {
    return hex_flag[static_cast<size_t>(ch)];
}

[[nodiscard]] constexpr auto is_hex(char const ch) noexcept -> bool {
    return is_hex(static_cast<unsigned char>(ch));
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr auto is_hex(T const value) noexcept -> bool {
    return !(value & ~static_cast<T>(0x0f));
}

[[nodiscard]] auto hex_char_to_binary(char ch) noexcept -> abc::expected<byte, std::error_code>;

}

#endif
