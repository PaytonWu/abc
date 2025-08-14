// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_BYTE_BIT_NUMBERING
#define ABC_BYTE_BIT_NUMBERING

#pragma once

#include "abc/details/config.h"

#include "details/byte_numbering_decl.h"

namespace abc
{

using ByteNumbering = details::ByteNumbering;

template <ByteNumbering ByteNubmeringV>
using ByteNumberingType = details::ByteNumberingType<ByteNubmeringV>;

template <std::endian Endian>
using EndianType = details::EndianType<Endian>;

enum class bit_numbering
{
    invalid,
    lsb0,
    msb0
};

using ByteNumberingMsb0 = ByteNumberingType<ByteNumbering::Msb0>;
using ByteNumberingLsb0 = ByteNumberingType<ByteNumbering::Lsb0>;
using ByteNumberingNone = ByteNumberingType<ByteNumbering::None>;

constexpr auto
operator==(ByteNumbering ByteNumbering, std::endian endian) noexcept -> bool
{
    return (ByteNumbering == ByteNumbering::Msb0 && endian == std::endian::big) || (ByteNumbering == ByteNumbering::Lsb0 && endian == std::endian::little);
}

constexpr inline ByteNumberingMsb0 byte_numbering_type_value_msb0{};
constexpr inline ByteNumberingMsb0 const & byte_numbering_type_value_be = byte_numbering_type_value_msb0;
constexpr inline ByteNumberingLsb0 byte_numbering_type_value_lsb0{};
constexpr inline ByteNumberingLsb0 const & byte_numbering_type_value_le = byte_numbering_type_value_lsb0;
constexpr inline ByteNumberingNone byte_numbering_type_value_none{};

template <ByteNumbering ByteNumberingV>
constexpr inline ByteNumberingType<ByteNumberingV> byte_numbering_type_v{};

} // namespace abc

#endif // ABC_BYTE_BIT_NUMBERING
