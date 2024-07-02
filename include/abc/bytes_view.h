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
constexpr bytes_view<ByteNumbering>::bytes_view(std::basic_string_view<byte> view) noexcept : view_{ view }
{
}

template <byte_numbering ByteNumbering>
constexpr bytes_view<ByteNumbering>::bytes_view(bytes_view rhs, byte_numbering_type<ByteNumbering>) noexcept : view_{ rhs.view_ }
{
}

template <byte_numbering ByteNumbering>
template <byte_numbering RhsByteNumbering>
    requires(ByteNumbering != RhsByteNumbering && (ByteNumbering == byte_numbering::none || RhsByteNumbering == byte_numbering::none))
constexpr bytes_view<ByteNumbering>::bytes_view(bytes_view<RhsByteNumbering> rhs, byte_numbering_type<RhsByteNumbering>) noexcept : view_{ rhs.view_ }
{
}

template <byte_numbering ByteNumbering>
constexpr bytes_view<ByteNumbering>::bytes_view(byte const * first, size_type count) noexcept : view_{ first, count }
{
}

template <byte_numbering ByteNumbering>
template <std::contiguous_iterator It, std::sized_sentinel_for<It> End>
    requires(std::same_as<std::iter_value_t<It>, abc::byte> && (!std::convertible_to<End, typename bytes_view<ByteNumbering>::size_type>))
constexpr bytes_view<ByteNumbering>::bytes_view(It first, End last) noexcept(noexcept(last - first)) : view_{ first, last }
{
}

// template <byte_numbering ByteNumbering>
// template <typename Range, typename DRange>
//     requires((!std::is_same_v<DRange, bytes_view<ByteNumbering>>) && std::ranges::contiguous_range<Range> && std::ranges::sized_range<Range> &&
//             std::is_same_v<std::ranges::range_value_t<Range>, abc::byte> && (!std::is_convertible_v<Range, abc::byte const *>) &&
//             (!requires(DRange & d) { d.operator ::abc::bytes_view<ByteNumbering>(); }))
// constexpr bytes_view<ByteNumbering>::bytes_view(Range && r) noexcept(noexcept(std::ranges::size(r)) && noexcept(std::ranges::data(r))) : view_{ std::forward<Range>(r) }
// {
// }

// template <byte_numbering ByteNumbering>
// constexpr bytes_view<ByteNumbering>::bytes_view(std::string_view sv) noexcept : view_{ reinterpret_cast<byte const *>(sv.data()), sv.size() }
// {
// }

template <byte_numbering ByteNumbering>
template <byte_numbering RhsByteNumbering>
constexpr auto
bytes_view<ByteNumbering>::from(bytes_view<RhsByteNumbering> rhs) -> bytes_view
{
    return bytes_view{ rhs, byte_numbering_type<RhsByteNumbering>{} };
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
