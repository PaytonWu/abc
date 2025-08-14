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

template <ByteNumbering ByteNumberingV>
template <ByteNumbering BufferByteNumberingV>
    requires(BufferByteNumberingV == ByteNumberingV)
constexpr bytes_view<ByteNumberingV>::bytes_view(byte const * first, size_type count, ByteNumberingType<BufferByteNumberingV>) noexcept : view_{ first, count }
{
}

template <ByteNumbering ByteNumberingV>
template <std::contiguous_iterator It, std::sized_sentinel_for<It> End, ByteNumbering BufferByteNumberingV>
    requires(std::same_as<std::iter_value_t<It>, abc::byte> && (!std::convertible_to<End, typename bytes_view<ByteNumberingV>::size_type>) && BufferByteNumberingV == ByteNumberingV)
constexpr bytes_view<ByteNumberingV>::bytes_view(It first, End last, ByteNumberingType<BufferByteNumberingV>) noexcept(noexcept(last - first)) : view_{ first, last }
{
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering BufferByteNumberingV>
    requires(BufferByteNumberingV == ByteNumberingV)
constexpr auto
bytes_view<ByteNumberingV>::from(abc::byte const * data, size_type size, ByteNumberingType<BufferByteNumberingV>) noexcept -> bytes_view
{
    return bytes_view{ data, size, ByteNumberingType<BufferByteNumberingV>{} };
}

template <ByteNumbering ByteNumberingV>
template <std::contiguous_iterator It, std::sized_sentinel_for<It> End, ByteNumbering BufferByteNumberingV>
    requires(std::same_as<std::iter_value_t<It>, abc::byte> && (!std::convertible_to<End, typename bytes_view<ByteNumberingV>::size_type>) && BufferByteNumberingV == ByteNumberingV)
constexpr auto
bytes_view<ByteNumberingV>::from(It first, End last, ByteNumberingType<BufferByteNumberingV>) noexcept(noexcept(last - first)) -> bytes_view
{
    return bytes_view{ first, last, ByteNumberingType<BufferByteNumberingV>{} };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::from(std::span<abc::byte const> bytes) noexcept -> bytes_view
    requires(ByteNumberingV == ByteNumbering::None)
{
    return bytes_view{ std::begin(bytes), std::end(bytes), ByteNumberingType<ByteNumberingV>{} };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering BufferByteNumberingV>
    requires(BufferByteNumberingV == ByteNumberingV)
constexpr auto
bytes_view<ByteNumberingV>::from(std::span<abc::byte const> bytes_span, ByteNumberingType<BufferByteNumberingV> bnt) noexcept -> bytes_view
{
    return bytes_view{ std::begin(bytes_span), std::end(bytes_span), bnt };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::from(std::basic_string_view<abc::byte> sv) noexcept -> bytes_view
    requires(ByteNumberingV == ByteNumbering::None)
{
    return bytes_view{ std::begin(sv), std::end(sv), ByteNumberingType<ByteNumberingV>{} };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering BufferByteNumberingV>
    requires(BufferByteNumberingV == ByteNumberingV)
constexpr auto
bytes_view<ByteNumberingV>::from(std::basic_string_view<abc::byte> sv, ByteNumberingType<BufferByteNumberingV> bnt) noexcept -> bytes_view
{
    return bytes_view{ std::begin(sv), std::end(sv), bnt };
}

template <ByteNumbering ByteNumberingV>
template <std::integral T>
    requires(ByteNumberingV != ByteNumbering::None)
constexpr auto
bytes_view<ByteNumberingV>::to() const noexcept -> T
{
    assert(view_.size() <= sizeof(T));

    T value{ 0 };
    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
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

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::begin() const noexcept -> const_iterator
{
    return view_.begin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::cbegin() const noexcept -> const_iterator
{
    return view_.cbegin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::end() const noexcept -> const_iterator
{
    return view_.end();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::cend() const noexcept -> const_iterator
{
    return view_.cend();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::rbegin() const noexcept -> const_reverse_iterator
{
    return view_.rbegin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::crbegin() const noexcept -> const_reverse_iterator
{
    return view_.crbegin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::rend() const noexcept -> const_reverse_iterator
{
    return view_.rend();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::crend() const noexcept -> const_reverse_iterator
{
    return view_.crend();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::front() const -> const_reference
{
    return view_.front();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::back() const -> const_reference
{
    return view_.back();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::operator[](size_type idx) const -> const_reference
{
    return view_[idx];
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::at(size_type pos) const -> const_reference
{
    return view_.at(pos);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::data() const noexcept -> const_pointer
{
    return view_.data();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::size() const noexcept -> size_type
{
    return view_.size();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::empty() const noexcept -> bool
{
    return view_.empty();
}

template <ByteNumbering ByteNumberingV>
template <std::size_t Count>
constexpr auto
bytes_view<ByteNumberingV>::first() const -> bytes_view
{
    return bytes_view<ByteNumberingV>{ view_.substr(0, Count) };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::first(size_type count) const -> bytes_view
{
    return bytes_view<ByteNumberingV>{ view_.substr(0, count) };
}

template <ByteNumbering ByteNumberingV>
template <std::size_t Count>
constexpr auto
bytes_view<ByteNumberingV>::last() const -> bytes_view
{
    return bytes_view<ByteNumberingV>{ view_.substr(view_.size() - Count) };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::last(size_type count) const -> bytes_view
{
    return bytes_view<ByteNumberingV>{ view_.substr(view_.size() - count) };
}

template <ByteNumbering ByteNumberingV>
template <std::size_t Offset, std::size_t Count>
constexpr auto
bytes_view<ByteNumberingV>::subview() const -> bytes_view
{
    return { view_.substr(Offset, Count) };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
bytes_view<ByteNumberingV>::subview(size_type offset, size_type count) const -> bytes_view
{
    return { view_.substr(offset, count) };
}

} // namespace abc

#endif // ABC_INCLUDE_ABC_BYTES_VIEW
