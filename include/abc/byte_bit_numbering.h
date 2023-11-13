// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_BYTE_BIT_NUMBERING
#define ABC_BYTE_BIT_NUMBERING

#pragma once

#include "abc/details/config.h"

#include <bit>

namespace abc {

enum class byte_numbering {
    none,
    lsb0,
    msb0
};

enum class bit_numbering {
    invalid,
    lsb0,
    msb0
};

template <byte_numbering ByteNubmering>
struct byte_numbering_type;

template <>
struct byte_numbering_type<byte_numbering::none> {
    constexpr static byte_numbering byte_numbering_value = byte_numbering::none;
};

template <>
struct byte_numbering_type<byte_numbering::msb0> {
    constexpr static byte_numbering byte_numbering_value = byte_numbering::msb0;
    constexpr static std::endian endian_value = std::endian::big;
};

template <>
struct byte_numbering_type<byte_numbering::lsb0> {
    constexpr static byte_numbering byte_numbering_value = byte_numbering::lsb0;
    constexpr static std::endian endian_value = std::endian::little;
};

template <std::endian Endian>
struct endian_type;

template <>
struct endian_type<std::endian::big> {
    constexpr static std::endian endian_value = std::endian::big;
    constexpr static byte_numbering byte_numbering_value = byte_numbering::msb0;
};

template <>
struct endian_type<std::endian::little> {
    constexpr static std::endian endian_value = std::endian::little;
    constexpr static byte_numbering byte_numbering_value = byte_numbering::lsb0;
};

constexpr auto operator==(byte_numbering byte_numbering, std::endian endian) noexcept -> bool {
    return (byte_numbering == byte_numbering::msb0 && endian == std::endian::big) || (byte_numbering == byte_numbering::lsb0 && endian == std::endian::little);
}

}

#endif //ABC_BYTE_BIT_NUMBERING
