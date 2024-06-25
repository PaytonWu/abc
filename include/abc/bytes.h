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

#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/lexicographical_compare.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/view/reverse.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
#include <span>
#include <utility>
#include <vector>
#include <bit>

namespace abc
{

template <byte_numbering ByteNumbering>
template <byte_numbering RhsByteNumbering> requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr
bytes<ByteNumbering>::bytes(bytes<RhsByteNumbering> const & rhs) : data_{ rhs.data_ }
{
    ranges::reverse(data_);
}

template <byte_numbering ByteNumbering>
template <byte_numbering RhsByteNumbering> requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr
bytes<ByteNumbering>::bytes(bytes<RhsByteNumbering> && rhs) noexcept : data_{ std::move(rhs.data_) }
{
    ranges::reverse(data_);
}

}

#endif
