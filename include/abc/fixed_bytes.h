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

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering == ByteNumbering)
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes(std::array<byte, N> const & src, byte_numbering_type<SrcByteNumbering>) : data_{ src }
{
}

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes(std::array<byte, N> const & src, byte_numbering_type<SrcByteNumbering>)
{
    ranges::copy(src | ranges::views::reverse, std::begin(data_));
}

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering == ByteNumbering)
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes(std::array<std::byte, N> const & src, byte_numbering_type<SrcByteNumbering>)
{
    ranges::copy(src | ranges::views::transform([](auto const byte) { return std::to_integer<byte>(byte); }), data_.begin());
}

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes(std::array<std::byte, N> const & src, byte_numbering_type<SrcByteNumbering>)
{
    ranges::copy(src | ranges::views::reverse | ranges::views::transform([](auto const b) { return std::to_integer<byte>(b); }), data_.begin());
}

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering == ByteNumbering)
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes(bytes_view<SrcByteNumbering> const src /*, byte_numbering_type<SrcByteNumbering>*/) : fixed_bytes{}
{
    if constexpr (ByteNumbering == byte_numbering::msb0)
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

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes(bytes_view<SrcByteNumbering> const src /*, byte_numbering_type<SrcByteNumbering>*/) : fixed_bytes{}
{
    if constexpr (ByteNumbering == byte_numbering::lsb0)
    {
        auto const end_pos = std::min(src.size(), N);
        auto src_reverse_view = src | ranges::views::reverse;
        ranges::copy(std::begin(src_reverse_view), std::next(std::begin(src_reverse_view), static_cast<difference_type>(end_pos)), std::begin(data_));
    }
    else
    {
        assert(ByteNumbering == byte_numbering::msb0);
        auto const end_pos = std::min(src.size(), N);
        ranges::copy(std::begin(src), std::next(std::begin(src), static_cast<difference_type>(end_pos)), std::begin(data_));
        ranges::reverse(data_);
    }
}

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering SrcByteNumbering>
    requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering == byte_numbering::none)
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes(bytes_view<SrcByteNumbering> const src) : fixed_bytes{}
{
    assert(src.size() == N);
    ranges::copy(src, std::begin(data_));
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes()
{
    data_.fill(0);
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes(std::unsigned_integral auto const value)
    requires((ByteNumbering == byte_numbering::lsb0) || (ByteNumbering == byte_numbering::msb0))
    : fixed_bytes{}
{
    if constexpr (ByteNumbering == byte_numbering::lsb0)
    {
        to_little_endian(value, data_);
    }

    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        to_big_endian(value, data_);
    }
}

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering ByteNumberingRhs>
    requires(ByteNumberingRhs != byte_numbering::none && ByteNumberingRhs != ByteNumbering)
constexpr fixed_bytes<N, ByteNumbering>::fixed_bytes(fixed_bytes<N, ByteNumberingRhs> const & rhs)
{
    ranges::copy(rhs | ranges::views::reverse, data_.begin());
}

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering DataByteNumbering>
auto
fixed_bytes<N, ByteNumbering>::from(std::array<byte, N> const & data) -> expected<fixed_bytes, std::error_code>
{
    if constexpr (DataByteNumbering == ByteNumbering || (DataByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none))
    {
        return fixed_bytes{ data, byte_numbering_type<DataByteNumbering>{} };
    }
    else
    {
        return make_unexpected(make_error_code(std::errc::invalid_argument));
    }
}

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering DataByteNumbering>
    requires(DataByteNumbering == ByteNumbering) || (DataByteNumbering != ByteNumbering && DataByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
auto fixed_bytes<N, ByteNumbering>::from(bytes_view<DataByteNumbering> const data) -> expected<fixed_bytes, std::error_code>
{
    return fixed_bytes{ data };
}

template <std::size_t N, byte_numbering ByteNumbering>
template <byte_numbering DataByteNumbering>
    requires(DataByteNumbering != ByteNumbering && DataByteNumbering == byte_numbering::none)
auto
fixed_bytes<N, ByteNumbering>::from(bytes_view<DataByteNumbering> const data) -> expected<fixed_bytes, std::error_code>
{
    if (data.size() != N)
    {
        return make_unexpected(make_error_code(std::errc::invalid_argument));
    }

    return fixed_bytes{ data };
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator[](size_t const index) const noexcept -> byte
{
    return data_[index];
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator[](size_t const index) noexcept -> byte &
{
    return data_[index];
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr fixed_bytes<N, ByteNumbering>::operator bytes_view<ByteNumbering>() const noexcept
{
    return abc::bytes_view<ByteNumbering>::from(data_.data(), data_.size(), byte_numbering_type<ByteNumbering>{});
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::front() const noexcept -> const_reference
{
    return data_.front();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::front() noexcept -> reference
{
    return data_.front();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::back() const noexcept -> const_reference
{
    return data_.back();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::back() noexcept -> reference
{
    return data_.back();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::begin() noexcept -> iterator
{
    return data_.begin();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::begin() const noexcept -> const_iterator
{
    return data_.begin();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::cbegin() const noexcept -> const_iterator
{
    return data_.cbegin();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::end() noexcept -> iterator
{
    return data_.end();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::end() const noexcept -> const_iterator
{
    return data_.end();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::cend() const noexcept -> const_iterator
{
    return data_.cend();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::rbegin() noexcept -> reverse_iterator
{
    return data_.rbegin();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::rbegin() const noexcept -> const_reverse_iterator
{
    return data_.rbegin();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::crbegin() const noexcept -> const_reverse_iterator
{
    return data_.crbegin();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::rend() noexcept -> reverse_iterator
{
    return data_.rend();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::rend() const noexcept -> const_reverse_iterator
{
    return data_.rend();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::crend() const noexcept -> const_reverse_iterator
{
    return data_.crend();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::data() noexcept -> pointer
{
    return data_.data();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::data() const noexcept -> const_pointer
{
    return data_.data();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::empty() const noexcept -> bool
{
    return data_.empty();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::size() const noexcept -> size_type
{
    return data_.size();
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::fill(byte const value) noexcept -> void
{
    data_.fill(value);
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::byte_numbering() noexcept -> abc::byte_numbering
{
    return ByteNumbering;
}

template <std::size_t N, byte_numbering ByteNumbering>
template <std::unsigned_integral T>
[[nodiscard]] constexpr auto
fixed_bytes<N, ByteNumbering>::to() const noexcept -> T
{
    T result{};
    if constexpr (ByteNumbering == byte_numbering::lsb0)
    {
        for (auto i = 0u; i < std::min(sizeof(T), data_.size()); ++i)
        {
            result |= static_cast<T>(data_[i]) << (i * 8);
        }
    }

    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        for (auto i = 0u; i < std::min(sizeof(T), data_.size()); ++i)
        {
            result |= static_cast<T>(data_[data_.size() - i - 1]) << (i * 8);
        }
    }
    return result;
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::zero() const noexcept -> bool
{
    return std::ranges::all_of(data_, [](auto const byte) { return byte == 0; });
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator^=(fixed_bytes const & other) -> fixed_bytes &
{
    for (auto i = 0u; i < N; ++i)
    {
        data_[i] ^= other.data_[i];
    }
    return *this;
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator^(fixed_bytes const & other) const -> fixed_bytes
{
    return fixed_bytes{ *this } ^= other;
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator|=(fixed_bytes const & other) -> fixed_bytes &
{
    for (auto i = 0u; i < N; ++i)
    {
        data_[i] |= other.data_[i];
    }
    return *this;
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator|(fixed_bytes const & other) const -> fixed_bytes
{
    return fixed_bytes{ *this } |= other;
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator&=(fixed_bytes const & other) -> fixed_bytes &
{
    for (auto i = 0u; i < N; ++i)
    {
        data_[i] &= other.data_[i];
    }
    return *this;
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator&(fixed_bytes const & other) const -> fixed_bytes
{
    return fixed_bytes{ *this } &= other;
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator~() const -> fixed_bytes
{
    fixed_bytes ret;
    for (auto i = 0u; i < N; ++i)
    {
        ret[i] = ~data_[i];
    }
    return ret;
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::operator++() -> fixed_bytes &
{
    if constexpr (ByteNumbering == byte_numbering::msb0)
    {
        for (auto i = N; i > 0 && !++data_[--i];)
        {
        }
        return *this;
    }

    if constexpr (ByteNumbering == byte_numbering::lsb0)
    {
        for (auto i = 0u; i < N && !++data_[i++];)
        {
        }
        return *this;
    }
}

template <std::size_t N, byte_numbering ByteNumbering>
constexpr auto
fixed_bytes<N, ByteNumbering>::clear() -> void
{
    data_.fill(0);
}

template <std::size_t N, byte_numbering ByteNumbering>
auto
fixed_bytes<N, ByteNumbering>::subbytes(size_type pos, size_type n) const -> expected<bytes<ByteNumbering>, std::error_code>
{
    if (pos >= size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    size_type offset = (n < size() - pos) ? n : size() - pos;

    auto start = std::next(std::begin(data_), pos);
    auto end = std::next(start, offset);

    return abc::bytes<ByteNumbering>::template from<ByteNumbering>(start, end);
}

template <std::size_t N, byte_numbering ByteNumbering>
auto
fixed_bytes<N, ByteNumbering>::subview(size_type pos, size_type n) const & -> expected<bytes_view<ByteNumbering>, std::error_code>
{
    if (pos >= size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    size_type offset = (n < size() - pos) ? n : size() - pos;

    auto start = std::next(std::begin(data_), pos);
    auto end = std::next(start, offset);

    return abc::bytes_view<ByteNumbering>::from(start, end, byte_numbering_type<ByteNumbering>{});
}

template <std::size_t N, byte_numbering ByteNumbering>
auto
fixed_bytes<N, ByteNumbering>::subview(size_type, size_type) const && -> expected<bytes_view<ByteNumbering>, std::error_code>
{
    return make_unexpected(make_error_code(errc::view_built_from_rvalue));
}

template <std::size_t N, byte_numbering ByteNumbering>
auto
fixed_bytes<N, ByteNumbering>::subspan(size_type pos, size_type n) & -> expected<std::span<byte>, std::error_code>
{
    if (pos >= size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    auto start = std::next(std::begin(data_), pos);
    size_type offset = (n < size() - pos) ? n : size() - pos;

    return std::span{ start, offset };
}

template <std::size_t N, byte_numbering ByteNumbering>
inline auto
fixed_bytes<N, ByteNumbering>::subspan(size_type, size_type) && -> expected<std::span<byte>, std::error_code>
{
    return make_unexpected(make_error_code(errc::span_built_from_rvalue));
}

template <std::size_t N, byte_numbering ByteNumbering>
auto
fixed_bytes<N, ByteNumbering>::subspan(size_type pos, size_type n) const & -> expected<std::span<byte const>, std::error_code>
{
    if (pos >= size())
    {
        return make_unexpected(make_error_code(std::errc::result_out_of_range));
    }

    auto start = std::next(std::begin(data_), pos);
    size_type offset = (n < size() - pos) ? n : size() - pos;

    return std::span{ start, offset };
}

template <std::size_t N, byte_numbering ByteNumbering>
auto
fixed_bytes<N, ByteNumbering>::subspan(size_type, size_type) const && -> expected<std::span<byte const>, std::error_code>
{
    return make_unexpected(make_error_code(errc::span_built_from_rvalue));
}

template <std::size_t N, byte_numbering ByteNumbering>
template <std::unsigned_integral T>
constexpr void
fixed_bytes<N, ByteNumbering>::to_little_endian(T value, internal_type & data)
    requires((ByteNumbering == byte_numbering::lsb0) || (ByteNumbering == byte_numbering::msb0))
{
    for (auto i = 0u; i < std::min(sizeof(T), data.size()); value >>= 8, ++i)
    {
        T v = value & static_cast<T>(0xff);
        data[i] = static_cast<uint8_t>(v);
    }
}

template <std::size_t N, byte_numbering ByteNumbering>
template <std::unsigned_integral T>
constexpr void
fixed_bytes<N, ByteNumbering>::to_big_endian(T value, internal_type & data)
    requires((ByteNumbering == byte_numbering::lsb0) || (ByteNumbering == byte_numbering::msb0))
{
    for (auto i = data.size(); i != 0; value >>= 8, --i)
    {
        T v = value & static_cast<T>(0xff);
        data[i - 1] = static_cast<uint8_t>(v);
    }
}

} // namespace abc

#endif // ABC_FIXED_BYTES
