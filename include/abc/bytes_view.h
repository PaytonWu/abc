// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BYTES_VIEW
#define ABC_INCLUDE_ABC_BYTES_VIEW

#pragma once

#include "bytes_view_decl.h"

#include <range/v3/range_fwd.hpp>
#include <range/v3/view/reverse.hpp>

#include <cassert>
#include <concepts>
#include <limits>
#include <string_view>

namespace abc
{

template <byte_numbering ByteNumbering>
constexpr bytes_view<ByteNumbering>::bytes_view(std::basic_string_view<abc::byte> view) noexcept requires(ByteNumbering == byte_numbering::none) : view_{ view }
{
}

template <byte_numbering ByteNumbering>
template <byte_numbering BufferByteNumbering> requires(BufferByteNumbering == ByteNumbering)
constexpr bytes_view<ByteNumbering>::bytes_view(byte const * first, size_type count, byte_numbering_type<BufferByteNumbering>) noexcept : view_{ first, count }
{
}

template <byte_numbering ByteNumbering>
template <std::contiguous_iterator It, std::sized_sentinel_for<It> End, byte_numbering BufferByteNumbering>
    requires(std::same_as<std::iter_value_t<It>, abc::byte> && (!std::convertible_to<End, typename bytes_view<ByteNumbering>::size_type>) && BufferByteNumbering == ByteNumbering)
constexpr bytes_view<ByteNumbering>::bytes_view(It first, End last, byte_numbering_type<BufferByteNumbering>) noexcept(noexcept(last - first)) : view_{ first, last }
{
}

template <byte_numbering ByteNumbering>
template <std::integral T>
    requires(ByteNumbering != byte_numbering::none)
constexpr auto
bytes_view<ByteNumbering>::to() const noexcept -> T
{
    assert(view_.size() <= sizeof(T));

    T value{ 0 };
    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        for (auto const byte : view_)
        {
            value <<= 8;
            value |= static_cast<T>(byte);
        }
    }
    else
    {
        for (auto const byte : view_ | ranges::views::reverse)
        {
            value <<= 8;
            value |= static_cast<T>(byte);
        }
    }

    return value;
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::begin() const noexcept -> const_iterator
{
    return view_.begin();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::cbegin() const noexcept -> const_iterator
{
    return view_.cbegin();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::end() const noexcept -> const_iterator
{
    return view_.end();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::cend() const noexcept -> const_iterator
{
    return view_.cend();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::rbegin() const noexcept -> const_reverse_iterator
{
    return view_.rbegin();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::crbegin() const noexcept -> const_reverse_iterator
{
    return view_.crbegin();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::rend() const noexcept -> const_reverse_iterator
{
    return view_.rend();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::crend() const noexcept -> const_reverse_iterator
{
    return view_.crend();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::front() const -> const_reference
{
    return view_.front();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::back() const -> const_reference
{
    return view_.back();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::operator[](size_type idx) const -> const_reference
{
    return view_[idx];
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::at(size_type pos) const -> const_reference
{
    return view_.at(pos);
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::data() const noexcept -> const_pointer
{
    return view_.data();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::size() const noexcept -> size_type
{
    return view_.size();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::empty() const noexcept -> bool
{
    return view_.empty();
}

template <byte_numbering ByteNumbering>
template <std::size_t Count>
constexpr auto
bytes_view<ByteNumbering>::first() const -> bytes_view<ByteNumbering>
{
    return bytes_view<ByteNumbering>{ view_.substr(0, Count) };
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::first(size_type count) const -> bytes_view<ByteNumbering>
{
    return bytes_view<ByteNumbering>{ view_.substr(0, count) };
}

template <byte_numbering ByteNumbering>
template <std::size_t Count>
constexpr auto
bytes_view<ByteNumbering>::last() const -> bytes_view<ByteNumbering>
{
    return bytes_view<ByteNumbering>{ view_.substr(view_.size() - Count) };
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::last(size_type count) const -> bytes_view<ByteNumbering>
{
    return bytes_view<ByteNumbering>{ view_.substr(view_.size() - count) };
}

template <byte_numbering ByteNumbering>
template <std::size_t Offset, std::size_t Count>
constexpr auto
bytes_view<ByteNumbering>::subview() const -> bytes_view<ByteNumbering>
{
    return { view_.substr(Offset, Count) };
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::subview(size_type offset, size_type count) const -> bytes_view<ByteNumbering>
{
    return { view_.substr(offset, count) };
}

} // namespace abc

#endif // ABC_INCLUDE_ABC_BYTES_VIEW
