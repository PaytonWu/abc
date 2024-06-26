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
constexpr bytes<ByteNumbering>::bytes(bytes<RhsByteNumbering> const & rhs) : data_{rhs.data_}
{
    ranges::reverse(data_);
}

template <byte_numbering ByteNumbering>
template <byte_numbering RhsByteNumbering>
    requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr bytes<ByteNumbering>::bytes(bytes<RhsByteNumbering> && rhs) noexcept : data_{std::move(rhs.data_)}
{
    ranges::reverse(data_);
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::bytes(bytes<byte_numbering::none> const & rhs)
    requires(ByteNumbering != byte_numbering::none)
    : data_{rhs.data_}
{
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::bytes(bytes<byte_numbering::none> && rhs) noexcept
    requires(ByteNumbering != byte_numbering::none)
    : data_{std::move(rhs.data_)}
{
}

template <byte_numbering ByteNumbering>
template <std::input_iterator Iterator>
constexpr bytes<ByteNumbering>::bytes(Iterator begin, Iterator end)
    requires(ByteNumbering == byte_numbering::none)
    : data_{begin, end}
{
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::bytes(std::span<byte const> const span)
    requires(ByteNumbering == byte_numbering::none)
    : data_{std::begin(span), std::end(span)}
{
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::bytes(bytes_view<ByteNumbering> const view) : data_{std::begin(view), std::end(view)}
{
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::bytes(bytes_view_t const view)
    requires(ByteNumbering != byte_numbering::none)
    : data_{std::begin(view), std::end(view)}
{
}

} // namespace abc

#endif
