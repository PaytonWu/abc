// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_STRING)
#define ABC_HEX_STRING

#pragma once

#include "abc/details/hex_string.h"

namespace abc {

using xhex_string_t = details::xabc_hex_string;

}

#include "abc/bytes.h"
#include "abc/simple_converter.h"

namespace abc {

template <>
constexpr auto to<xbytes_t, xhex_string_t>(xhex_string_t const & from) -> xbytes_t {
    size_t index = 0;
    xbytes_t ret;
    auto const & value = from.value();
    ret.reserve((value.size() - index + 1) / 2);

    if (value.size() % 2) {
        auto const h = details::hex_char_to_binary(value[index++]);
        ret.push_back(h);
    }

    for (auto i = index; i < value.size(); i += 2) {
        auto const h = details::hex_char_to_binary(value[i]);
        auto const l = details::hex_char_to_binary(value[1 + i]);

        ret.push_back(static_cast<xbyte_t>(h * 16 + l));
    }

    return ret;
}

}

#endif
