// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_HEX_STRING)
#define ABC_DETAILS_HEX_STRING

#pragma once

#include "abc/bytes.h"
#include "abc/hex_string_format.h"
#include "abc/hex_utility.h"

#include <fmt/format.h>

#include <cassert>
#include <compare>
#include <ranges>
#include <span>


namespace abc::details {

class [[nodiscard]] xabc_hex_string;

constexpr auto to_binary(std::string_view input) -> xbytes_t {
    if (hex_string(input)) {
        return hex_string_to_binary(input);
    }

    return xbytes_t{ input.begin(), input.end() };
}

class [[nodiscard]] xabc_hex_string {
    xbytes_t binary_data_;

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

    constexpr static auto from(std::span<xbyte_t const> const input) -> xabc_hex_string {
        return xabc_hex_string{ input };
    }

    [[nodiscard]] auto operator==(xabc_hex_string const&) const noexcept -> bool = default;
    [[nodiscard]] auto operator<=>(xabc_hex_string const&) const noexcept -> std::strong_ordering = default;

    [[nodiscard]] constexpr auto to_string(format const fmt = format::default_format) const -> std::string {
        std::string r;

        if (empty()) {
            return r;
        }

        r.reserve((binary_data_.size() + 1) * 2);
        if (lower_case(fmt)) {
            r.append(hex_prefix);
            std::ranges::for_each(binary_data_ | std::views::transform([](auto const byte) { return fmt::format("{:02x}", byte); }), [&r](auto str) { r.append(std::move(str)); });
        } else if (upper_case(fmt)) {
            r.append(hex_prefix_uppercase);
            std::ranges::for_each(binary_data_ | std::views::transform([](auto const byte) { return fmt::format("{:02X}", byte); }), [&r](auto str) { r.append(std::move(str)); });
        } else {
            assert(false);
        }

        return r;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return binary_data_.empty();
    }

    [[nodiscard]] constexpr auto binary_data() const noexcept -> xbytes_t const & {
        return binary_data_;
    }

    [[nodiscard]] constexpr auto binary_size() const noexcept -> size_t {
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

private:
    constexpr explicit xabc_hex_string(std::span<xbyte_t const> const input) : binary_data_{ std::begin(input), std::end(input) } {
    }

    constexpr explicit xabc_hex_string(std::string_view const input) : binary_data_{ to_binary(input) } {
    }
};

}

#endif
