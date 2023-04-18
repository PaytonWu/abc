// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_SIMPLE_CONVERTER)
#define ABC_SIMPLE_CONVERTER

#pragma once

#include "abc/bytes.h"
#include "abc/hex_string.h"

namespace abc {

template <typename ToT, typename FromT>
auto to(FromT const & from)->ToT;

template <>
constexpr auto to<xbytes_t, xhex_string_t>(xhex_string_t const& from) -> xbytes_t {
    return from.binary_data();
}

template <>
constexpr auto to<xhex_string_t, xbytes_t>(xbytes_t const& from) -> xhex_string_t {
    return xhex_string_t::from(from);
}

}

#endif
