// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_CONVERTER
#define ABC_CONVERTER

#pragma once

#include "abc/hex_string.h"
#include "abc/fixed_bytes.h"

namespace abc {

template <size_t N, byte_numbering ByteNumbering>
constexpr auto to_hex_string(fixed_bytes<N, ByteNumbering> const & fixed_bytes) -> hex_string {
    return hex_string::from_bytes(fixed_bytes, ByteNumbering);
}

}

#endif //ABC_CONVERTER
