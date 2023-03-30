// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_HEX_STRING)
#define ABC_DETAILS_HEX_STRING

#pragma once

#include "byte.h"
#include "case_insensitive_string.h"

#include <compare>
#include <span>

namespace abc::details {

enum class xabc_hex_string_format : uint8_t {
    without_prefix,
    with_prefix,
};

class [[nodiscard]] xabc_hex_string {
    xcase_insensitive_string_t<char> value_;

public:
    xabc_hex_string() = default;
    xabc_hex_string(xabc_hex_string const &) = default;
    xabc_hex_string(xabc_hex_string &&) = default;
    auto operator=(xabc_hex_string const &)->xabc_hex_string & = default;
    auto operator=(xabc_hex_string &&)->xabc_hex_string & = default;
    ~xabc_hex_string() = default;

    [[nodiscard]] static auto from(std::string_view const & input) -> xabc_hex_string;
    [[nodiscard]] static auto from(std::span<xbyte_t const>  const & input) -> xabc_hex_string;

    [[nodiscard]] auto operator==(xabc_hex_string const &) const noexcept -> bool = default;
    [[nodiscard]] auto operator<=>(xabc_hex_string const &) const noexcept -> std::weak_ordering = default;

    [[nodiscard]] auto empty() const noexcept -> bool;
    [[nodiscard]] auto to_string(xabc_hex_string_format format = xabc_hex_string_format::with_prefix) const->std::string;
    [[nodiscard]] auto value() const->std::string;
    [[nodiscard]] auto size(xabc_hex_string_format format = xabc_hex_string_format::with_prefix) const noexcept -> size_t;
    [[nodiscard]] auto length(xabc_hex_string_format format = xabc_hex_string_format::with_prefix) const noexcept -> size_t;

    auto swap(xabc_hex_string & other) noexcept -> void;

private:
    explicit xabc_hex_string(std::span<xbyte_t const> const & input);
    explicit xabc_hex_string(std::string_view const & input);
};

}

#endif
