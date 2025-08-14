// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_FIXED_BYTES_FWD_DECL
#define ABC_INCLUDE_ABC_FIXED_BYTES_FWD_DECL

#pragma once

#include "byte_bit_numbering.h"

#include <cstddef>

namespace abc
{

template <std::size_t N, ByteNumbering ByteNumbering>
class [[nodiscard]] fixed_bytes;

}

#endif // ABC_INCLUDE_ABC_FIXED_BYTES_FWD_DECL
