// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_STRING_FORMAT)
#define ABC_HEX_STRING_FORMAT

#pragma once

#include "abc/details/config.h"

#include <type_traits>

namespace abc {

enum class [[nodiscard]] hex_string_format : uint8_t {
    default_format = 0b00000000u,   // 0x + leading_zero + lower_case
    lower_case = 0b00000000u,
    upper_case = 0b00000001u,
};

constexpr auto operator|(hex_string_format const lhs, hex_string_format const rhs) noexcept -> hex_string_format {
    using underlying_t = std::underlying_type_t<hex_string_format>;
    return static_cast<hex_string_format>(static_cast<underlying_t>(lhs) | static_cast<underlying_t>(rhs));
}

constexpr auto operator&(hex_string_format const lhs, hex_string_format const rhs) noexcept -> hex_string_format {
    using underlying_t = std::underlying_type_t<hex_string_format>;
    return static_cast<hex_string_format>(static_cast<underlying_t>(lhs) | static_cast<underlying_t>(rhs));
}

constexpr auto operator^(hex_string_format const lhs, hex_string_format const rhs) noexcept -> hex_string_format {
    using underlying_t = std::underlying_type_t<hex_string_format>;
    return static_cast<hex_string_format>(static_cast<underlying_t>(lhs) ^ static_cast<underlying_t>(rhs));
}

constexpr auto operator|=(hex_string_format & lhs, hex_string_format const rhs) noexcept -> hex_string_format & {
    return lhs = lhs | rhs;
}

constexpr auto operator&=(hex_string_format & lhs, hex_string_format const rhs) noexcept -> hex_string_format & {
    return lhs = lhs & rhs;
}

constexpr auto operator^=(hex_string_format & lhs, hex_string_format const rhs) noexcept -> hex_string_format & {
    return lhs = lhs ^ rhs;
}

constexpr bool lower_case(hex_string_format const format) noexcept {
    return !(static_cast<uint8_t>(format) & 0b00000001u);
}

constexpr bool upper_case(hex_string_format const format) noexcept {
    return !lower_case(format);
}

}

#endif
