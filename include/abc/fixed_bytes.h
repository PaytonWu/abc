// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_FIXED_BYTES)
#define ABC_FIXED_BYTES

#pragma once

#include "abc/details/fixed_bytes.h"

namespace abc {

template <size_t Bytes, std::endian Endian = std::endian::big>
using xfixed_bytes_t = details::xabc_fixed_bytes<Bytes, Endian>;

using xbytes4_t = xfixed_bytes_t<4>;
using xbytes8_t = xfixed_bytes_t<8>;
using xbytes16_t = xfixed_bytes_t<16>;
using xbytes32_t = xfixed_bytes_t<32>;
using xbytes256_t = xfixed_bytes_t<256>;

using xbytes4_be_t = xfixed_bytes_t<4, std::endian::big>;
using xbytes8_be_t = xfixed_bytes_t<8, std::endian::big>;
using xbytes16_be_t = xfixed_bytes_t<16, std::endian::big>;
using xbytes32_be_t = xfixed_bytes_t<32, std::endian::big>;
using xbytes256_be_t = xfixed_bytes_t<256, std::endian::big>;


using xbytes4_le_t = xfixed_bytes_t<4, std::endian::little>;
using xbytes8_le_t = xfixed_bytes_t<8, std::endian::little>;
using xbytes16_le_t = xfixed_bytes_t<16, std::endian::little>;
using xbytes32_le_t = xfixed_bytes_t<32, std::endian::little>;
using xbytes256_le_t = xfixed_bytes_t<256, std::endian::little>;

//using xbytes_view_t = std::span<xbyte_t const>;
//using xbytes32_view_t = std::span<xbyte_t const, 32>;
//using xbytes16_view_t = std::span<xbyte_t const, 16>;
//using xbytes8_view_t = std::span<xbyte_t const, 8>;
//using xbytes256_view_t = std::span<xbyte_t const, 256>;

}

#endif // ABC_FIXED_BYTES
