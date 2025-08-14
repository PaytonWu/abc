// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_DETAILS_BYTE_NUMBERING_FWD_DECL
#define ABC_INCLUDE_ABC_DETAILS_BYTE_NUMBERING_FWD_DECL

#pragma once

#include <bit>

namespace abc::details
{

enum class [[nodiscard]] ByteNumbering;

template <ByteNumbering ByteNubmeringV>
struct ByteNumberingType;

template <std::endian Endian>
struct EndianType;

constexpr auto operator==(ByteNumbering ByteNumbering, std::endian endian) noexcept -> bool;

} // namespace abc::details

#endif // ABC_INCLUDE_ABC_DETAILS_BYTE_NUMBERING_FWD_DECL
