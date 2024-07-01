// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_BYTES)
#define ABC_BYTES

#pragma once

#include "bytes_decl.h"

#include <range/v3/algorithm.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/view/reverse.hpp>

#include <algorithm>
#include <cassert>
#include <cstring>
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
template <byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering == ByteNumbering || SrcByteNumbering == byte_numbering::none || ByteNumbering == byte_numbering::none)
constexpr bytes<ByteNumbering>::bytes(std::initializer_list<value_type> const il, byte_numbering_type<SrcByteNumbering>) : data_{ il }
{
}

template <byte_numbering ByteNumbering>
template <byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr bytes<ByteNumbering>::bytes(std::initializer_list<value_type> const il, byte_numbering_type<SrcByteNumbering>) : data_{ il }
{
    ranges::reverse(data_);
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

template <byte_numbering ByteNumbering>
template <byte_numbering ViewByteNumbering>
constexpr auto
bytes<ByteNumbering>::from(bytes_view<ViewByteNumbering> const view) -> bytes
{
    return bytes{ view };
}

template <byte_numbering ByteNumbering>
template <byte_numbering DataByteNumbering>
constexpr auto
bytes<ByteNumbering>::from(std::initializer_list<byte> il) -> bytes
{
    return bytes{ il, byte_numbering_type<DataByteNumbering>{} };
}

template <byte_numbering ByteNumbering>
template <byte_numbering RhsByteNumbering>
constexpr auto
bytes<ByteNumbering>::from(bytes<RhsByteNumbering> && rhs) -> bytes
{
    return bytes{ std::move(rhs) };
}

template <byte_numbering ByteNumbering>
template <std::integral T>
    requires(ByteNumbering != byte_numbering::none)
constexpr auto
bytes<ByteNumbering>::to() const noexcept -> T
{
    assert(data_.size() <= sizeof(T));

    T value{ 0 };
    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        for (auto const byte : data_)
        {
            value <<= 8;
            value |= static_cast<T>(byte);
        }
    }
    else
    {
        for (auto const byte : data_ | ranges::views::reverse)
        {
            value <<= 8;
            value |= static_cast<T>(byte);
        }
    }

    return value;
}

template <byte_numbering ByteNumbering>
template <byte_numbering ToByteNumbering>
    requires(ByteNumbering != ToByteNumbering)
constexpr auto
bytes<ByteNumbering>::to() const & -> bytes<ToByteNumbering>
{
    return bytes<ToByteNumbering>{ static_cast<bytes_view<ByteNumbering>>(*this) };
}

template <byte_numbering ByteNumbering>
template <byte_numbering ToByteNumbering>
    requires(ByteNumbering != ToByteNumbering)
