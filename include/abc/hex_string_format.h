// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_STRING_FORMAT)
#define ABC_HEX_STRING_FORMAT

#pragma once

#include "abc/details/hex_string_format.h"

namespace abc {

using xhex_string_format_t = details::xabc_hex_string_format;

constexpr bool lower_case(xhex_string_format_t const format) noexcept {
    return !(static_cast<uint8_t>(format) >> 4);
}

constexpr bool upper_case(xhex_string_format_t const format) noexcept {
    return static_cast<bool>((static_cast<uint8_t>(format) >> 4) & 0b0001);
}

constexpr bool mixed_case(xhex_string_format_t const format) noexcept {
    return static_cast<bool>((static_cast<uint8_t>(format) >> 4) & 0b0010);
}

constexpr bool with_prefix(xhex_string_format_t const format) noexcept {
    return static_cast<bool>((static_cast<uint8_t>(format) & 0b00000001u));
}

constexpr bool without_prefix(xhex_string_format_t const format) noexcept {
    return !with_prefix(format);
}

}

#endif
