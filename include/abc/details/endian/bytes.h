// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_ENDIAN_BYTES)
#define ABC_DETAILS_ENDIAN_BYTES

#pragma once

#include "abc/byte.h"

#include <algorithm>
#include <bit>
#include <cassert>
#include <concepts>
#include <compare>
#include <iterator>
#include <ranges>
#include <span>
#include <utility>
#include <vector>

namespace abc::details::endian {

template <std::endian Endian>
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
    xabc_bytes(xabc_bytes const &) = default;
    auto operator=(xabc_bytes const &)->xabc_bytes & = default;
    xabc_bytes(xabc_bytes &&) = default;
    auto operator=(xabc_bytes &&)->xabc_bytes & = default;
    ~xabc_bytes() = default;

    constexpr auto operator=(std::initializer_list<value_type> const il) noexcept -> xabc_bytes & {
        data_ = il;
        return *this;
    }

    template <std::input_iterator Iterator>
    constexpr xabc_bytes(Iterator first, Iterator last) : data_{ first, last } {
    }

    constexpr explicit xabc_bytes(std::span<xbyte_t const> const span) : xabc_bytes{ std::begin(span), std::end(span) } {
    }

    constexpr xabc_bytes(std::initializer_list<value_type> const init) : data_{ init } {
    }

    constexpr xabc_bytes(std::unsigned_integral auto i) : xabc_bytes{} {
        data_.reserve(sizeof(i));

        do {
            data_.push_back(static_cast<xbyte_t>(i));
            i >>= 8;
        } while (i);

        if constexpr (Endian == std::endian::big) {
            std::ranges::reverse(data_);
        }
    }

//    constexpr void assign(size_type const count, xbyte_t const & value) {
//        data_.assign(count, value);
//    }
//
//    template <std::input_iterator Iterator>
//    constexpr void assign(Iterator first, Iterator last) {
//        data_.assign(first, last);
//    }
//
//    constexpr void assign(std::initializer_list<value_type> const init_list) {
//        data_.assign(init_list);
//    }

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

    constexpr void swap(xabc_bytes & other) noexcept {
        data_.swap(other.data_);
    }

    constexpr explicit operator std::vector<xbyte_t> const &() const noexcept {
        return data_;
    }

    constexpr explicit operator std::vector<xbyte_t> & () noexcept {
        return data_;
    }

    [[nodiscard]] constexpr auto least_significant_byte() const -> const_reference {
        if constexpr (Endian == std::endian::big) {
            return data_.back();
        }

        if constexpr (Endian == std::endian::little) {
            return data_.front();
        }
    }

    [[nodiscard]] constexpr auto most_significant_byte() const -> const_reference {
        if constexpr (Endian == std::endian::big) {
            return data_.front();
        }

        if constexpr (Endian == std::endian::little) {
            return data_.back();
        }
    }

private:
    friend constexpr auto operator==(xabc_bytes const & lhs, xabc_bytes const & rhs) noexcept -> bool = default;
    friend constexpr auto operator<=>(xabc_bytes const & lhs, xabc_bytes const & rhs) noexcept -> std::strong_ordering {
        if constexpr (Endian == std::endian::big) {
            return lhs.data_ <=> rhs.data_;
        }

        if constexpr (Endian == std::endian::little) {
            return std::views::reverse(lhs) <=> std::views::reverse(rhs);
        }
    }
};

} // namespace abc::details::endian

#endif //ABC_DETAILS_ENDIAN_BYTES
