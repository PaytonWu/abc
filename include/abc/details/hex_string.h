// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_HEX_STRING)
#define ABC_DETAILS_HEX_STRING

#pragma once

#include "abc/bytes.h"
#include "abc/hex_string_format.h"
#include "abc/hex_utility.h"

#include <fmt/format.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/reverse.hpp>

#include <bit>
#include <cassert>
#include <compare>
#include <ranges>
#include <span>

namespace abc::details {

class [[nodiscard]] xabc_hex_string {
    xbytes_t binary_data_;  // force big endian

public:
    xabc_hex_string() = default;
    xabc_hex_string(xabc_hex_string const&) = default;
    xabc_hex_string(xabc_hex_string&&) = default;
    auto operator=(xabc_hex_string const&)->xabc_hex_string & = default;
    auto operator=(xabc_hex_string&&)->xabc_hex_string & = default;
    ~xabc_hex_string() = default;

    using format = xhex_string_format_t;

    constexpr static auto from(std::string_view const input) -> xabc_hex_string {
        return xabc_hex_string{ input };
    }

    constexpr static auto from(std::span<xbyte_t const> const input, std::endian const endian) -> xabc_hex_string {
        return xabc_hex_string{ input, endian };
    }

    [[nodiscard]] auto operator==(xabc_hex_string const&) const noexcept -> bool = default;
    [[nodiscard]] auto operator<=>(xabc_hex_string const&) const noexcept -> std::strong_ordering = default;

    [[nodiscard]] constexpr auto to_string(format const fmt = format::default_format) const -> std::string {
        std::string r;

        if (empty()) {
            return r;
        }

        r.reserve((binary_data_.size() + 1) * 2);
        std::array<char, 3> hex{ 0, 0, 0 };
        if (lower_case(fmt)) {
            r.append(hex_prefix);
            std::ranges::for_each(binary_data_ | std::views::transform([&hex](auto const byte) mutable { assert(hex[2] == 0); hex[0] = lower_case_hex_digits[byte >> 4], hex[1] = lower_case_hex_digits[byte & 0x0f]; return hex.data(); }), [&r](auto const* c_str) { r.append(c_str); });
        } else if (upper_case(fmt)) {
            r.append(hex_prefix_uppercase);
            std::ranges::for_each(binary_data_ | std::views::transform([&hex](auto const byte) mutable { assert(hex[2] == 0); hex[0] = upper_case_hex_digits[byte >> 4], hex[1] = upper_case_hex_digits[byte & 0x0f]; return hex.data(); }), [&r](auto const* c_str) { r.append(c_str); });
        } else {
            assert(false);
        }

        return r;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return binary_data_.empty();
    }

    [[nodiscard]] constexpr auto bytes_size() const noexcept -> size_t {
        return binary_data_.size();
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_t {
        return (binary_data_.size() << 1) + (static_cast<size_t>(!binary_data_.empty()) << 1);
    }

    [[nodiscard]] constexpr auto length() const noexcept -> size_t {
        return size();
    }

    constexpr void swap(xabc_hex_string& other) noexcept {
        binary_data_.swap(other.binary_data_);
    }

    template <std::endian Endian, std::enable_if_t<Endian == std::endian::little> * = nullptr>
    constexpr auto to_bytes() const -> xbytes_t {
        if (Endian == std::endian::little) {
            return binary_data_ | std::views::reverse | ranges::to<xbytes_t>();
        }
    }

    template<std::endian Endian, std::enable_if_t<Endian == std::endian::big> * = nullptr>
    constexpr auto to_bytes() const -> xbytes_t const & {
        return binary_data_;
    }

private:
    constexpr explicit xabc_hex_string(std::span<xbyte_t const> const input, std::endian const endian) : binary_data_{ std::begin(input), std::end(input) } {
        if (endian == std::endian::little) {
            std::ranges::reverse(binary_data_);
        }
    }

    constexpr explicit xabc_hex_string(std::string_view const input) : binary_data_{ to_binary(input) } {
    }
};

}

#endif
