// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_FIXED_BYTES)
#define ABC_FIXED_BYTES

#pragma once

#include "abc/details/fixed_bytes.h"
#include "abc/byte_bit_numbering.h"

namespace abc {

template <size_t Bytes, xbyte_numbering_t Endian = xbyte_numbering_t::msb0>
using xfixed_bytes_t = details::xabc_fixed_bytes<Bytes, Endian>;

using xbytes4_t = xfixed_bytes_t<4>;
using xbytes8_t = xfixed_bytes_t<8>;
using xbytes16_t = xfixed_bytes_t<16>;
using xbytes32_t = xfixed_bytes_t<32>;
using xbytes256_t = xfixed_bytes_t<256>;

using xbytes4_be_t = xfixed_bytes_t<4, xbyte_numbering_t::msb0>;
using xbytes8_be_t = xfixed_bytes_t<8, xbyte_numbering_t::msb0>;
using xbytes16_be_t = xfixed_bytes_t<16, xbyte_numbering_t::msb0>;
using xbytes32_be_t = xfixed_bytes_t<32, xbyte_numbering_t::msb0>;
using xbytes256_be_t = xfixed_bytes_t<256, xbyte_numbering_t::msb0>;


using xbytes4_le_t = xfixed_bytes_t<4, xbyte_numbering_t::lsb0>;
using xbytes8_le_t = xfixed_bytes_t<8, xbyte_numbering_t::lsb0>;
using xbytes16_le_t = xfixed_bytes_t<16, xbyte_numbering_t::lsb0>;
using xbytes32_le_t = xfixed_bytes_t<32, xbyte_numbering_t::lsb0>;
using xbytes256_le_t = xfixed_bytes_t<256, xbyte_numbering_t::lsb0>;

//using xbytes_view_t = std::span<xbyte_t const>;
//using xbytes32_view_t = std::span<xbyte_t const, 32>;
//using xbytes16_view_t = std::span<xbyte_t const, 16>;
//using xbytes8_view_t = std::span<xbyte_t const, 8>;
//using xbytes256_view_t = std::span<xbyte_t const, 256>;

}

#endif // ABC_FIXED_BYTES
