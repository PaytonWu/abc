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

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberingV>
    requires(RhsByteNumberingV != ByteNumberingV && RhsByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr Bytes<ByteNumberingV>::Bytes(Bytes<RhsByteNumberingV> && rhs) : data_{ std::move(rhs.data_) }
{
    ranges::reverse(data_);
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberingV>
    requires(RhsByteNumberingV != ByteNumberingV && (RhsByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
constexpr Bytes<ByteNumberingV>::Bytes(Bytes<RhsByteNumberingV> && rhs) noexcept : data_{ std::move(rhs.data_) }
{
}

template <ByteNumbering ByteNumberingV>
constexpr Bytes<ByteNumberingV>::Bytes(bytes_view<ByteNumberingV> const view) : data_{ std::begin(view), std::end(view) }
{
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberingV>
    requires(RhsByteNumberingV != ByteNumberingV && RhsByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr Bytes<ByteNumberingV>::Bytes(bytes_view<RhsByteNumberingV> const view) : data_{ std::begin(view), std::end(view) }
{
    ranges::reverse(data_);
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ViewByteNumberingV>
    requires(ViewByteNumberingV != ByteNumberingV && (ViewByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
constexpr Bytes<ByteNumberingV>::Bytes(bytes_view<ViewByteNumberingV> const view) : data_{ std::begin(view), std::end(view) }
{
}

template <ByteNumbering ByteNumberingV>
constexpr Bytes<ByteNumberingV>::Bytes(std::initializer_list<value_type> const il) : data_{ il }
{
}

template <ByteNumbering ByteNumberingV>
template <std::input_iterator Iterator, ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV == ByteNumberingV || SrcByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None)
constexpr Bytes<ByteNumberingV>::Bytes(Iterator begin, Iterator end, ByteNumberingType<SrcByteNumberingV>) : data_{ begin, end }
{
}

template <ByteNumbering ByteNumberingV>
template <std::input_iterator Iterator, ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr Bytes<ByteNumberingV>::Bytes(Iterator begin, Iterator end, ByteNumberingType<SrcByteNumberingV>) : data_{ begin, end }
{
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV == ByteNumberingV || SrcByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None)
constexpr Bytes<ByteNumberingV>::Bytes(std::initializer_list<value_type> const il, ByteNumberingType<SrcByteNumberingV>) : data_{ il }
{
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr Bytes<ByteNumberingV>::Bytes(std::initializer_list<value_type> const il, ByteNumberingType<SrcByteNumberingV>) : data_{ il }
{
    ranges::reverse(data_);
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberingV>
    requires(RhsByteNumberingV != ByteNumberingV && RhsByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr auto
Bytes<ByteNumberingV>::operator=(Bytes<RhsByteNumberingV> && rhs) -> Bytes &
{
    data_ = std::move(rhs.data_);
    ranges::reverse(data_);
    return *this;
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberV>
    requires(RhsByteNumberV != ByteNumberingV && (RhsByteNumberV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
constexpr auto
Bytes<ByteNumberingV>::operator=(Bytes<RhsByteNumberV> && rhs) noexcept -> Bytes &
{
    data_ = std::move(rhs.data_);
    return *this;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::operator=(bytes_view<ByteNumberingV> view) -> Bytes &
{
    data_.assign(std::begin(view), std::end(view));
    return *this;
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ViewByteNumberingV>
    requires(ViewByteNumberingV != ByteNumberingV && ViewByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr auto
Bytes<ByteNumberingV>::operator=(bytes_view<ViewByteNumberingV> view) -> Bytes &
{
    data_.assign(std::begin(view), std::end(view));
    ranges::reverse(data_);
    return *this;
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ViewByteNumberingV>
    requires(ViewByteNumberingV != ByteNumberingV && (ViewByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
constexpr auto
Bytes<ByteNumberingV>::operator=(bytes_view<ViewByteNumberingV> view) -> Bytes &
{
    data_.assign(std::begin(view), std::end(view));
    return *this;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::operator=(std::initializer_list<value_type> const il) -> Bytes &
{
    data_ = il;
    return *this;
}

template <ByteNumbering ByteNumberingV>
template <std::integral T>
constexpr auto
Bytes<ByteNumberingV>::from(T i) -> Bytes
{
    Bytes bs;
    bs.reserve(sizeof(i));

    for (std::size_t j = 0; j < sizeof(T) && i != 0; ++j)
    {
        // force little endian
        bs.push_back(static_cast<byte>(i));
        i >>= 8;
    }

    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
    {
        ranges::reverse(bs);
    }

    return bs;
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering DataByteNumberingV, std::input_iterator InputIterator>
constexpr auto
Bytes<ByteNumberingV>::from(InputIterator begin, InputIterator end) -> Bytes
{
    return Bytes{ begin, end, ByteNumberingType<DataByteNumberingV>{} };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ViewByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::from(bytes_view<ViewByteNumberingV> const view) -> Bytes
{
    return Bytes{ view };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering DataByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::from(std::initializer_list<byte> il) -> Bytes
{
    return Bytes{ il, ByteNumberingType<DataByteNumberingV>{} };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::from(Bytes<RhsByteNumberingV> && rhs) -> Bytes
{
    return Bytes{ std::move(rhs) };
}

template <ByteNumbering ByteNumberingV>
template <std::integral T>
    requires(ByteNumberingV != ByteNumbering::None)
constexpr auto
Bytes<ByteNumberingV>::to() const noexcept -> T
{
    assert(data_.size() <= sizeof(T));

    T value{ 0 };
    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
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

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ToByteNumberingV>
    requires(ByteNumberingV != ToByteNumberingV)
constexpr auto
Bytes<ByteNumberingV>::to() const & -> Bytes<ToByteNumberingV>
{
    return Bytes<ToByteNumberingV>{ static_cast<bytes_view<ByteNumberingV>>(*this) };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ToByteNumberingV>
    requires(ByteNumberingV != ToByteNumberingV)
constexpr auto
Bytes<ByteNumberingV>::to() && -> Bytes<ToByteNumberingV>
{
    return Bytes<ToByteNumberingV>{ std::move(*this) };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::assign(size_type const count, byte const & value) -> void
{
    data_.assign(count, value);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::at(size_type const pos) -> reference
{
    return data_.at(pos);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::at(size_type const pos) const -> const_reference
{
    return data_.at(pos);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::operator[](size_type const pos) -> reference
{
    return data_[pos];
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::operator[](size_type const pos) const -> const_reference
{
    return data_[pos];
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::front() -> reference
{
    return data_.front();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::front() const -> const_reference
{
    return data_.front();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::back() -> reference
{
    return data_.back();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::back() const -> const_reference
{
    return data_.back();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::data() noexcept -> value_type *
{
    return data_.data();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::data() const noexcept -> value_type const *
{
    return data_.data();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::begin() noexcept -> iterator
{
    return data_.begin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::begin() const noexcept -> const_iterator
{
    return data_.begin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::cbegin() const noexcept -> const_iterator
{
    return data_.cbegin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::end() noexcept -> iterator
{
    return data_.end();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::end() const noexcept -> const_iterator
{
    return data_.end();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::cend() const noexcept -> const_iterator
{
    return data_.cend();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::empty() const noexcept -> bool
{
    return data_.empty();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::size() const noexcept -> size_type
{
    return data_.size();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::resize(size_type new_size) -> void
{
    data_.resize(new_size);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::reserve(size_type const new_cap) -> void
{
    data_.reserve(new_cap);
}

template <ByteNumbering ByteNumberingV>
constexpr void
Bytes<ByteNumberingV>::clear() noexcept
{
    data_.clear();
}

template <ByteNumbering ByteNumberingV>
constexpr void
Bytes<ByteNumberingV>::push_back(value_type const value)
{
    data_.push_back(value);
}

template <ByteNumbering ByteNumberingV>
constexpr void
Bytes<ByteNumberingV>::pop_back()
{
    data_.pop_back();
}

template <ByteNumbering ByteNumberingV>
constexpr void
Bytes<ByteNumberingV>::swap(Bytes & other) noexcept
{
    data_.swap(other.data_);
}

template <ByteNumbering ByteNumberingV>
constexpr Bytes<ByteNumberingV>::operator std::vector<byte> const &() const noexcept
    requires(ByteNumberingV == ByteNumbering::None)
{
    return data_;
}

template <ByteNumbering ByteNumberingV>
constexpr Bytes<ByteNumberingV>::operator std::vector<byte> &() noexcept
    requires(ByteNumberingV == ByteNumbering::None)
{
    return data_;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::least_significant_byte() const -> const_reference
    requires(ByteNumberingV != ByteNumbering::None)
{
    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
    {
        return data_.back();
    }

    if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
    {
        return data_.front();
    }

    unreachable();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::least_significant_byte() -> reference
    requires(ByteNumberingV != ByteNumbering::None)
{
    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
    {
        return data_.back();
    }

    if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
    {
        return data_.front();
    }

    unreachable();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::most_significant_byte() const -> const_reference
    requires(ByteNumberingV != ByteNumbering::None)
{
    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
    {
        return data_.front();
    }

    if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
    {
        return data_.back();
    }

    unreachable();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::most_significant_byte() -> reference
    requires(ByteNumberingV != ByteNumbering::None)
{
    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
    {
        return data_.front();
    }

    if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
    {
        return data_.back();
    }

    unreachable();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::first(size_t const count) const noexcept -> bytes_view<ByteNumberingV>
{
    assert(count <= data_.size());
    return bytes_view<ByteNumberingV>{ std::addressof(data_[0]), count };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::last(size_t const count) const noexcept -> bytes_view<ByteNumberingV>
{
    assert(count <= data_.size());
    return bytes_view<ByteNumberingV>::from(std::addressof(data_[size() - count]), count, ByteNumberingType<ByteNumberingV>{});
}

template <ByteNumbering ByteNumberingV>
auto
Bytes<ByteNumberingV>::subview(size_type const pos, size_type const n) const -> expected<bytes_view<ByteNumberingV>, std::error_code>
{
    if (pos >= data_.size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    return bytes_view<ByteNumberingV>{ std::addressof(data_[pos]), std::min(n, size() - pos) };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::view() const noexcept -> bytes_view<ByteNumberingV>
{
    return static_cast<bytes_view<ByteNumberingV>>(*this);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::operator+(byte const other) const -> Bytes
{
    auto result = *this;
    return result += other;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::operator+=(byte const other) -> Bytes &
{
    data_.reserve(size() + 1);
    data_.push_back(other);
    return *this;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::operator+(bytes_view<ByteNumberingV> const other) const -> Bytes
{
    auto data = *this;
    return data += other;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
Bytes<ByteNumberingV>::operator+=(bytes_view<ByteNumberingV> const other) -> Bytes &
{
    data_.reserve(size() + other.size());
    ranges::copy(other, std::back_inserter(data_));
    return *this;
}

template <ByteNumbering ByteNumberingV>
constexpr Bytes<ByteNumberingV>::operator bytes_view<ByteNumberingV>() const noexcept
{
    return bytes_view<ByteNumberingV>::from(data(), size(), ByteNumberingType<ByteNumberingV>{});
}

[[nodiscard]] constexpr auto
operator<=>(Bytes<ByteNumbering::None> const & lhs, std::vector<byte> const & rhs) -> std::strong_ordering
{
    return static_cast<std::vector<byte>>(lhs) <=> rhs;
}

[[nodiscard]] constexpr auto
operator==(Bytes<ByteNumbering::None> const & lhs, std::vector<byte> const & rhs) -> bool
{
    return static_cast<std::vector<byte>>(lhs) == rhs;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
operator+(byte const lhs, bytes_view<ByteNumberingV> const & rhs) -> Bytes<ByteNumberingV>
{
    return Bytes<ByteNumberingV>{ lhs } + rhs;
}

template <ByteNumbering ByteNumberingV>
constexpr void
swap(Bytes<ByteNumberingV> & lhs, Bytes<ByteNumberingV> & rhs) noexcept
{
    lhs.swap(rhs);
}

constexpr auto
erase(Bytes<ByteNumbering::None> & c, Bytes<ByteNumbering::None>::value_type const value) -> Bytes<ByteNumbering::None>::size_type
{
    return std::erase(static_cast<std::vector<byte> &>(c), value);
}

template <typename Predictor>
constexpr auto
erase_if(Bytes<ByteNumbering::None> & c, Predictor predictor) -> Bytes<ByteNumbering::None>::size_type
{
    return std::erase_if(static_cast<std::vector<byte> &>(c), std::move(predictor));
}

} // namespace abc

#endif
