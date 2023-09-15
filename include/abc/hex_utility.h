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

namespace abc {

constexpr inline char const * hex_prefix{ "0x" };
constexpr inline char const * hex_prefix_uppercase{ "0X" };
constexpr inline std::string_view hex_prefix_view{ hex_prefix };
constexpr inline std::string_view hex_prefix_uppercase_view{ hex_prefix_uppercase };
constexpr inline char lower_case_hex_digits[] = "0123456789abcdef";
constexpr inline char upper_case_hex_digits[] = "0123456789ABCDEF";
#if defined(ABC_CPP23)
constexpr inline std::bitset<256> hex_flag
#else
inline std::bitset<256> const hex_flag
#endif
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

[[nodiscard]] constexpr auto hex_char_to_binary(char const ch) noexcept -> abc::expected<byte, errc> {
    if ('0' <= ch && ch <= '9') {
        return static_cast<byte>(ch - '0');
    }

    if ('a' <= ch && ch <= 'f') {
        return static_cast<byte>(ch - 'a' + 10);
    }

    if ('A' <= ch && ch <= 'F') {
        return static_cast<byte>(ch - 'A' + 10);
    }

    return make_unexpected(errc::invalid_hex_char);
}

template <std::endian Endian>
constexpr auto hex_string_to_binary(std::string_view string_slice) -> expected<bytes, errc> {
    if (has_hex_prefix(string_slice)) {
        string_slice.remove_prefix(2);
    }

    if (!hex_string_without_prefix(string_slice)) {
        return make_unexpected(errc::invalid_hex_char);
    }

    bytes binary_data;
    binary_data.reserve((string_slice.size() + 1) / 2);
    if constexpr (Endian == std::endian::big) {
        if (string_slice.size() & 1) {
            [[maybe_unused]] auto r = hex_char_to_binary(string_slice.front()).transform([&binary_data, &string_slice](auto const b) mutable { binary_data.push_back(b); string_slice.remove_prefix(1); });
            assert(r.has_value());
        }

        auto const & chunks = string_slice | ranges::views::chunk(2);
        ranges::for_each(chunks, [&binary_data](ranges::viewable_range auto && compound_byte) mutable {
            byte byte{};
            for (auto const [i, nibble_byte] : compound_byte | ranges::views::reverse | ranges::views::enumerate) {
                byte |= hex_char_to_binary(nibble_byte).value() << (4 * i);
            }
            binary_data.push_back(byte);
        });

        return binary_data;
    }

    if constexpr (Endian == std::endian::little) {
        if (string_slice.size() & 1) {
            [[maybe_unused]] auto r = hex_char_to_binary(string_slice.back()).transform([&binary_data, &string_slice](auto const b) mutable { binary_data.push_back(b); string_slice.remove_suffix(1); });
            assert(r.has_value());
        }

        auto const & chunks = string_slice | ranges::views::reverse | ranges::views::chunk(2);
        ranges::for_each(chunks, [&binary_data](ranges::viewable_range auto && compound_byte) mutable {
            byte byte{};
            for (auto const [i, nibble_byte] : compound_byte | ranges::views::enumerate) {
                byte |= hex_char_to_binary(nibble_byte).value() << (4 * i);
            }
            binary_data.push_back(byte);
        });
        return binary_data;
    }
}

constexpr auto to_binary(std::string_view input) -> bytes {
    return hex_string_to_binary<std::endian::big>(input).or_else([input](auto const) { return expected<bytes, errc>{ std::in_place, std::begin(input), std::end(input) }; }).value();
}

}

#endif
