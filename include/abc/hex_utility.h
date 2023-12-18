// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_UTILITY)
#define ABC_HEX_UTILITY

#pragma once

#include <abc/bytes.h>
#include <abc/error.h>
#include <abc/expected.h>

#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/algorithm/all_of.hpp>
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

namespace abc
{

class hex_utility
{
public:
    constexpr static char lower_case_hex_digits[] = "0123456789abcdef";
    constexpr static char upper_case_hex_digits[] = "0123456789ABCDEF";
    constexpr static auto prefix = "0x";
    constexpr static auto prefix_uppercase = "0X";

#if defined(ABC_CPP23)
    constexpr static std::bitset<256> const hex_flag
    {
        "00000000000000000000000000000000"
        "00000000000000000000000000000000"
        "00000000000000000000000000000000"
        "00000000000000000000000000000000"
        "00000000000000000000000001111110"  // abcdef
        "00000000000000000000000001111110"  // ABCDEF
        "00000011111111110000000000000000"  // 0123456789
        "00000000000000000000000000000000"
    };
#else
    static std::bitset<256> const hex_flag;
#endif

    [[nodiscard]] constexpr static auto
    is_hex(unsigned char const ch) noexcept -> bool
    {
        return hex_flag[static_cast<size_t>(ch)];
    }

    [[nodiscard]] constexpr static auto
    is_hex(char const ch) noexcept -> bool
    {
        return is_hex(static_cast<unsigned char>(ch));
    }

    template <std::unsigned_integral T>
    [[nodiscard]] constexpr static auto
    is_hex(T const value) noexcept -> bool
    {
        return !(value & ~static_cast<T>(0x0f));
    }

    /// @brief check if the input string is a valid hex string.
    /// @param string_slice input string slice to check.
    /// @return true if the input string is a valid hex string, otherwise false.
    [[nodiscard]] constexpr static auto
    is_hex(std::string_view string_slice) noexcept -> bool
    {
        if (has_hex_prefix(string_slice))
        {
            string_slice = string_slice.substr(2);
        }

        return hex_utility::is_hex_without_prefix(string_slice);
    }

    /// @brief check if the input string has the prefix. this function doesn't check if the input string is a valid hex string, it only checks the prefix.
    /// @param string_slice input string slice to check.
    /// @return true if the input string has the prefix, otherwise false.
    [[nodiscard]] constexpr static auto
    has_hex_prefix(std::string_view const string_slice) noexcept -> bool
    {
        return string_slice.starts_with(prefix) || string_slice.starts_with(prefix_uppercase);
    }

    [[nodiscard]] constexpr static auto
    is_hex_without_prefix(std::string_view const string_slice) noexcept -> bool
    {   // "0123456789abcdefABCDEF"
        return ranges::all_of(string_slice,
                              [](auto const ch) {
                                  return is_hex(ch);
                              });
    }

    [[nodiscard]] constexpr static auto
    is_hex_with_prefix(std::string_view const string_slice) noexcept -> bool
    { // "0x0123456789abcdefABCDEF" or "0X0123456789abcdefABCDEF"
        if (!has_hex_prefix(string_slice))
        {
            return false;
        }

        return is_hex_without_prefix(string_slice.substr(2));
    }

    [[nodiscard]] static auto
    hex_char_to_binary(char ch) noexcept -> abc::expected<byte, std::error_code>;

};
}

#endif
