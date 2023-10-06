// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_BYTES)
#define ABC_BYTES

#pragma once

#include "byte.h"
#include "byte_bit_numbering.h"
#include "expected.h"
#include "utility.h"

#include <range/v3/algorithm/equal.hpp>
#include <range/v3/algorithm/lexicographical_compare.hpp>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/algorithm.hpp>
#include <range/v3/view/reverse.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cstring>
#include <span>
#include <utility>
#include <vector>
#include <bit>

namespace abc {

template <byte_numbering ByteNumbering>
class [[nodiscard]] bytes {
private:
    template <byte_numbering RhsByteNumbering>
    friend class bytes;

    using internal_type = std::vector<byte>;

    internal_type data_{};

public:
    using value_type = internal_type::value_type;
    using allocator_type = internal_type::allocator_type;
    using size_type = internal_type::size_type;
    using difference_type = internal_type::difference_type;
    using reference = internal_type::reference;
    using const_reference = internal_type::const_reference;
    using pointer = internal_type::pointer;
    using const_pointer = internal_type::const_pointer;
    using iterator = internal_type::iterator;
    using const_iterator = internal_type::const_iterator;
    using reverse_iterator = internal_type::reverse_iterator;
    using const_reverse_iterator = internal_type::const_reverse_iterator;

    // public constructors

    bytes() = default;

    template <byte_numbering RhsByteNumbering> requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr explicit bytes(bytes<RhsByteNumbering> const & rhs) : data_{ rhs.data_ } {
        ranges::reverse(data_);
    }

    template <byte_numbering RhsByteNumbering> requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr explicit bytes(bytes<RhsByteNumbering> && rhs) noexcept : data_{ std::move(rhs.data_) } {
        ranges::reverse(data_);
    }

    template <byte_numbering RhsByteNumbering> requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering == byte_numbering::none)
    constexpr explicit bytes(bytes<RhsByteNumbering> const & rhs) : data_{ rhs.data_ } {
    }

    template <byte_numbering RhsByteNumbering> requires(RhsByteNumbering != ByteNumbering && RhsByteNumbering == byte_numbering::none)
    constexpr explicit bytes(bytes<RhsByteNumbering> && rhs) noexcept : data_{ std::move(rhs.data_) } {
    }

    constexpr explicit bytes(std::vector<byte> raw) noexcept requires(ByteNumbering == byte_numbering::none)
        : data_{ std::move(raw) } {
    }

    template <std::input_iterator Iterator>
    constexpr bytes(Iterator begin, Iterator end) requires(ByteNumbering == byte_numbering::none)
        : data_{ begin, end } {
    }

    constexpr explicit bytes(std::span<byte const> const span) requires(ByteNumbering == byte_numbering::none)
        : bytes{ std::begin(span), std::end(span) } {
    }

    constexpr bytes(std::initializer_list<value_type> const il) : data_{ il } {
    }

    constexpr auto operator=(std::vector<byte> raw) noexcept -> bytes & requires(ByteNumbering == byte_numbering::none) {
        data_ = std::move(raw);
        return *this;
    }

private:
    template <std::input_iterator Iterator, byte_numbering SrcByteNumbering> requires(SrcByteNumbering == ByteNumbering)
    constexpr bytes(Iterator begin, Iterator end, byte_numbering_type<SrcByteNumbering>) : data_{ begin, end } {
    }

    template <std::input_iterator Iterator, byte_numbering SrcByteNumbering> requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr bytes(Iterator begin, Iterator end, byte_numbering_type<SrcByteNumbering>) : data_{ begin, end } {
        ranges::reverse(data_);
    }

    template <byte_numbering SrcByteNumbering> requires(SrcByteNumbering == ByteNumbering)
    constexpr bytes(std::initializer_list<value_type> const il, byte_numbering_type<SrcByteNumbering>) : data_{ il } {
    }

    template <byte_numbering SrcByteNumbering> requires(SrcByteNumbering != ByteNumbering && SrcByteNumbering != byte_numbering::none && ByteNumbering != byte_numbering::none)
    constexpr bytes(std::initializer_list<value_type> const il, byte_numbering_type<SrcByteNumbering>) : data_{ il } {
        ranges::reverse(data_);
    }

