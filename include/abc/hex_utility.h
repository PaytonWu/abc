// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_UTILITY)
#define ABC_HEX_UTILITY

#pragma once

#include "abc/byte.h"

#include <algorithm>
#include <bitset>
#include <cstddef>
#include <concepts>
#include <string_view>

namespace abc {

inline constexpr char const * hex_prefix{ "0x" };
inline constexpr char const * hex_prefix_uppercase{ "0X" };
inline constexpr std::bitset<256> hex_flag{
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000000000000"
    "00000000000000000000000001111110"  // abcdef
    "00000000000000000000000001111110"  // ABCDEF
    "00000011111111110000000000000000"  // 0123456789
    "00000000000000000000000000000000"
};

[[nodiscard]] constexpr auto hex_char(unsigned char const ch) noexcept -> bool {
    return hex_flag[static_cast<size_t>(ch)];
}

[[nodiscard]] constexpr auto hex_char(char const ch) noexcept -> bool {
    return hex_char(static_cast<unsigned char>(ch));
}

template <std::unsigned_integral T>
[[nodiscard]] constexpr auto hex_binary(T const value) noexcept -> bool {
    return !(value & ~static_cast<T>(0x0f));
}

[[nodiscard]] constexpr auto hex_string_without_prefix(std::string_view const string_slice) noexcept -> bool {   // "0123456789abcdefABCDEF"
    return std::ranges::all_of(string_slice,
        [](auto const ch) {
            return hex_char(ch);
        });
}

[[nodiscard]] constexpr auto has_hex_prefix(std::string_view const string_slice) noexcept -> bool {
    return string_slice.starts_with(hex_prefix) || string_slice.starts_with(hex_prefix_uppercase);
}

[[nodiscard]] constexpr auto hex_string_with_prefix(std::string_view const string_slice) noexcept -> bool { // "0x0123456789abcdefABCDEF" or "0X0123456789abcdefABCDEF"
    if (!has_hex_prefix(string_slice)) {
        return false;
    }

    return hex_string_without_prefix(string_slice.substr(2));
}

[[nodiscard]] constexpr auto hex_string(std::string_view const string_slice) noexcept -> bool { // "0123456789abcdefABCDEF" or "0x0123456789abcdefABCDEF" or "0X0123456789abcdefABCDEF"
    return hex_string_with_prefix(string_slice) || hex_string_without_prefix(string_slice);
}

}

#endif
