// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_BYTES)
#define ABC_BYTES

#pragma once

#include "abc/byte.h"
#include "abc/byte_bit_numbering.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

namespace abc {

class [[nodiscard]] bytes {
private:
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

    bytes() = default;
    bytes(bytes const &) = default;
    auto operator=(bytes const &)->bytes & = default;
    bytes(bytes &&) = default;
    auto operator=(bytes &&)->bytes & = default;
    ~bytes() = default;

    constexpr explicit bytes(std::vector<byte> raw) noexcept : data_{ std::move(raw) } {
    }

    constexpr auto operator=(std::vector<byte> raw) noexcept -> bytes & {
        data_ = std::move(raw);
        return *this;
    }

    template <std::input_iterator Iterator>
    constexpr bytes(Iterator first, Iterator last) : data_{ first, last } {
    }

    constexpr explicit bytes(std::span<byte const> const span) : bytes{ std::begin(span), std::end(span) } {
    }

    constexpr bytes(std::initializer_list<value_type> const init) : data_{ init } {
    }

    constexpr void assign(size_type const count, byte const & value) {
        data_.assign(count, value);
    }

    template <std::input_iterator Iterator>
    constexpr void assign(Iterator first, Iterator last) {
        data_.assign(first, last);
    }

    constexpr void assign(std::initializer_list<value_type> const init_list) {
        data_.assign(init_list);
    }

    constexpr auto at(size_type const pos) -> reference {
        return data_.at(pos);
    }

    [[nodiscard]] constexpr auto at(size_type const pos) const -> const_reference {
        return data_.at(pos);
    }

    constexpr auto operator[](size_type const pos) -> reference {
        return data_[pos];
    }

    constexpr auto operator[](size_type const pos) const -> const_reference {
        return data_[pos];
    }

    constexpr auto front() -> reference {
        return data_.front();
    }

    [[nodiscard]] constexpr auto front() const -> const_reference {
        return data_.front();
    }

    constexpr auto back() -> reference {
        return data_.back();
    }

    [[nodiscard]] constexpr auto back() const -> const_reference {
        return data_.back();
    }

    [[nodiscard]] constexpr auto data() noexcept -> value_type * {
        return data_.data();
    }

    [[nodiscard]] constexpr auto data() const noexcept -> value_type const * {
        return data_.data();
    }

    constexpr auto begin() noexcept -> iterator {
        return data_.begin();
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return data_.begin();
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return data_.cbegin();
    }

    constexpr auto end() noexcept -> iterator {
        return data_.end();
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return data_.end();
    }

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
        return data_.cend();
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return data_.empty();
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return data_.size();
    }

    constexpr void reserve(size_type const new_cap) {
        data_.reserve(new_cap);
    }

    constexpr void clear() noexcept {
        data_.clear();
    }

    constexpr auto insert(const const_iterator pos, value_type const value) -> iterator {
        return data_.insert(pos, value);
    }

    constexpr auto insert(const const_iterator pos, size_type const count, value_type const value) -> iterator {
        return data_.insert(pos, count, value);
    }

    template <std::input_iterator Iterator>
    constexpr auto insert(const_iterator pos, Iterator first, Iterator last) -> iterator {
        return data_.insert(pos, first, last);
    }

    constexpr auto insert(const const_iterator pos, const std::initializer_list<value_type> init_list) -> iterator {
        return data_.insert(pos, init_list);
    }

    constexpr auto erase(const const_iterator pos) -> iterator {
        return data_.erase(pos);
    }

    constexpr auto erase(const const_iterator first, const const_iterator last) -> iterator {
        return data_.erase(first, last);
    }

    constexpr void push_back(value_type const value) {
        data_.push_back(value);
    }

    constexpr void pop_back() {
        data_.pop_back();
    }

    constexpr void resize(size_type const count) {
        data_.resize(count);
    }

    constexpr void resize(size_type const count, value_type const value) {
        data_.resize(count, value);
    }

    constexpr void swap(bytes & other) noexcept {
        data_.swap(other.data_);
    }

    constexpr explicit operator std::vector<byte> const &() const noexcept {
        return data_;
    }

    constexpr explicit operator std::vector<byte> & () noexcept {
        return data_;
    }

    //constexpr auto operator+(bytes const & other) const -> bytes {
    //    internal_type data;
    //    data.reserve(size() + other.size());

    //    std::ranges::copy(*this, std::back_inserter(data));
    //    std::ranges::copy(other, std::back_inserter(data));

    //    return bytes{ std::move(data) };
    //}

    //constexpr auto operator+=(bytes const & other) -> bytes & {
    //    data_.reserve(size() + other.size());
    //    std::ranges::copy(other, std::back_inserter(data_));
    //    return *this;
    //}

    constexpr auto operator+(std::span<byte const> const other) const -> bytes {
        auto data = *this;
        return data += other;
    }

    constexpr auto operator+=(std::span<byte const> const other) -> bytes & {
        data_.reserve(size() + other.size());
        std::copy(std::begin(other), std::end(other), std::back_inserter(data_));
        return *this;
    }

    constexpr auto operator+(byte const byte) const -> bytes {
        auto bytes = *this;
        bytes += byte;
        return bytes;
    }

    constexpr auto operator+=(byte const byte) -> bytes & {
        data_.push_back(byte);
        return *this;
    }

    [[nodiscard]] constexpr auto first(size_t const count) const noexcept -> std::span<byte const> {
        assert(count <= data_.size());
        return std::span{ std::begin(data_), count };
    }

    [[nodiscard]] constexpr auto last(size_t const count) const noexcept -> std::span<byte const> {
        assert(count <= data_.size());
        return std::span{ std::next(std::begin(data_), static_cast<std::ptrdiff_t>(size() - count)), count };
    }