    template <byte_numbering SrcByteNumbering>
    constexpr explicit bytes(std::span<byte const> const src, byte_numbering_type<SrcByteNumbering>) : bytes{ std::begin(src), std::end(src), byte_numbering_type<SrcByteNumbering>{} } {
    }

public:
    constexpr auto operator=(std::initializer_list<value_type> const il) noexcept -> bytes & {
        data_ = il;
        return *this;
    }

    template <std::integral T>
    constexpr static auto from(T i) -> bytes {
        bytes bs;
        bs.reserve(sizeof(i));

        for (std::size_t j = 0; j < sizeof(T) && i != 0; ++j) {
            // force little endian
            bs.push_back(static_cast<byte>(i));
            i >>= 8;
        }

        if constexpr (ByteNumbering == byte_numbering::msb0) {
            ranges::reverse(bs);
        }

        return bs;
    }

    template <byte_numbering DataByteNumbering, std::input_iterator InputIterator>
    constexpr static auto from(InputIterator begin, InputIterator end) -> bytes {
        return bytes{ begin, end, byte_numbering_type<DataByteNumbering>{} };
    }

    template <byte_numbering DataByteNumbering>
    constexpr static auto from(std::span<byte const> const data) -> bytes {
        return bytes{ data, byte_numbering_type<DataByteNumbering>{} };
    }

    template <byte_numbering DataByteNumbering>
    constexpr static auto from(std::initializer_list<byte> il) -> bytes {
        return bytes{ il, byte_numbering_type<DataByteNumbering>{} };
    }

    template <byte_numbering RhsByteNumbering>
    constexpr static auto from(bytes<RhsByteNumbering> const & rhs) -> bytes {
        return bytes{ rhs };
    }

    template <byte_numbering RhsByteNumbering>
    constexpr static auto from(bytes<RhsByteNumbering> && rhs) -> bytes {
        return bytes{ std::move(rhs) };
    }

    template <std::integral T> requires(ByteNumbering != byte_numbering::none)
    constexpr auto to() const noexcept -> T {
        T value{0};
        if constexpr (ByteNumbering == byte_numbering::msb0) {
            for (auto const byte : data_ | ranges::views::reverse) {
                value <<= 8;
                value |= static_cast<T>(byte);
            }
        } else {
            for (auto const byte : data_) {
                value <<= 8;
                value |= static_cast<T>(byte);
            }
        }

        return value;
    }

    template <byte_numbering ToByteNumbering> requires (ByteNumbering != ToByteNumbering)
    constexpr auto to() const & -> bytes<ToByteNumbering> {
        return bytes<ToByteNumbering>{ *this };
    }

    template <byte_numbering ToByteNumbering> requires (ByteNumbering != ToByteNumbering)
    constexpr auto to() && -> bytes<ToByteNumbering> {
        return bytes<ToByteNumbering>{ std::move(*this) };
    }

    constexpr void assign(size_type const count, byte const & value) {
        data_.assign(count, value);
    }

    [[nodiscard]]
    constexpr auto at(size_type const pos) -> reference {
        return data_.at(pos);
    }

    [[nodiscard]]
    constexpr auto at(size_type const pos) const -> const_reference {
        return data_.at(pos);
    }

    [[nodiscard]]
    constexpr auto operator[](size_type const pos) -> reference {
        return data_[pos];
    }

    [[nodiscard]]
    constexpr auto operator[](size_type const pos) const -> const_reference {
        return data_[pos];
    }

    [[nodiscard]]
    constexpr auto front() -> reference {
        return data_.front();
    }

    [[nodiscard]]
    constexpr auto front() const -> const_reference {
        return data_.front();
    }

    [[nodiscard]]
    constexpr auto back() -> reference {
        return data_.back();
    }

    [[nodiscard]]
    constexpr auto back() const -> const_reference {
        return data_.back();
    }

    [[nodiscard]]
    constexpr auto data() noexcept -> value_type * {
        return data_.data();
    }

    [[nodiscard]]
    constexpr auto data() const noexcept -> value_type const * {
        return data_.data();
    }

    [[nodiscard]]
    constexpr auto begin() noexcept -> iterator {
        return data_.begin();
    }

    [[nodiscard]]
    constexpr auto begin() const noexcept -> const_iterator {
        return data_.begin();
    }

