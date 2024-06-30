// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_BYTES)
#define ABC_BYTES

#pragma once

#include "bytes_decl.h"

#include "byte.h"
#include "bytes_decl.h"
#include "bytes_view.h"
#include "expected.h"
#include "utility.h"

#include <range/v3/algorithm.hpp>
#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/lexicographical_compare.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/view/reverse.hpp>

#include <algorithm>
#include <bit>
#include <cassert>
#include <cstring>
#include <span>
#include <utility>
#include <vector>

namespace abc
{

template <byte_numbering ByteNumbering>
template <byte_numbering RhsByteNumbering>
    requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr bytes<ByteNumbering>::bytes(bytes<RhsByteNumbering> && rhs) : data_{ std::move(rhs.data_) }
{
    ranges::reverse(data_);
}

template <byte_numbering ByteNumbering>
template <byte_numbering RhsByteNumbering>
    requires(RhsByteNumbering != ByteNumbering && (RhsByteNumbering == byte_numbering::none || ByteNumbering == byte_numbering::none))
constexpr bytes<ByteNumbering>::bytes(bytes<RhsByteNumbering> && rhs) noexcept : data_{ std::move(rhs.data_) }
{
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::bytes(bytes_view<ByteNumbering> const view) : data_{ std::begin(view), std::end(view) }
{
}

template <byte_numbering ByteNumbering>
template <byte_numbering RhsByteNumbering>
    requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr bytes<ByteNumbering>::bytes(bytes_view<RhsByteNumbering> const view) : data_{ std::begin(view), std::end(view) }
{
    ranges::reverse(data_);
}

template <byte_numbering ByteNumbering>
template <byte_numbering ViewByteNumbering>
    requires(ViewByteNumbering != ByteNumbering && (ViewByteNumbering == byte_numbering::none || ByteNumbering == byte_numbering::none))
constexpr bytes<ByteNumbering>::bytes(bytes_view<ViewByteNumbering> const view) : data_{ std::begin(view), std::end(view) }
{
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::bytes(std::initializer_list<value_type> const il) : data_{ il }
{
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator=(std::vector<byte> raw) noexcept -> bytes & requires(ByteNumbering == byte_numbering::none) {
    data_ = std::move(raw);
    return *this;
}

template <byte_numbering ByteNumbering>
template <std::input_iterator Iterator, byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering == ByteNumbering || SrcByteNumbering == byte_numbering::none || ByteNumbering == byte_numbering::none)
constexpr bytes<ByteNumbering>::bytes(Iterator begin, Iterator end, byte_numbering_type<SrcByteNumbering>) : data_{ begin, end }
{
}

template <byte_numbering ByteNumbering>
template <std::input_iterator Iterator, byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr bytes<ByteNumbering>::bytes(Iterator begin, Iterator end, byte_numbering_type<SrcByteNumbering>) : data_{ begin, end }
{
}

template <byte_numbering ByteNumbering>
template <std::integral T>
constexpr auto
bytes<ByteNumbering>::from(T i) -> bytes
{
    bytes bs;
    bs.reserve(sizeof(i));

    for (std::size_t j = 0; j < sizeof(T) && i != 0; ++j)
    {
        // force little endian
        bs.push_back(static_cast<byte>(i));
        i >>= 8;
    }

    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        ranges::reverse(bs);
    }

    return bs;
}

template <byte_numbering ByteNumbering>
template <byte_numbering DataByteNumbering, std::input_iterator InputIterator>
constexpr auto
bytes<ByteNumbering>::from(InputIterator begin, InputIterator end) -> bytes
{
    return bytes{ begin, end, byte_numbering_type<DataByteNumbering>{} };
}

} // namespace abc

#endif
