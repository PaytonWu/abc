// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_HEX_STRING)
#define ABC_DETAILS_HEX_STRING

#pragma once

#include "abc/byte.h"
#include "abc/case_insensitive_string.h"
#include "abc/hex_utility.h"

#include <fmt/format.h>

#include <cassert>
#include <compare>
#include <span>


namespace abc::details {

enum class xabc_hex_string_format : uint8_t {
    without_prefix,
    with_prefix,
};

class [[nodiscard]] xabc_hex_string;

constexpr auto hex_char_to_binary(char const ch) noexcept -> xbyte_t {
    if ('0' <= ch && ch <= '9') {
        return static_cast<xbyte_t>(ch - '0');
    }

    if ('a' <= ch && ch <= 'f') {
        return static_cast<xbyte_t>(ch - 'a' + 10);
    }

    if ('A' <= ch && ch <= 'F') {
        return static_cast<xbyte_t>(ch - 'A' + 10);
    }

    assert(false);
    return static_cast<xbyte_t>(ch);
}

template <std::input_iterator Iterator>
constexpr auto to_hex_string(Iterator begin, Iterator end, xcase_insensitive_string_t<char> const & prefix) -> xcase_insensitive_string_t<char> {
    using iterator_traits = std::iterator_traits<Iterator>;
    static_assert(sizeof(typename iterator_traits::value_type) == 1);

    constexpr char hex_digits[] = "0123456789abcdef";

    size_t off = prefix.size();
    xcase_insensitive_string_t<char> hex(std::distance(begin, end) * 2 + off, '0');
    hex.replace(0, off, prefix);
    for (auto it = begin; it != end; ++it) {
        hex[off++] = hex_digits[(*it >> 4) & 0x0f];
        hex[off++] = hex_digits[*it & 0x0f];
    }
    return hex;
}

template <typename T>
constexpr auto to_hex_string(T const & data) -> xcase_insensitive_string_t<char> {
    return to_hex_string(std::begin(data), std::end(data), "");
}

template <typename T>
constexpr auto to_hex_string_prefixed(T const & data) -> xcase_insensitive_string_t<char> {
    return to_hex_string(std::begin(data), std::end(data), hex_prefix);
}

class [[nodiscard]] xabc_hex_string {
    xcase_insensitive_string_t<char> value_;

public:
    xabc_hex_string() = default;
    xabc_hex_string(xabc_hex_string const &) = default;
    xabc_hex_string(xabc_hex_string &&) = default;
    auto operator=(xabc_hex_string const &) -> xabc_hex_string & = default;
    auto operator=(xabc_hex_string &&) -> xabc_hex_string & = default;
    ~xabc_hex_string() = default;

    constexpr static auto from(std::string_view const input) -> xabc_hex_string {
        return xabc_hex_string{ input };
    }

    static auto from(std::span<xbyte_t const> const input) -> xabc_hex_string {
        return xabc_hex_string{ input };
    }

    [[nodiscard]] auto operator==(xabc_hex_string const &) const noexcept -> bool = default;
    [[nodiscard]] auto operator<=>(xabc_hex_string const &) const noexcept -> std::weak_ordering = default;

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return value_.empty();
    }

    [[nodiscard]] auto to_string(xabc_hex_string_format const format = xabc_hex_string_format::with_prefix) const -> std::string {
        if (empty()) {
            return {};
        }

        if (format == xabc_hex_string_format::with_prefix) {
            return fmt::format("{}{}", hex_prefix, value_);
        }

        return fmt::format("{}", value_);
    }

    [[nodiscard]] constexpr auto value() const -> std::string {
        return { value_.begin(), value_.end() };
    }

    [[nodiscard]] constexpr auto size(xabc_hex_string_format const format = xabc_hex_string_format::with_prefix) const noexcept -> size_t {
        return value_.size() + (static_cast<size_t>(format) << 1);
    }

    [[nodiscard]] constexpr auto length(xabc_hex_string_format const format = xabc_hex_string_format::with_prefix) const noexcept -> size_t {
        return size(format);
    }

    constexpr void swap(xabc_hex_string & other) noexcept {
        value_.swap(other.value_);
    }

private:
    constexpr explicit xabc_hex_string(std::span<xbyte_t const> const input) : value_{ to_hex_string(input) } {
        assert((value_.size() & 1u) == 0);
    }

    constexpr explicit xabc_hex_string(std::string_view input) : value_{ to_hex_string(input) } {
        assert((value_.size() & 1u) == 0);
    }
};

}

#endif
