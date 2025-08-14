// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_FIXED_BYTES)
#define ABC_FIXED_BYTES

#pragma once

#include "bytes.h"
#include "expected.h"
#include "fixed_bytes_decl.h"

#include <range/v3/algorithm/copy.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/subrange.hpp>
#include <range/v3/view/transform.hpp>

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <span>

namespace abc
{

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV == ByteNumberingV)
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes(std::array<byte, N> const & src, ByteNumberingType<SrcByteNumberingV>) : data_{ src }
{
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes(std::array<byte, N> const & src, ByteNumberingType<SrcByteNumberingV>)
{
    ranges::copy(src | ranges::views::reverse, std::begin(data_));
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV == ByteNumberingV)
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes(std::array<std::byte, N> const & src, ByteNumberingType<SrcByteNumberingV>)
{
    ranges::copy(src | ranges::views::transform([](auto const byte) { return std::to_integer<byte>(byte); }), data_.begin());
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes(std::array<std::byte, N> const & src, ByteNumberingType<SrcByteNumberingV>)
{
    ranges::copy(src | ranges::views::reverse | ranges::views::transform([](auto const b) { return std::to_integer<byte>(b); }), data_.begin());
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV == ByteNumberingV)
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes(bytes_view<SrcByteNumberingV> const src /*, ByteNumberingType<SrcByteNumberingV>*/) : fixed_bytes{}
{
    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
    {
        if (src.size() == N)
        {
            ranges::copy(src, std::begin(data_));
        }
        else if (src.size() > N)
        {
            auto const bytes_begin = std::next(std::begin(src), static_cast<difference_type>(src.size() - N));
            ranges::copy(bytes_begin, std::end(src), std::begin(data_));
        }
        else
        {
            assert(src.size() < N);
            auto const data_begin = std::next(std::begin(data_), static_cast<difference_type>(N - src.size()));
            ranges::copy(src, data_begin);
        }
    }
    else
    {
        auto const end_pos = std::min(src.size(), N);
        ranges::copy(std::begin(src), std::next(std::begin(src), static_cast<difference_type>(end_pos)), std::begin(data_));
    }
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes(bytes_view<SrcByteNumberingV> const src /*, ByteNumberingType<SrcByteNumberingV>*/) : fixed_bytes{}
{
    if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
    {
        auto const end_pos = std::min(src.size(), N);
        auto src_reverse_view = src | ranges::views::reverse;
        ranges::copy(std::begin(src_reverse_view), std::next(std::begin(src_reverse_view), static_cast<difference_type>(end_pos)), std::begin(data_));
    }
    else
    {
        assert(ByteNumberingV == ByteNumbering::Msb0);
        auto const end_pos = std::min(src.size(), N);
        ranges::copy(std::begin(src), std::next(std::begin(src), static_cast<difference_type>(end_pos)), std::begin(data_));
        ranges::reverse(data_);
    }
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering SrcByteNumberingV>
    requires(SrcByteNumberingV != ByteNumberingV && SrcByteNumberingV == ByteNumbering::None)
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes(bytes_view<SrcByteNumberingV> const src) : fixed_bytes{}
{
    assert(src.size() == N);
    ranges::copy(src, std::begin(data_));
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes()
{
    data_.fill(0);
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes(std::unsigned_integral auto const value)
    requires((ByteNumberingV == ByteNumbering::Lsb0) || (ByteNumberingV == ByteNumbering::Msb0))
    : fixed_bytes{}
{
    if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
    {
        to_little_endian(value, data_);
    }

    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
    {
        to_big_endian(value, data_);
    }
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering ByteNumberingRhsV>
    requires(ByteNumberingRhsV != ByteNumbering::None && ByteNumberingRhsV != ByteNumberingV)
constexpr fixed_bytes<N, ByteNumberingV>::fixed_bytes(fixed_bytes<N, ByteNumberingRhsV> const & rhs)
{
    ranges::copy(rhs | ranges::views::reverse, data_.begin());
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering DataByteNumberingV>
auto
fixed_bytes<N, ByteNumberingV>::from(std::array<byte, N> const & data) -> expected<fixed_bytes, std::error_code>
{
    if constexpr (DataByteNumberingV == ByteNumberingV || (DataByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None))
    {
        return fixed_bytes{ data, ByteNumberingType<DataByteNumberingV>{} };
    }
    else
    {
        return make_unexpected(make_error_code(std::errc::invalid_argument));
    }
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering DataByteNumberingV>
    requires(DataByteNumberingV == ByteNumberingV) || (DataByteNumberingV != ByteNumberingV && DataByteNumberingV != ByteNumbering::None && ByteNumberingV != ByteNumbering::None)
auto fixed_bytes<N, ByteNumberingV>::from(bytes_view<DataByteNumberingV> const data) -> expected<fixed_bytes, std::error_code>
{
    return fixed_bytes{ data };
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <ByteNumbering DataByteNumberingV>
    requires(DataByteNumberingV != ByteNumberingV && DataByteNumberingV == ByteNumbering::None)
auto
fixed_bytes<N, ByteNumberingV>::from(bytes_view<DataByteNumberingV> const data) -> expected<fixed_bytes, std::error_code>
{
    if (data.size() != N)
    {
        return make_unexpected(make_error_code(std::errc::invalid_argument));
    }

    return fixed_bytes{ data };
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator[](size_t const index) const noexcept -> byte
{
    return data_[index];
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator[](size_t const index) noexcept -> byte &
{
    return data_[index];
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr fixed_bytes<N, ByteNumberingV>::operator bytes_view<ByteNumberingV>() const noexcept
{
    return abc::bytes_view<ByteNumberingV>::from(data_.data(), data_.size(), ByteNumberingType<ByteNumberingV>{});
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::front() const noexcept -> const_reference
{
    return data_.front();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::front() noexcept -> reference
{
    return data_.front();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::back() const noexcept -> const_reference
{
    return data_.back();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::back() noexcept -> reference
{
    return data_.back();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::begin() noexcept -> iterator
{
    return data_.begin();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::begin() const noexcept -> const_iterator
{
    return data_.begin();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::cbegin() const noexcept -> const_iterator
{
    return data_.cbegin();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::end() noexcept -> iterator
{
    return data_.end();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::end() const noexcept -> const_iterator
{
    return data_.end();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::cend() const noexcept -> const_iterator
{
    return data_.cend();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::rbegin() noexcept -> reverse_iterator
{
    return data_.rbegin();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::rbegin() const noexcept -> const_reverse_iterator
{
    return data_.rbegin();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::crbegin() const noexcept -> const_reverse_iterator
{
    return data_.crbegin();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::rend() noexcept -> reverse_iterator
{
    return data_.rend();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::rend() const noexcept -> const_reverse_iterator
{
    return data_.rend();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::crend() const noexcept -> const_reverse_iterator
{
    return data_.crend();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::data() noexcept -> pointer
{
    return data_.data();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::data() const noexcept -> const_pointer
{
    return data_.data();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::empty() const noexcept -> bool
{
    return data_.empty();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::size() const noexcept -> size_type
{
    return data_.size();
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::fill(byte const value) noexcept -> void
{
    data_.fill(value);
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::byte_numbering() noexcept -> abc::ByteNumbering
{
    return ByteNumberingV;
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <std::unsigned_integral T>
[[nodiscard]] constexpr auto
fixed_bytes<N, ByteNumberingV>::to() const noexcept -> T
{
    T result{};
    if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
    {
        for (auto i = 0u; i < std::min(sizeof(T), data_.size()); ++i)
        {
            result |= static_cast<T>(data_[i]) << (i * 8);
        }
    }

    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
    {
        for (auto i = 0u; i < std::min(sizeof(T), data_.size()); ++i)
        {
            result |= static_cast<T>(data_[data_.size() - i - 1]) << (i * 8);
        }
    }
    return result;
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::zero() const noexcept -> bool
{
    return std::ranges::all_of(data_, [](auto const byte) { return byte == 0; });
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator^=(fixed_bytes const & other) -> fixed_bytes &
{
    for (auto i = 0u; i < N; ++i)
    {
        data_[i] ^= other.data_[i];
    }
    return *this;
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator^(fixed_bytes const & other) const -> fixed_bytes
{
    return fixed_bytes{ *this } ^= other;
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator|=(fixed_bytes const & other) -> fixed_bytes &
{
    for (auto i = 0u; i < N; ++i)
    {
        data_[i] |= other.data_[i];
    }
    return *this;
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator|(fixed_bytes const & other) const -> fixed_bytes
{
    return fixed_bytes{ *this } |= other;
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator&=(fixed_bytes const & other) -> fixed_bytes &
{
    for (auto i = 0u; i < N; ++i)
    {
        data_[i] &= other.data_[i];
    }
    return *this;
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator&(fixed_bytes const & other) const -> fixed_bytes
{
    return fixed_bytes{ *this } &= other;
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator~() const -> fixed_bytes
{
    fixed_bytes ret;
    for (auto i = 0u; i < N; ++i)
    {
        ret[i] = ~data_[i];
    }
    return ret;
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::operator++() -> fixed_bytes &
{
    if constexpr (ByteNumberingV == ByteNumbering::Msb0)
    {
        for (auto i = N; i > 0 && !++data_[--i];)
        {
        }
        return *this;
    }

    if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
    {
        for (auto i = 0u; i < N && !++data_[i++];)
        {
        }
        return *this;
    }
}

template <std::size_t N, ByteNumbering ByteNumberingV>
constexpr auto
fixed_bytes<N, ByteNumberingV>::clear() -> void
{
    data_.fill(0);
}

template <std::size_t N, ByteNumbering ByteNumberingV>
auto
fixed_bytes<N, ByteNumberingV>::subbytes(size_type pos, size_type n) const -> expected<Bytes<ByteNumberingV>, std::error_code>
{
    if (pos >= size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    size_type offset = (n < size() - pos) ? n : size() - pos;

    auto start = std::next(std::begin(data_), pos);
    auto end = std::next(start, offset);

    return abc::Bytes<ByteNumberingV>::template from<ByteNumberingV>(start, end);
}

template <std::size_t N, ByteNumbering ByteNumberingV>
auto
fixed_bytes<N, ByteNumberingV>::subview(size_type pos, size_type n) const & -> expected<bytes_view<ByteNumberingV>, std::error_code>
{
    if (pos >= size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    size_type offset = (n < size() - pos) ? n : size() - pos;

    auto start = std::next(std::begin(data_), pos);
    auto end = std::next(start, offset);

    return abc::bytes_view<ByteNumberingV>::from(start, end, ByteNumberingType<ByteNumberingV>{});
}

template <std::size_t N, ByteNumbering ByteNumberingV>
auto
fixed_bytes<N, ByteNumberingV>::subview(size_type, size_type) const && -> expected<bytes_view<ByteNumberingV>, std::error_code>
{
    return make_unexpected(make_error_code(errc::view_built_from_rvalue));
}

template <std::size_t N, ByteNumbering ByteNumberingV>
auto
fixed_bytes<N, ByteNumberingV>::subspan(size_type pos, size_type n) & -> expected<std::span<byte>, std::error_code>
{
    if (pos >= size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    auto start = std::next(std::begin(data_), pos);
    size_type offset = (n < size() - pos) ? n : size() - pos;

    return std::span{ start, offset };
}

template <std::size_t N, ByteNumbering ByteNumberingV>
inline auto
fixed_bytes<N, ByteNumberingV>::subspan(size_type, size_type) && -> expected<std::span<byte>, std::error_code>
{
    return make_unexpected(make_error_code(errc::span_built_from_rvalue));
}

template <std::size_t N, ByteNumbering ByteNumberingV>
auto
fixed_bytes<N, ByteNumberingV>::subspan(size_type pos, size_type n) const & -> expected<std::span<byte const>, std::error_code>
{
    if (pos >= size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    auto start = std::next(std::begin(data_), pos);
    size_type offset = (n < size() - pos) ? n : size() - pos;

    return std::span{ start, offset };
}

template <std::size_t N, ByteNumbering ByteNumberingV>
auto
fixed_bytes<N, ByteNumberingV>::subspan(size_type, size_type) const && -> expected<std::span<byte const>, std::error_code>
{
    return make_unexpected(make_error_code(errc::span_built_from_rvalue));
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <std::unsigned_integral T>
constexpr void
fixed_bytes<N, ByteNumberingV>::to_little_endian(T value, internal_type & data)
    requires((ByteNumberingV == ByteNumbering::Lsb0) || (ByteNumberingV == ByteNumbering::Msb0))
{
    for (auto i = 0u; i < std::min(sizeof(T), data.size()); value >>= 8, ++i)
    {
        T v = value & static_cast<T>(0xff);
        data[i] = static_cast<uint8_t>(v);
    }
}

template <std::size_t N, ByteNumbering ByteNumberingV>
template <std::unsigned_integral T>
constexpr void
fixed_bytes<N, ByteNumberingV>::to_big_endian(T value, internal_type & data)
    requires((ByteNumberingV == ByteNumbering::Lsb0) || (ByteNumberingV == ByteNumbering::Msb0))
{
    for (auto i = data.size(); i != 0; value >>= 8, --i)
    {
        T v = value & static_cast<T>(0xff);
        data[i - 1] = static_cast<uint8_t>(v);
    }
}

} // namespace abc

#endif // ABC_FIXED_BYTES
