// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_DETAILS_ENDIAN
#define ABC_DETAILS_ENDIAN

#pragma once

#include "abc/details/config.h"

namespace abc::details {

enum class xabc_byte_numbering {
    none,
    lsb0,
    msb0
};

enum class xabc_bit_numbering {
    invalid,
    lsb0,
    msb0
};

}

#endif //ABC_DETAILS_ENDIAN
