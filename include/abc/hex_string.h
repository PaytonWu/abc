// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_STRING)
#define ABC_HEX_STRING

#pragma once

#include "byte.h"
#include "char_traits_case_insensitive.h"
#include "hex_string_fwd.h"
#include "simple_converter.h"

#include <compare>
#include <span>
#include <string_view>

namespace abc::details {

enum class xabc_hex_string_format : uint8_t {
    without_prefix,
    with_prefix,
};

}

namespace abc {
using xhex_string_format_t = details::xabc_hex_string_format;
}

namespace abc::details {

class [[nodiscard]] xabc_hex_string {
    xcase_insentive_string_t<char> value_;

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
    [[nodiscard]] auto to_string(xhex_string_format_t format = xhex_string_format_t::with_prefix) const->std::string;
    [[nodiscard]] auto value() const->std::string;
    [[nodiscard]] auto size(xhex_string_format_t format = xhex_string_format_t::with_prefix) const noexcept -> size_t;
    [[nodiscard]] auto length(xhex_string_format_t format = xhex_string_format_t::with_prefix) const noexcept -> size_t;

    auto swap(xabc_hex_string & other) noexcept -> void;

private:
    explicit xabc_hex_string(std::span<xbyte_t const> const & input);
    explicit xabc_hex_string(std::string_view const & input);
};
}

#include "bytes_fwd.h"

namespace abc {

template <>
auto to<xbytes_t, xhex_string_t>(xhex_string_t const & from)->xbytes_t;

}

#endif
