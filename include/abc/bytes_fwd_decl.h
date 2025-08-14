// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BASIC_BYTES_FWD_DECL
#define ABC_INCLUDE_ABC_BASIC_BYTES_FWD_DECL

#pragma once

#include "byte_bit_numbering.h"

namespace abc
{

template <ByteNumbering ByteNumberingV>
class [[nodiscard]] BasicBytes;

using Bytes = BasicBytes<ByteNumbering::None>;
using Msb0Bytes = BasicBytes<ByteNumbering::Msb0>;
using Lsb0Bytes = BasicBytes<ByteNumbering::Lsb0>;
using BigEndianBytes = Msb0Bytes;
using LittleEndianBytes = Lsb0Bytes;

} // namespace abc

#endif // ABC_INCLUDE_ABC_BASIC_BYTES_FWD_DECL