    constexpr auto sub_bytes(size_t const offset, size_t const count = -1) const -> bytes {
        if (offset >= data_.size()) {
            return {};
        }

        return bytes{ std::span{ data_.data() + offset, count } };
    }

private:
    friend constexpr auto operator==(bytes const & lhs, bytes const & rhs) noexcept -> bool = default;
    friend constexpr auto operator<=>(bytes const & lhs, bytes const & rhs) noexcept -> std::strong_ordering = default;
};

template <byte_numbering ByteNumbering>
class [[nodiscard]] bytes_with {
private:
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

    bytes_with() = default;
    bytes_with(bytes_with const &) = default;
    auto operator=(bytes_with const &) -> bytes_with & = default;
    bytes_with(bytes_with &&) = default;
    auto operator=(bytes_with &&) -> bytes_with & = default;
    ~bytes_with() = default;

    constexpr auto operator=(std::initializer_list<value_type> const il) noexcept -> bytes & {
        data_ = il;
        return *this;
    }

    template <std::input_iterator Iterator>
    constexpr bytes_with(Iterator first, Iterator last) : data_{ first, last } {
    }

    constexpr explicit bytes_with(std::span<byte const> const span) : bytes_with{ std::begin(span), std::end(span) } {
    }

    constexpr bytes_with(std::initializer_list<value_type> const init) : data_{ init } {
    }

    constexpr static auto from(std::unsigned_integral auto i) -> bytes_with {
        bytes_with bs;
        bs.reserve(sizeof(i));

        do {
            bs.push_back(static_cast<byte>(i));
            i >>= 8;
        } while (i);

        if constexpr (ByteNumbering == byte_numbering::msb0) {
            std::ranges::reverse(bs);
        }

        return bs;
    }

    [[nodiscard]] constexpr auto at(size_type const pos) -> reference {
        return data_.at(pos);
    }

    [[nodiscard]] constexpr auto at(size_type const pos) const -> const_reference {
        return data_.at(pos);
    }

    [[nodiscard]] constexpr auto operator[](size_type const pos) -> reference {
        return data_[pos];
    }

    [[nodiscard]] constexpr auto operator[](size_type const pos) const -> const_reference {
        return data_[pos];
    }

    [[nodiscard]] constexpr auto front() -> reference {
        return data_.front();
    }

    [[nodiscard]] constexpr auto front() const -> const_reference {
        return data_.front();
    }

    constexpr auto back() -> reference {
        return data_.back();
    }

    [[nodiscard]] constexpr auto back() const -> const_reference {
        return data_.back();
    }

    constexpr auto data() noexcept -> value_type * {
        return data_.data();
    }

    [[nodiscard]] constexpr auto data() const noexcept -> value_type const * {
        return data_.data();
    }

    constexpr auto begin() noexcept -> iterator {
        return data_.begin();
    }

    [[nodiscard]] constexpr auto begin() const noexcept -> const_iterator {
        return data_.begin();
    }

    [[nodiscard]] constexpr auto cbegin() const noexcept -> const_iterator {
        return data_.cbegin();
    }

    constexpr auto end() noexcept -> iterator {
        return data_.end();
    }

    [[nodiscard]] constexpr auto end() const noexcept -> const_iterator {
        return data_.end();
    }

    [[nodiscard]] constexpr auto cend() const noexcept -> const_iterator {
        return data_.cend();
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return data_.empty();
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return data_.size();
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

    constexpr void swap(bytes_with & other) noexcept {
        data_.swap(other.data_);
    }

    constexpr explicit operator std::vector<byte> const &() const noexcept {
        return data_;
    }

    constexpr explicit operator std::vector<byte> & () noexcept {
        return data_;
    }

    [[nodiscard]] constexpr auto least_significant_byte() const -> const_reference {
        if constexpr (ByteNumbering == byte_numbering::msb0) {
            return data_.back();
        }

        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            return data_.front();
        }
    }

    [[nodiscard]] constexpr auto most_significant_byte() const -> const_reference {
        if constexpr (ByteNumbering == byte_numbering::msb0) {
            return data_.front();
        }

        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            return data_.back();
        }
    }

private:
    friend constexpr auto operator==(bytes_with const & lhs, bytes_with const & rhs) noexcept -> bool = default;
};

using bytes_msb0_t = bytes_with<byte_numbering::msb0>;
using bytes_lsb0_t = bytes_with<byte_numbering::lsb0>;

[[nodiscard]] constexpr auto operator<=>(bytes const & lhs, std::vector<byte> const & rhs) -> std::strong_ordering {
    return static_cast<std::vector<byte>>(lhs) <=> rhs;
}

[[nodiscard]] constexpr auto operator==(bytes const & lhs, std::vector<byte> const & rhs) -> bool {
    return static_cast<std::vector<byte>>(lhs) == rhs;
}

//constexpr auto operator+(std::span<byte const> const lhs, bytes const& rhs) -> bytes {
//    return bytes{ lhs } + rhs;
//}
//
//constexpr auto operator+(byte const lhs, std::span<byte const> const rhs) -> bytes {
//    return bytes{ lhs } + rhs;
//}

constexpr void swap(bytes & lhs, bytes & rhs) noexcept {
    lhs.swap(rhs);
}

constexpr auto erase(bytes & c, bytes::value_type const value) -> bytes::size_type {
    return std::erase(static_cast<std::vector<byte> &>(c), value);
}

template <typename Predictor>
constexpr auto erase_if(bytes & c, Predictor predictor) -> bytes::size_type {
    return std::erase_if(static_cast<std::vector<byte> &>(c), std::move(predictor));
}

}

#endif
