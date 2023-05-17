// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_HEX_STRING_FORMAT)
#define ABC_DETAILS_HEX_STRING_FORMAT

#pragma once

#include <type_traits>

namespace abc::details {

enum class [[nodiscard]] xabc_hex_string_format : uint8_t {
    default_format = 0b00000000u,   // 0x + leading_zero + lower_case
    lower_case = 0b00000000u,
    upper_case = 0b00000001u,
};

constexpr auto operator|(xabc_hex_string_format const lhs, xabc_hex_string_format const rhs) noexcept -> xabc_hex_string_format {
    using underlying_t = std::underlying_type_t<xabc_hex_string_format>;
    return static_cast<xabc_hex_string_format>(static_cast<underlying_t>(lhs) | static_cast<underlying_t>(rhs));
}

constexpr auto operator&(xabc_hex_string_format const lhs, xabc_hex_string_format const rhs) noexcept -> xabc_hex_string_format {
    using underlying_t = std::underlying_type_t<xabc_hex_string_format>;
    return static_cast<xabc_hex_string_format>(static_cast<underlying_t>(lhs) | static_cast<underlying_t>(rhs));
}

constexpr auto operator^(xabc_hex_string_format const lhs, xabc_hex_string_format const rhs) noexcept -> xabc_hex_string_format {
    using underlying_t = std::underlying_type_t<xabc_hex_string_format>;
    return static_cast<xabc_hex_string_format>(static_cast<underlying_t>(lhs) ^ static_cast<underlying_t>(rhs));
}

constexpr auto operator|=(xabc_hex_string_format & lhs, xabc_hex_string_format const rhs) noexcept -> xabc_hex_string_format & {
    return lhs = lhs | rhs;
}

constexpr auto operator&=(xabc_hex_string_format & lhs, xabc_hex_string_format const rhs) noexcept -> xabc_hex_string_format & {
    return lhs = lhs & rhs;
}

constexpr auto operator^=(xabc_hex_string_format & lhs, xabc_hex_string_format const rhs) noexcept -> xabc_hex_string_format & {
    return lhs = lhs ^ rhs;
}

}

#endif
