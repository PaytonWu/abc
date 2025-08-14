// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BASIC_BYTES
#define ABC_INCLUDE_ABC_BASIC_BYTES

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
constexpr BasicBytes<ByteNumberingV>::BasicBytes(BasicBytes<RhsByteNumberingV> && rhs) : data_{ std::move(rhs.data_) }
{
    ranges::reverse(data_);
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberingV>
    requires(RhsByteNumberingV != ByteNumberingV && (RhsByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
constexpr BasicBytes<ByteNumberingV>::BasicBytes(BasicBytes<RhsByteNumberingV> && rhs) noexcept : data_{ std::move(rhs.data_) }
{
}

template <ByteNumbering ByteNumberingV>
constexpr BasicBytes<ByteNumberingV>::BasicBytes(bytes_view<ByteNumberingV> const view) : data_{ std::begin(view), std::end(view) }
{
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberingV>
    requires(RhsByteNumberingV != ByteNumberingV && RhsByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr BasicBytes<ByteNumberingV>::BasicBytes(bytes_view<RhsByteNumberingV> const view) : data_{ std::begin(view), std::end(view) }
{
    ranges::reverse(data_);
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ViewByteNumberingV>
    requires(ViewByteNumberingV != ByteNumberingV && (ViewByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
constexpr BasicBytes<ByteNumberingV>::BasicBytes(bytes_view<ViewByteNumberingV> const view) : data_{ std::begin(view), std::end(view) }
{
}

template <ByteNumbering ByteNumberingV>
constexpr BasicBytes<ByteNumberingV>::BasicBytes(std::string_view str)
    requires(ByteNumberingV == ByteNumbering::None)
    : data_{ str.begin(), str.end() }
{
}

template <ByteNumbering ByteNumberingV>
constexpr BasicBytes<ByteNumberingV>::BasicBytes(std::initializer_list<value_type> const il) : data_{ il }
{
}

template <ByteNumbering ByteNumberingV>
template <std::input_iterator Iterator, ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV == ByteNumberingV || SrcByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None)
constexpr BasicBytes<ByteNumberingV>::BasicBytes(Iterator begin, Iterator end, ByteNumberingType<SrcByteNumberingV>) : data_{ begin, end }
{
}

template <ByteNumbering ByteNumberingV>
template <std::input_iterator Iterator, ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr BasicBytes<ByteNumberingV>::BasicBytes(Iterator begin, Iterator end, ByteNumberingType<SrcByteNumberingV>) : data_{ begin, end }
{
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV == ByteNumberingV || SrcByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None)
constexpr BasicBytes<ByteNumberingV>::BasicBytes(std::initializer_list<value_type> const il, ByteNumberingType<SrcByteNumberingV>) : data_{ il }
{
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr BasicBytes<ByteNumberingV>::BasicBytes(std::initializer_list<value_type> const il, ByteNumberingType<SrcByteNumberingV>) : data_{ il }
{
    ranges::reverse(data_);
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberingV>
    requires(RhsByteNumberingV != ByteNumberingV && RhsByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr auto
BasicBytes<ByteNumberingV>::operator=(BasicBytes<RhsByteNumberingV> && rhs) -> BasicBytes &
{
    data_ = std::move(rhs.data_);
    ranges::reverse(data_);
    return *this;
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberV>
    requires(RhsByteNumberV != ByteNumberingV && (RhsByteNumberV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
constexpr auto
BasicBytes<ByteNumberingV>::operator=(BasicBytes<RhsByteNumberV> && rhs) noexcept -> BasicBytes &
{
    data_ = std::move(rhs.data_);
    return *this;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::operator=(bytes_view<ByteNumberingV> view) -> BasicBytes &
{
    data_.assign(std::begin(view), std::end(view));
    return *this;
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ViewByteNumberingV>
    requires(ViewByteNumberingV != ByteNumberingV && ViewByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr auto
BasicBytes<ByteNumberingV>::operator=(bytes_view<ViewByteNumberingV> view) -> BasicBytes &
{
    data_.assign(std::begin(view), std::end(view));
    ranges::reverse(data_);
    return *this;
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ViewByteNumberingV>
    requires(ViewByteNumberingV != ByteNumberingV && (ViewByteNumberingV == ByteNumbering::None || ByteNumberingV == ByteNumbering::None))
constexpr auto
BasicBytes<ByteNumberingV>::operator=(bytes_view<ViewByteNumberingV> view) -> BasicBytes &
{
    data_.assign(std::begin(view), std::end(view));
    return *this;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::operator=(std::initializer_list<value_type> const il) -> BasicBytes &
{
    data_ = il;
    return *this;
}

template <ByteNumbering ByteNumberingV>
template <std::integral T>
constexpr auto
BasicBytes<ByteNumberingV>::from(T i) -> BasicBytes
{
    BasicBytes bs;
    bs.reserve(sizeof(i));

    for (std::size_t j = 0; j < sizeof(T) && i != 0; ++j)
    {
        // force little endian
        bs.push_back(static_cast<byte_t>(i));
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
BasicBytes<ByteNumberingV>::from(InputIterator begin, InputIterator end) -> BasicBytes
{
    return BasicBytes{ begin, end, ByteNumberingType<DataByteNumberingV>{} };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ViewByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::from(bytes_view<ViewByteNumberingV> const view) -> BasicBytes
{
    return BasicBytes{ view };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering DataByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::from(std::initializer_list<byte_t> il) -> BasicBytes
{
    return BasicBytes{ il, ByteNumberingType<DataByteNumberingV>{} };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering RhsByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::from(BasicBytes<RhsByteNumberingV> && rhs) -> BasicBytes
{
    return BasicBytes{ std::move(rhs) };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::from(std::string_view str) -> BasicBytes
    requires(ByteNumberingV == ByteNumbering::None)
{
    return BasicBytes{ str };
}

template <ByteNumbering ByteNumberingV>
template <std::integral T>
    requires(ByteNumberingV != ByteNumbering::None)
constexpr auto
BasicBytes<ByteNumberingV>::to() const noexcept -> T
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
BasicBytes<ByteNumberingV>::to() const & -> BasicBytes<ToByteNumberingV>
{
    return BasicBytes<ToByteNumberingV>{ static_cast<bytes_view<ByteNumberingV>>(*this) };
}

template <ByteNumbering ByteNumberingV>
template <ByteNumbering ToByteNumberingV>
    requires(ByteNumberingV != ToByteNumberingV)
constexpr auto
BasicBytes<ByteNumberingV>::to() && -> BasicBytes<ToByteNumberingV>
{
    return BasicBytes<ToByteNumberingV>{ std::move(*this) };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::assign(size_type const count, byte_t const & value) -> void
{
    data_.assign(count, value);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::at(size_type const pos) -> reference
{
    return data_.at(pos);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::at(size_type const pos) const -> const_reference
{
    return data_.at(pos);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::operator[](size_type const pos) -> reference
{
    return data_[pos];
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::operator[](size_type const pos) const -> const_reference
{
    return data_[pos];
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::front() -> reference
{
    return data_.front();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::front() const -> const_reference
{
    return data_.front();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::back() -> reference
{
    return data_.back();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::back() const -> const_reference
{
    return data_.back();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::data() noexcept -> value_type *
{
    return data_.data();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::data() const noexcept -> value_type const *
{
    return data_.data();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::begin() noexcept -> iterator
{
    return data_.begin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::begin() const noexcept -> const_iterator
{
    return data_.begin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::cbegin() const noexcept -> const_iterator
{
    return data_.cbegin();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::end() noexcept -> iterator
{
    return data_.end();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::end() const noexcept -> const_iterator
{
    return data_.end();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::cend() const noexcept -> const_iterator
{
    return data_.cend();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::empty() const noexcept -> bool
{
    return data_.empty();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::size() const noexcept -> size_type
{
    return data_.size();
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::resize(size_type new_size) -> void
{
    data_.resize(new_size);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::reserve(size_type const new_cap) -> void
{
    data_.reserve(new_cap);
}

template <ByteNumbering ByteNumberingV>
constexpr void
BasicBytes<ByteNumberingV>::clear() noexcept
{
    data_.clear();
}

template <ByteNumbering ByteNumberingV>
constexpr void
BasicBytes<ByteNumberingV>::push_back(value_type const value)
{
    data_.push_back(value);
}

template <ByteNumbering ByteNumberingV>
constexpr void
BasicBytes<ByteNumberingV>::pop_back()
{
    data_.pop_back();
}

template <ByteNumbering ByteNumberingV>
constexpr void
BasicBytes<ByteNumberingV>::swap(BasicBytes & other) noexcept
{
    data_.swap(other.data_);
}

template <ByteNumbering ByteNumberingV>
constexpr BasicBytes<ByteNumberingV>::operator std::vector<byte_t> const &() const noexcept
    requires(ByteNumberingV == ByteNumbering::None)
{
    return data_;
}

template <ByteNumbering ByteNumberingV>
constexpr BasicBytes<ByteNumberingV>::operator std::vector<byte_t> &() noexcept
    requires(ByteNumberingV == ByteNumbering::None)
{
    return data_;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::least_significant_byte() const -> const_reference
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
BasicBytes<ByteNumberingV>::least_significant_byte() -> reference
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
BasicBytes<ByteNumberingV>::most_significant_byte() const -> const_reference
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
BasicBytes<ByteNumberingV>::most_significant_byte() -> reference
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
BasicBytes<ByteNumberingV>::first(size_t const count) const noexcept -> bytes_view<ByteNumberingV>
{
    assert(count <= data_.size());
    return bytes_view<ByteNumberingV>{ std::addressof(data_[0]), count };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::last(size_t const count) const noexcept -> bytes_view<ByteNumberingV>
{
    assert(count <= data_.size());
    return bytes_view<ByteNumberingV>::from(std::addressof(data_[size() - count]), count, ByteNumberingType<ByteNumberingV>{});
}

template <ByteNumbering ByteNumberingV>
auto
BasicBytes<ByteNumberingV>::subview(size_type const pos, size_type const n) const -> expected<bytes_view<ByteNumberingV>, std::error_code>
{
    if (pos >= data_.size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    return bytes_view<ByteNumberingV>{ std::addressof(data_[pos]), std::min(n, size() - pos) };
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::view() const noexcept -> bytes_view<ByteNumberingV>
{
    return static_cast<bytes_view<ByteNumberingV>>(*this);
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::operator+(byte_t const other) const -> BasicBytes
{
    auto result = *this;
    return result += other;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::operator+=(byte_t const other) -> BasicBytes &
{
    data_.reserve(size() + 1);
    data_.push_back(other);
    return *this;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::operator+(bytes_view<ByteNumberingV> const other) const -> BasicBytes
{
    auto data = *this;
    return data += other;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
BasicBytes<ByteNumberingV>::operator+=(bytes_view<ByteNumberingV> const other) -> BasicBytes &
{
    data_.reserve(size() + other.size());
    ranges::copy(other, std::back_inserter(data_));
    return *this;
}

template <ByteNumbering ByteNumberingV>
constexpr BasicBytes<ByteNumberingV>::operator bytes_view<ByteNumberingV>() const noexcept
{
    return bytes_view<ByteNumberingV>::from(data(), size(), ByteNumberingType<ByteNumberingV>{});
}

[[nodiscard]] constexpr auto
operator<=>(BasicBytes<ByteNumbering::None> const & lhs, std::vector<byte_t> const & rhs) -> std::strong_ordering
{
    return static_cast<std::vector<byte_t>>(lhs) <=> rhs;
}

[[nodiscard]] constexpr auto
operator==(BasicBytes<ByteNumbering::None> const & lhs, std::vector<byte_t> const & rhs) -> bool
{
    return static_cast<std::vector<byte_t>>(lhs) == rhs;
}

template <ByteNumbering ByteNumberingV>
constexpr auto
operator+(byte_t const lhs, bytes_view<ByteNumberingV> const & rhs) -> BasicBytes<ByteNumberingV>
{
    return BasicBytes<ByteNumberingV>{ lhs } + rhs;
}

template <ByteNumbering ByteNumberingV>
constexpr void
swap(BasicBytes<ByteNumberingV> & lhs, BasicBytes<ByteNumberingV> & rhs) noexcept
{
    lhs.swap(rhs);
}

constexpr auto
erase(BasicBytes<ByteNumbering::None> & c, BasicBytes<ByteNumbering::None>::value_type const value) -> BasicBytes<ByteNumbering::None>::size_type
{
    return std::erase(static_cast<std::vector<byte_t> &>(c), value);
}

template <typename Predictor>
constexpr auto
erase_if(BasicBytes<ByteNumbering::None> & c, Predictor predictor) -> BasicBytes<ByteNumbering::None>::size_type
{
    return std::erase_if(static_cast<std::vector<byte_t> &>(c), std::move(predictor));
}

} // namespace abc

#endif // ABC_INCLUDE_ABC_BASIC_BYTES
