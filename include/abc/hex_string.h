// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_STRING)
#define ABC_HEX_STRING

#pragma once

#include "abc/details/hex_string.h"

namespace abc {
using xhex_string_format_t = details::xabc_hex_string_format;
using xhex_string_t = details::xabc_hex_string;
}

#include "abc/bytes.h"
#include "abc/simple_converter.h"

namespace abc {

template <>
auto to<xbytes_t, xhex_string_t>(xhex_string_t const & from) -> xbytes_t;

}

#endif
