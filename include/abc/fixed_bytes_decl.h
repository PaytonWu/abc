// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_FIXED_BYTES_DECL
#define ABC_INCLUDE_ABC_FIXED_BYTES_DECL

#pragma once

#include "fixed_bytes_fwd_decl.h"

namespace abc {

using bytes4_msb0_t = fixed_bytes<4, byte_numbering::msb0>;
using bytes8_msb0_t = fixed_bytes<8, byte_numbering::msb0>;
using bytes16_msb0_t = fixed_bytes<16, byte_numbering::msb0>;
using bytes32_msb0_t = fixed_bytes<32, byte_numbering::msb0>;
using bytes48_msb0_t = fixed_bytes<48, byte_numbering::msb0>;
using bytes64_msb0_t = fixed_bytes<64, byte_numbering::msb0>;
using bytes96_msb0_t = fixed_bytes<96, byte_numbering::msb0>;
using bytes128_msb0_t = fixed_bytes<128, byte_numbering::msb0>;
using bytes256_msb0_t = fixed_bytes<256, byte_numbering::msb0>;

using bytes4_be_t = bytes4_msb0_t;
using bytes8_be_t = bytes8_msb0_t;
using bytes16_be_t = bytes16_msb0_t;
using bytes32_be_t = bytes32_msb0_t;
using bytes48_be_t = bytes48_msb0_t;
using bytes64_be_t = bytes64_msb0_t;
using bytes96_be_t = bytes96_msb0_t;
using bytes128_be_t = bytes128_msb0_t;
using bytes256_be_t = bytes256_msb0_t;

using bytes4_lsb0_t = fixed_bytes<4, byte_numbering::lsb0>;
using bytes8_lsb0_t = fixed_bytes<8, byte_numbering::lsb0>;
using bytes16_lsb0_t = fixed_bytes<16, byte_numbering::lsb0>;
using bytes32_lsb0_t = fixed_bytes<32, byte_numbering::lsb0>;
using bytes48_lsb0_t = fixed_bytes<48, byte_numbering::lsb0>;
using bytes64_lsb0_t = fixed_bytes<64, byte_numbering::lsb0>;
using bytes96_lsb0_t = fixed_bytes<96, byte_numbering::lsb0>;
using bytes128_lsb0_t = fixed_bytes<128, byte_numbering::lsb0>;
using bytes256_lsb0_t = fixed_bytes<256, byte_numbering::lsb0>;

using bytes4_le_t = bytes4_lsb0_t;
using bytes8_le_t = bytes8_lsb0_t;
using bytes16_le_t = bytes16_lsb0_t;
using bytes32_le_t = bytes32_lsb0_t;
using bytes48_le_t = bytes48_lsb0_t;
using bytes64_le_t = bytes64_lsb0_t;
using bytes96_le_t = bytes96_lsb0_t;
using bytes128_le_t = bytes128_lsb0_t;
using bytes256_le_t = bytes256_lsb0_t;

using bytes4_t = fixed_bytes<4, byte_numbering::none>;
using bytes8_t = fixed_bytes<8, byte_numbering::none>;
using bytes16_t = fixed_bytes<16, byte_numbering::none>;
using bytes32_t = fixed_bytes<32, byte_numbering::none>;
using bytes48_t = fixed_bytes<48, byte_numbering::none>;
using bytes64_t = fixed_bytes<64, byte_numbering::none>;
using bytes96_t = fixed_bytes<96, byte_numbering::none>;
using bytes128_t = fixed_bytes<128, byte_numbering::none>;
using bytes256_t = fixed_bytes<256, byte_numbering::none>;

}

#endif //ABC_INCLUDE_ABC_FIXED_BYTES_DECL
