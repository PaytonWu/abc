// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_DETAILS_BYTE_NUMBERING_DECL
#define ABC_INCLUDE_ABC_DETAILS_BYTE_NUMBERING_DECL

#pragma once

#include "byte_numbering_fwd_decl.h"

namespace abc::details
{

enum class ByteNumbering
{
    None,
    Lsb0,
    Msb0
};

template <>
struct ByteNumberingType<ByteNumbering::None>
{
    constexpr static ByteNumbering byte_numbering_v = ByteNumbering::None;
};

template <>
struct ByteNumberingType<ByteNumbering::Msb0>
{
    constexpr static ByteNumbering byte_numbering_v = ByteNumbering::Msb0;
    constexpr static std::endian endian_value = std::endian::big;
};

template <>
struct ByteNumberingType<ByteNumbering::Lsb0>
{
    constexpr static ByteNumbering byte_numbering_v = ByteNumbering::Lsb0;
    constexpr static std::endian endian_value = std::endian::little;
};

template <>
struct EndianType<std::endian::big>
{
    constexpr static std::endian endian_value = std::endian::big;
    constexpr static ByteNumbering byte_numbering_v = ByteNumbering::Msb0;
};

template <>
struct EndianType<std::endian::little>
{
    constexpr static std::endian endian_value = std::endian::little;
    constexpr static ByteNumbering byte_numbering_v = ByteNumbering::Lsb0;
};

} // namespace abc::details

#endif // ABC_INCLUDE_ABC_DETAILS_BYTE_NUMBERING_DECL