    [[nodiscard]]
    constexpr auto cbegin() const noexcept -> const_iterator {
        return data_.cbegin();
    }

    [[nodiscard]]
    constexpr auto end() noexcept -> iterator {
        return data_.end();
    }

    [[nodiscard]]
    constexpr auto end() const noexcept -> const_iterator {
        return data_.end();
    }

    [[nodiscard]]
    constexpr auto cend() const noexcept -> const_iterator {
        return data_.cend();
    }

    [[nodiscard]]
    constexpr auto empty() const noexcept -> bool {
        return data_.empty();
    }

    [[nodiscard]]
    constexpr auto size() const noexcept -> size_type {
        return data_.size();
    }

    constexpr auto resize(size_type new_size) -> void {
        data_.resize(new_size);
    }

    constexpr void reserve(size_type const new_cap) {
        data_.reserve(new_cap);
    }

    constexpr void clear() noexcept {
        data_.clear();
    }

    constexpr void push_back(value_type const value) {
        data_.push_back(value);
    }

    constexpr void pop_back() {
        data_.pop_back();
    }

    constexpr void swap(bytes & other) noexcept {
        data_.swap(other.data_);
    }

    constexpr explicit operator std::vector<byte> const &() const noexcept requires(ByteNumbering == byte_numbering::none) {
        return data_;
    }

    constexpr explicit operator std::vector<byte> & () noexcept requires(ByteNumbering == byte_numbering::none) {
        return data_;
    }

    [[nodiscard]]
    constexpr auto least_significant_byte() const -> const_reference requires(ByteNumbering != byte_numbering::none) {
        if constexpr (ByteNumbering == byte_numbering::msb0) {
            return data_.back();
        }

        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            return data_.front();
        }
    }

    [[nodiscard]]
    constexpr auto least_significant_byte() -> reference requires(ByteNumbering != byte_numbering::none) {
        if constexpr (ByteNumbering == byte_numbering::msb0) {
            return data_.back();
        }

        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            return data_.front();
        }
    }

    [[nodiscard]]
    constexpr auto most_significant_byte() const -> const_reference requires(ByteNumbering != byte_numbering::none) {
        if constexpr (ByteNumbering == byte_numbering::msb0) {
            return data_.front();
        }

        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            return data_.back();
        }
    }

    [[nodiscard]]
    constexpr auto most_significant_byte() -> reference requires(ByteNumbering != byte_numbering::none) {
        if constexpr (ByteNumbering == byte_numbering::msb0) {
            return data_.front();
        }

        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            return data_.back();
        }
    }

    [[nodiscard]]
    constexpr auto first(size_t const count) const noexcept -> std::span<byte const> {
        assert(count <= data_.size());
        return std::span{ std::begin(data_), count };
    }

    [[nodiscard]]
    constexpr auto last(size_t const count) const noexcept -> std::span<byte const> {
        assert(count <= data_.size());
        return std::span{ std::next(std::begin(data_), static_cast<std::ptrdiff_t>(size() - count)), count };
    }

    inline auto subspan(size_type const pos, size_type const n = static_cast<size_type>(-1)) const -> expected<std::span<byte const>, std::error_code> {
        if (pos >= data_.size()) {
            return make_unexpected(make_error_code(std::errc::result_out_of_range));
        }

        auto start = std::next(std::begin(data_), static_cast<ptrdiff_t>(pos));
        size_type count = (n < size() - pos) ? n : size() - pos;

        return std::span{start, count};
    }

    inline auto subspan(size_type const pos, size_type const n = static_cast<size_type>(-1)) -> expected<std::span<byte>, std::error_code> {
        if (pos >= data_.size()) {
            return make_unexpected(make_error_code(std::errc::result_out_of_range));
        }

        auto start = std::next(std::begin(data_), static_cast<ptrdiff_t>(pos));
        size_type count = (n < size() - pos) ? n : size() - pos;

        return std::span{start, count};
    }

    inline auto subbytes(size_type const pos, size_type const n = static_cast<size_type>(-1)) const -> expected<bytes, std::error_code> {
        if (pos >= data_.size()) {
            return make_unexpected(make_error_code(std::errc::result_out_of_range));
        }

        auto start = std::next(std::begin(data_), static_cast<ptrdiff_t>(pos));
        size_type count = (n < size() - pos) ? n : size() - pos;

        return bytes{ start, std::next(start, static_cast<ptrdiff_t>(count)), byte_numbering_type<ByteNumbering>{} };
    }

    constexpr auto operator+(bytes const & other) const -> bytes {
        auto result = *this;
        return result += other;
    }

    constexpr auto operator+=(bytes const & other) -> bytes {
        data_.reserve(size() + other.size());
        ranges::copy(other, std::back_inserter(data_));
        return *this;
    }

    constexpr auto operator+(byte const other) const -> bytes {
        auto result = *this;
        return result += other;
    }

    constexpr auto operator+=(byte const other) -> bytes {
        data_.reserve(size() + 1);
        data_.push_back(other);
        return *this;
    }
    
    constexpr auto operator+(std::span<byte const> const other) const -> bytes {
        auto data = *this;
        return data += other;
    }

    constexpr auto operator+=(std::span<byte const> const other) -> bytes & {
        data_.reserve(size() + other.size());
        ranges::copy(other, std::back_inserter(data_));
        return *this;
    }

