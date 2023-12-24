// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BYTES_DECL
#define ABC_INCLUDE_ABC_BYTES_DECL

#pragma once

#include "bytes_fwd_decl.h"

namespace abc
{

using bytes_t = bytes<byte_numbering::none>;
using bytes_msb0_t = bytes<byte_numbering::msb0>;
using bytes_lsb0_t = bytes<byte_numbering::lsb0>;
using bytes_be_t = bytes_msb0_t;
using bytes_le_t = bytes_lsb0_t;

}

#endif //ABC_INCLUDE_ABC_BYTES_DECL
