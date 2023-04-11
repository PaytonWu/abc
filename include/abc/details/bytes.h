// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_BYTES)
#define ABC_DETAILS_BYTES

#pragma once

#include "abc/byte.h"

#include <cassert>
#include <compare>
#include <span>
#include <utility>
#include <vector>

namespace abc::details {

class [[nodiscard]] xabc_bytes {
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

    constexpr explicit xabc_bytes(std::span<xbyte_t const> const span) : xabc_bytes{ std::begin(span), std::end(span) } {
    }

    constexpr xabc_bytes(std::initializer_list<value_type> const init) : data_{ init } {
    }

    constexpr void assign(size_type const count, xbyte_t const & value) {
        data_.assign(count, value);
    }

    template <typename InputIt>
    constexpr void assign(InputIt first, InputIt last) {
        data_.assign(first, last);
    }

    constexpr void assign(std::initializer_list<value_type> const ilist) {
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

    constexpr auto insert(const_iterator pos, size_type const count, value_type const value) -> iterator {
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

    explicit operator std::vector<xbyte_t> const &() const noexcept {
        return data_;
    }

    explicit operator std::vector<xbyte_t> & () noexcept {
        return data_;
    }

    auto operator+(xabc_bytes const & other) const -> xabc_bytes;
    auto operator+=(xabc_bytes const & other) -> xabc_bytes &;
    auto operator+(std::span<xbyte_t const> other) const -> xabc_bytes;
    auto operator+=(std::span<xbyte_t const> other) -> xabc_bytes &;
    auto operator+(xbyte_t byte) const -> xabc_bytes;
    auto operator+=(xbyte_t byte) -> xabc_bytes &;

    [[nodiscard]] constexpr auto first(size_t const count) const noexcept -> std::span<xbyte_t const> {
        assert(count <= data_.size());
        return std::span{ std::begin(data_), count };
    }

    [[nodiscard]] constexpr auto last(size_t const count) const noexcept -> std::span<xbyte_t const> {
        assert(count <= data_.size());
        return std::span{ std::next(std::begin(data_), static_cast<std::ptrdiff_t>(size() - count)), count };
    }

    auto sub_bytes(size_t offset, size_t count = -1) const -> xabc_bytes;

private:
    friend [[nodiscard]] constexpr auto operator==(xabc_bytes const & lhs, xabc_bytes const & rhs) noexcept -> bool = default;
    friend [[nodiscard]] constexpr auto operator<=>(xabc_bytes const & lhs, xabc_bytes const & rhs) noexcept -> std::strong_ordering = default;
};

} // namespace abc::details

#endif