private:
    friend constexpr auto operator==(bytes const & lhs, bytes const & rhs) noexcept -> bool = default;
    friend constexpr auto operator<=>(bytes const & lhs, bytes const & rhs) noexcept -> std::strong_ordering = default;

//    template <byte_numbering LhsByteNumbering, byte_numbering RhsByteNumbering> requires(LhsByteNumbering != RhsByteNumbering && LhsByteNumbering != byte_numbering::none && RhsByteNumbering != byte_numbering::none)
//    friend constexpr auto operator==(bytes<LhsByteNumbering> const & lhs, bytes<RhsByteNumbering> const & rhs) noexcept -> bool {
//        return ranges::equal(lhs.data_ | ranges::views::reverse, rhs.data_ | ranges::views::all);
//    }
//
//    template <byte_numbering LhsByteNumbering, byte_numbering RhsByteNumbering> requires(LhsByteNumbering != RhsByteNumbering && LhsByteNumbering != byte_numbering::none && RhsByteNumbering != byte_numbering::none)
//    friend constexpr auto operator<=>(bytes<LhsByteNumbering> const & lhs, bytes<RhsByteNumbering> const & rhs) noexcept -> std::strong_ordering {
//        return lhs.data_ | ranges::views::reverse <=> rhs.data_ | ranges::views::all;
//    }
};

[[nodiscard]]
constexpr auto operator<=>(bytes<byte_numbering::none> const & lhs, std::vector<byte> const & rhs) -> std::strong_ordering {
    return static_cast<std::vector<byte>>(lhs) <=> rhs;
}

[[nodiscard]]
constexpr auto operator==(bytes<byte_numbering::none> const & lhs, std::vector<byte> const & rhs) -> bool {
    return static_cast<std::vector<byte>>(lhs) == rhs;
}

//constexpr auto operator+(std::span<byte const> const lhs, bytes const& rhs) -> bytes {
//    return bytes{ lhs } + rhs;
//}

template <byte_numbering ByteNumbering>
constexpr auto operator+(byte const lhs, bytes<ByteNumbering> const & rhs) -> bytes<ByteNumbering> {
    return bytes<ByteNumbering>{ lhs } + rhs;
}

template <byte_numbering ByteNumbering>
constexpr void swap(bytes<ByteNumbering> & lhs, bytes<ByteNumbering> & rhs) noexcept {
    lhs.swap(rhs);
}

constexpr auto erase(bytes<byte_numbering::none> & c, bytes<byte_numbering::none>::value_type const value) -> bytes<byte_numbering::none>::size_type {
    return std::erase(static_cast<std::vector<byte> &>(c), value);
}

template <typename Predictor>
constexpr auto erase_if(bytes<byte_numbering::none> & c, Predictor predictor) -> bytes<byte_numbering::none>::size_type {
    return std::erase_if(static_cast<std::vector<byte> &>(c), std::move(predictor));
}

using bytes_t = bytes<byte_numbering::none>;
using bytes_msb0_t = bytes<byte_numbering::msb0>;
using bytes_lsb0_t = bytes<byte_numbering::lsb0>;
using bytes_be_t = bytes_msb0_t;
using bytes_le_t = bytes_lsb0_t;

}

#endif