constexpr auto
bytes<ByteNumbering>::to() && -> bytes<ToByteNumbering>
{
    return bytes<ToByteNumbering>{ std::move(*this) };
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::assign(size_type const count, byte const & value) -> void
{
    data_.assign(count, value);
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::at(size_type const pos) -> reference
{
    return data_.at(pos);
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::at(size_type const pos) const -> const_reference
{
    return data_.at(pos);
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator[](size_type const pos) -> reference
{
    return data_[pos];
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator[](size_type const pos) const -> const_reference
{
    return data_[pos];
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::front() -> reference
{
    return data_.front();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::front() const -> const_reference
{
    return data_.front();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::back() -> reference
{
    return data_.back();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::back() const -> const_reference
{
    return data_.back();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::data() noexcept -> value_type *
{
    return data_.data();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::data() const noexcept -> value_type const *
{
    return data_.data();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::begin() noexcept -> iterator
{
    return data_.begin();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::begin() const noexcept -> const_iterator
{
    return data_.begin();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::cbegin() const noexcept -> const_iterator
{
    return data_.cbegin();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::end() noexcept -> iterator
{
    return data_.end();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::end() const noexcept -> const_iterator
{
    return data_.end();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::cend() const noexcept -> const_iterator
{
    return data_.cend();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::empty() const noexcept -> bool
{
    return data_.empty();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::size() const noexcept -> size_type
{
    return data_.size();
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::resize(size_type new_size) -> void
{
    data_.resize(new_size);
}

template <byte_numbering ByteNumbering>
constexpr void
bytes<ByteNumbering>::reserve(size_type const new_cap)
{
    data_.reserve(new_cap);
}

template <byte_numbering ByteNumbering>
constexpr void
bytes<ByteNumbering>::clear() noexcept
{
    data_.clear();
}

template <byte_numbering ByteNumbering>
constexpr void
bytes<ByteNumbering>::push_back(value_type const value)
{
    data_.push_back(value);
}

template <byte_numbering ByteNumbering>
constexpr void
bytes<ByteNumbering>::pop_back()
{
    data_.pop_back();
}

template <byte_numbering ByteNumbering>
constexpr void
bytes<ByteNumbering>::swap(bytes & other) noexcept
{
    data_.swap(other.data_);
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::operator std::vector<byte> const &() const noexcept
    requires(ByteNumbering == byte_numbering::none)
{
    return data_;
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::operator std::vector<byte> &() noexcept
    requires(ByteNumbering == byte_numbering::none)
{
    return data_;
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::least_significant_byte() const -> const_reference
    requires(ByteNumbering != byte_numbering::none)
{
    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        return data_.back();
    }

    if constexpr (ByteNumbering == byte_numbering::lsb0)
    {
        return data_.front();
    }
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::least_significant_byte() -> reference
    requires(ByteNumbering != byte_numbering::none)
{
    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        return data_.back();
    }

    if constexpr (ByteNumbering == byte_numbering::lsb0)
    {
        return data_.front();
    }
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::most_significant_byte() const -> const_reference
    requires(ByteNumbering != byte_numbering::none)
{
    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        return data_.front();
    }

    if constexpr (ByteNumbering == byte_numbering::lsb0)
    {
        return data_.back();
    }
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::most_significant_byte() -> reference
    requires(ByteNumbering != byte_numbering::none)
{
    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        return data_.front();
    }

    if constexpr (ByteNumbering == byte_numbering::lsb0)
    {
        return data_.back();
    }
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::first(size_t const count) const noexcept -> bytes_view<ByteNumbering>
{
    assert(count <= data_.size());
    return bytes_view<ByteNumbering>{ std::addressof(data_[0]), count };
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::last(size_t const count) const noexcept -> bytes_view<ByteNumbering>
{
    assert(count <= data_.size());
    return bytes_view<ByteNumbering>{ std::addressof(data_[size() - count]), count };
}

template <byte_numbering ByteNumbering>
auto
bytes<ByteNumbering>::subview(size_type const pos, size_type const n) const -> expected<bytes_view<ByteNumbering>, std::error_code>
{
    if (pos >= data_.size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    return bytes_view<ByteNumbering>{ std::addressof(data_[pos]), std::min(n, size() - pos) };
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator+(bytes const & other) const -> bytes
{
    auto result = *this;
    return result += other;
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator+=(bytes const & other) -> bytes &
{
    data_.reserve(size() + other.size());
    ranges::copy(other, std::back_inserter(data_));
    return *this;
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator+(byte const other) const -> bytes
{
    auto result = *this;
    return result += other;
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator+=(byte const other) -> bytes &
{
    data_.reserve(size() + 1);
    data_.push_back(other);
    return *this;
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator+(bytes_view<ByteNumbering> const other) const -> bytes
{
    auto data = *this;
    return data += other;
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator+=(bytes_view<ByteNumbering> const other) -> bytes &
{
    data_.reserve(size() + other.size());
    ranges::copy(other, std::back_inserter(data_));
    return *this;
}

template <byte_numbering ByteNumbering>
constexpr bytes<ByteNumbering>::operator bytes_view<ByteNumbering>() const noexcept
{
    return { data(), size() };
}

template <byte_numbering ByteNumbering>
constexpr auto
bytes<ByteNumbering>::operator=(bytes_view<ByteNumbering> const view) -> bytes &
{
    data_.assign(std::begin(view), std::end(view));
    return *this;
}

[[nodiscard]] constexpr auto
operator<=>(bytes<byte_numbering::none> const & lhs, std::vector<byte> const & rhs) -> std::strong_ordering
{
    return static_cast<std::vector<byte>>(lhs) <=> rhs;
}

[[nodiscard]] constexpr auto
operator==(bytes<byte_numbering::none> const & lhs, std::vector<byte> const & rhs) -> bool
{
    return static_cast<std::vector<byte>>(lhs) == rhs;
}

template <byte_numbering ByteNumbering>
constexpr auto
operator+(byte const lhs, bytes<ByteNumbering> const & rhs) -> bytes<ByteNumbering>
{
    return bytes<ByteNumbering>{ lhs } + rhs;
}

template <byte_numbering ByteNumbering>
constexpr void
swap(bytes<ByteNumbering> & lhs, bytes<ByteNumbering> & rhs) noexcept
{
    lhs.swap(rhs);
}

constexpr auto
erase(bytes<byte_numbering::none> & c, bytes<byte_numbering::none>::value_type const value) -> bytes<byte_numbering::none>::size_type
{
    return std::erase(static_cast<std::vector<byte> &>(c), value);
}

template <typename Predictor>
constexpr auto
erase_if(bytes<byte_numbering::none> & c, Predictor predictor) -> bytes<byte_numbering::none>::size_type
{
    return std::erase_if(static_cast<std::vector<byte> &>(c), std::move(predictor));
}

} // namespace abc

#endif
