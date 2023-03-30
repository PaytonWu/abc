// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_BYTES)
#define ABC_BYTES

#pragma once

#include "byte.h"
#include "bytes_fwd.h"
#include "error.h"
#include "simple_converter.h"

#include <array>
#include <cassert>
#include <span>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

namespace abc::details {

class xabc_bytes {
private:
    using internal_type = std::vector<xbyte_t>;

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

    xabc_bytes() = default;
    constexpr xabc_bytes(xabc_bytes const &) = default;
    auto operator=(xabc_bytes const &)->xabc_bytes & = default;
    constexpr xabc_bytes(xabc_bytes &&) = default;
    auto operator=(xabc_bytes &&)->xabc_bytes & = default;
    ~xabc_bytes() = default;

    constexpr explicit xabc_bytes(std::vector<xbyte_t> raw) noexcept : data_{ std::move(raw) } {
    }

    auto operator=(std::vector<xbyte_t> raw) noexcept -> xabc_bytes & {
        data_ = std::move(raw);
        return *this;
    }

    constexpr xabc_bytes(size_type const count, xbyte_t const value) : data_(count, value) {
    }

    constexpr explicit xabc_bytes(size_type const count) : data_(count) {
    }

    template <typename InputIt>
    constexpr xabc_bytes(InputIt first, InputIt last) : data_{ first, last } {
    }

    constexpr explicit xabc_bytes(std::span<xbyte_t const> const & span) : xabc_bytes{ std::begin(span), std::end(span) } {
    }

    constexpr xabc_bytes(std::initializer_list<value_type> init) : data_{ init } {
    }

    constexpr void assign(size_type const count, xbyte_t const & value) {
        data_.assign(count, value);
    }

    template <typename InputIt>
    constexpr void assign(InputIt first, InputIt last) {
        data_.assign(first, last);
    }

    constexpr void assign(std::initializer_list<value_type> ilist) {
        data_.assign(ilist);
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

    constexpr auto insert(const_iterator pos, value_type const value) -> iterator {
        return data_.insert(std::move(pos), value);
    }

    constexpr auto insert(const_iterator pos, size_type count, value_type const value) -> iterator {
        return data_.insert(std::move(pos), count, value);
    }

    template <typename InputIt>
    constexpr auto insert(const_iterator pos, InputIt first, InputIt last) -> iterator {
        return data_.insert(std::move(pos), first, last);
    }

    constexpr auto insert(const_iterator pos, std::initializer_list<value_type> ilist) -> iterator {
        return data_.insert(std::move(pos), ilist);
    }

    constexpr auto erase(const_iterator pos) -> iterator {
        return data_.erase(std::move(pos));
    }

    constexpr auto erase(const_iterator first, const_iterator last) -> iterator {
        return data_.erase(std::move(first), std::move(last));
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

    constexpr void swap(xabc_bytes & other) noexcept {
        data_.swap(other.data_);
    }

    [[nodiscard]] constexpr auto raw_data() const noexcept -> std::vector<xbyte_t> const & {
        return data_;
    }

    [[nodiscard]] constexpr auto raw_data() noexcept -> std::vector<xbyte_t> & {
        return data_;
    }

    explicit operator std::vector<xbyte_t>() const {
        return data_;
    }

    [[nodiscard]] auto operator+(xabc_bytes const & other) const->xabc_bytes;
    auto operator+=(xabc_bytes const & other)->xabc_bytes &;
    //[[nodiscard]] auto operator+(xabc_bytes && other) const -> xabc_bytes;
    //auto operator+=(xabc_bytes && other) -> xabc_bytes &;
    [[nodiscard]] auto operator+(std::span<xbyte_t const> const & other) const->xabc_bytes;
    auto operator+=(std::span<xbyte_t const> const & other)->xabc_bytes &;
    [[nodiscard]] auto operator+(xbyte_t byte) const->xabc_bytes;
    auto operator+=(xbyte_t byte)->xabc_bytes &;

    [[nodiscard]] constexpr auto first(size_t const count) const noexcept -> std::span<xbyte_t const> {
        assert(count <= data_.size());
        return std::span{ std::begin(data_), count };
    }

    [[nodiscard]] constexpr auto last(size_t const count) const noexcept -> std::span<xbyte_t const> {
        assert(count <= data_.size());
        return std::span{ std::next(std::begin(data_), static_cast<std::ptrdiff_t>(size() - count)), count };
    }

    [[nodiscard]] auto sub_bytes(size_t offset, size_t count = -1) const->xabc_bytes;
};

}  //

namespace abc {

using xbytes_t = details::xabc_bytes;

using xbytes32_t = std::array<xbyte_t, 32>;
using xbytes16_t = std::array<xbyte_t, 16>;
using xbytes8_t = std::array<xbyte_t, 8>;
using xbytes1_t = std::array<xbyte_t, 1>;
using xbytes256_t = std::array<xbyte_t, 256>;

using xbytes_view_t = std::span<xbyte_t const>;
using xbytes32_view_t = std::span<xbyte_t const, 32>;
using xbytes16_view_t = std::span<xbyte_t const, 16>;
using xbytes8_view_t = std::span<xbyte_t const, 8>;
using xbytes256_view_t = std::span<xbyte_t const, 256>;


[[nodiscard]] constexpr auto operator<=>(xbytes_t const & lhs, xbytes_t const & rhs) -> std::strong_ordering {
    return lhs.raw_data() <=> rhs.raw_data();
}

[[nodiscard]] constexpr auto operator==(xbytes_t const & lhs, xbytes_t const & rhs) -> bool {
    return lhs.raw_data() == rhs.raw_data();
}

[[nodiscard]] constexpr auto operator<=>(xbytes_t const & lhs, std::vector<xbyte_t> const & rhs) -> std::strong_ordering {
    return lhs.raw_data() <=> rhs;
}

[[nodiscard]] constexpr auto operator==(xbytes_t const & lhs, std::vector<xbyte_t> const & rhs) -> bool {
    return lhs.raw_data() == rhs;
}

[[nodiscard]] auto operator+(std::span<xbyte_t const> const & lhs, xbytes_t const & rhs)->xbytes_t;
[[nodiscard]] auto operator+(xbyte_t lhs, xbytes_t const & rhs)->xbytes_t;
[[nodiscard]] auto operator+(xbyte_t lhs, std::span<xbyte_t const> const & rhs)->xbytes_t;

constexpr void swap(xbytes_t & lhs, xbytes_t & rhs) noexcept {
    lhs.swap(rhs);
}

constexpr auto erase(xbytes_t & c, xbytes_t::value_type const value) -> xbytes_t::size_type {
    return std::erase(c.raw_data(), value);
}

template <typename Predictor>
constexpr auto erase_if(xbytes_t & c, Predictor predictor) -> xbytes_t::size_type {
    return std::erase_if(c.raw_data(), std::move(predictor));
}

}

#include "hex_string_fwd.h"

namespace abc {

template <>
auto to<xhex_string_t, xbytes_t>(xbytes_t const & from)->xhex_string_t;

}

#endif
