// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_BYTES)
#define ABC_BYTES

#pragma once

#include "byte.h"
#include "details/bytes.h"

#include <array>
#include <span>
#include <utility>
#include <vector>

namespace abc {

using xbytes_t = details::xabc_bytes;

using xbytes32_t = std::array<xbyte_t, 32>;
using xbytes16_t = std::array<xbyte_t, 16>;
using xbytes8_t = std::array<xbyte_t, 8>;
using xbytes1_t = std::array<xbyte_t, 1>;
using xbytes256_t = std::array<xbyte_t, 256>;

using xbytes_view_t = std::span<xbyte_t const>;
using xbytes32_view_t = std::span<xbyte_t const, 32>;
using xbytes16_view_t = std::span<xbyte_t const, 16>;
using xbytes8_view_t = std::span<xbyte_t const, 8>;
using xbytes256_view_t = std::span<xbyte_t const, 256>;


[[nodiscard]] constexpr auto operator<=>(xbytes_t const & lhs, xbytes_t const & rhs) -> std::strong_ordering {
    return lhs.raw_data() <=> rhs.raw_data();
}

[[nodiscard]] constexpr auto operator==(xbytes_t const & lhs, xbytes_t const & rhs) -> bool {
    return lhs.raw_data() == rhs.raw_data();
}

[[nodiscard]] constexpr auto operator<=>(xbytes_t const & lhs, std::vector<xbyte_t> const & rhs) -> std::strong_ordering {
    return lhs.raw_data() <=> rhs;
}

[[nodiscard]] constexpr auto operator==(xbytes_t const & lhs, std::vector<xbyte_t> const & rhs) -> bool {
    return lhs.raw_data() == rhs;
}

[[nodiscard]] auto operator+(std::span<xbyte_t const> const & lhs, xbytes_t const & rhs)->xbytes_t;
[[nodiscard]] auto operator+(xbyte_t lhs, xbytes_t const & rhs)->xbytes_t;
[[nodiscard]] auto operator+(xbyte_t lhs, std::span<xbyte_t const> const & rhs)->xbytes_t;

constexpr void swap(xbytes_t & lhs, xbytes_t & rhs) noexcept {
    lhs.swap(rhs);
}

constexpr auto erase(xbytes_t & c, xbytes_t::value_type const value) -> xbytes_t::size_type {
    return std::erase(c.raw_data(), value);
}

template <typename Predictor>
constexpr auto erase_if(xbytes_t & c, Predictor predictor) -> xbytes_t::size_type {
    return std::erase_if(c.raw_data(), std::move(predictor));
}

}

#include "hex_string.h"
#include "simple_converter.h"

namespace abc {

template <>
auto to<xhex_string_t, xbytes_t>(xbytes_t const & from)->xhex_string_t;

}

#endif
