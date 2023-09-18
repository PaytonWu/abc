// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_BYTE_BIT_NUMBERING
#define ABC_BYTE_BIT_NUMBERING

#pragma once

#include "abc/details/config.h"

namespace abc {

enum class byte_numbering {
    none,
    lsb0,
    msb0
};

enum class bit_numbering {
    invalid,
    lsb0,
    msb0
};

template <byte_numbering ByteNubmering>
struct byte_numbering_t {
    constexpr static byte_numbering value = ByteNubmering;
};

}

#endif //ABC_BYTE_BIT_NUMBERING
