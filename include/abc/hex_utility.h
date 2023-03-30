// Copyright(c) 2022 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_UTILITY)
#define ABC_HEX_UTILITY

#pragma once

#include "byte.h"

#include <cstddef>
#include <string_view>

namespace abc {

inline constexpr char const * hex_prefix{ "0x" };
inline constexpr char const * hex_prefix_uppercase{ "0X" };

constexpr auto is_hex_char(char const ch) noexcept -> bool {
    return '0' <= ch && ch <= '9' || 'a' <= ch && ch <= 'f' || 'A' <= ch && ch <= 'F';
}

constexpr auto is_hex_binary(xbyte_t const byte) noexcept -> bool {
    return static_cast<xbyte_t>(0x00) <= byte && byte <= static_cast<xbyte_t>(0x0f);
}

constexpr auto is_hex_binary(std::byte const byte) noexcept -> bool {
    return is_hex_binary(std::to_integer<xbyte_t>(byte));
}

auto hex_string_without_prefix(std::string_view const & string_slice) noexcept -> bool; // "0123456789abcdefABCDEF"
auto hex_string_with_prefix(std::string_view const & string_slice) noexcept -> bool;    // "0x0123456789abcdefABCDEF" or "0X0123456789abcdefABCDEF"
auto hex_string(std::string_view const & string_slice) noexcept -> bool;                // "0123456789abcdefABCDEF" or "0x0123456789abcdefABCDEF" or "0X0123456789abcdefABCDEF"

auto string_has_hex_prefix(std::string_view const & string_slice) noexcept -> bool;

}

#endif
