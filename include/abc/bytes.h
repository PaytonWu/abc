// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_BYTES)
#define ABC_BYTES

#pragma once

#include "abc/byte.h"
#include "abc/byte_bit_numbering.h"
#include "abc/details/bytes.h"

#include <array>
#include <span>
#include <utility>
#include <vector>

namespace abc {

using xbytes_t = details::xabc_bytes;
using xbytes_be_t = details::xabc_bytes_with<xbyte_numbering_t::msb0>;
using xbytes_le_t = details::xabc_bytes_with<xbyte_numbering_t::lsb0>;

[[nodiscard]] constexpr auto operator<=>(xbytes_t const & lhs, std::vector<xbyte_t> const & rhs) -> std::strong_ordering {
    return static_cast<std::vector<xbyte_t>>(lhs) <=> rhs;
}

[[nodiscard]] constexpr auto operator==(xbytes_t const & lhs, std::vector<xbyte_t> const & rhs) -> bool {
    return static_cast<std::vector<xbyte_t>>(lhs) == rhs;
}

//constexpr auto operator+(std::span<xbyte_t const> const lhs, xbytes_t const& rhs) -> xbytes_t {
//    return xbytes_t{ lhs } + rhs;
//}
//
//constexpr auto operator+(xbyte_t const lhs, std::span<xbyte_t const> const rhs) -> xbytes_t {
//    return xbytes_t{ lhs } + rhs;
//}

constexpr void swap(xbytes_t & lhs, xbytes_t & rhs) noexcept {
    lhs.swap(rhs);
}

constexpr auto erase(xbytes_t & c, xbytes_t::value_type const value) -> xbytes_t::size_type {
    return std::erase(static_cast<std::vector<xbyte_t> &>(c), value);
}

template <typename Predictor>
constexpr auto erase_if(xbytes_t & c, Predictor predictor) -> xbytes_t::size_type {
    return std::erase_if(static_cast<std::vector<xbyte_t> &>(c), std::move(predictor));
}

}

#endif
