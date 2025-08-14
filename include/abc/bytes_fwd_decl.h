// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BYTES_FWD_DECL
#define ABC_INCLUDE_ABC_BYTES_FWD_DECL

#pragma once

#include "byte_bit_numbering.h"

namespace abc
{

template <ByteNumbering ByteNumberingV>
class [[nodiscard]] Bytes;

using bytes_t = Bytes<ByteNumbering::None>;
using bytes_msb0_t = Bytes<ByteNumbering::Msb0>;
using bytes_lsb0_t = Bytes<ByteNumbering::Lsb0>;
using bytes_be_t = bytes_msb0_t;
using bytes_le_t = bytes_lsb0_t;
}

#endif //ABC_INCLUDE_ABC_BYTES_FWD_DECL
