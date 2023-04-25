// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_FIXED_BYTES)
#define ABC_DETAILS_FIXED_BYTES

#pragma once

#include "abc/byte.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <ranges>

namespace abc::details {

template <size_t Size, std::endian Endian>
class [[nodiscard]] xabc_fixed_bytes {
    static_assert(Size > 0);
    static_assert(Endian == std::endian::little || Endian == std::endian::big);

private:
    using internal_type = std::array<xbyte_t, Size>;
    internal_type data_{};

public:
    using value_type = typename internal_type::value_type;
    using size_type = typename internal_type::size_type;
    using difference_type = typename internal_type::difference_type;
    using reference = typename internal_type::reference;
    using const_reference = typename internal_type::const_reference;
    using pointer = typename internal_type::pointer;
    using const_pointer = typename internal_type::const_pointer;
    using iterator = typename internal_type::iterator;
    using const_iterator = typename internal_type::const_iterator;
    using reverse_iterator = typename internal_type::reverse_iterator;
    using const_reverse_iterator = typename internal_type::const_reverse_iterator;

    xabc_fixed_bytes(xabc_fixed_bytes const &) = default;
    auto operator=(xabc_fixed_bytes const &) -> xabc_fixed_bytes & = default;
    xabc_fixed_bytes(xabc_fixed_bytes &&) = default;
    auto operator=(xabc_fixed_bytes &&) -> xabc_fixed_bytes & = default;
    ~xabc_fixed_bytes() = default;

    constexpr xabc_fixed_bytes() {
        data_.fill(0);
    }

    constexpr xabc_fixed_bytes(std::initializer_list<std::byte> const list) : xabc_fixed_bytes{} {
        assert(list.size() <= Size);
        std::ranges::copy(list | std::views::transform([](auto const byte) { return std::to_integer<xbyte_t>(byte); }), data_.begin());
    }

    constexpr explicit xabc_fixed_bytes(std::unsigned_integral auto const value) : xabc_fixed_bytes{} {
        if constexpr (Endian == std::endian::little) {
            to_little_endian(value, data_);
        }

        if constexpr (Endian == std::endian::big) {
            to_big_endian(value, data_);
        }
    }

    constexpr explicit xabc_fixed_bytes(internal_type const & data) : data_{ data } {
    }

    template <size_t SizeU>
    constexpr explicit xabc_fixed_bytes(std::array<xbyte_t, SizeU> const & data) : xabc_fixed_bytes{} {
        std::copy(data.begin(), data.end(), data_.begin());
    }

    template <size_t SizeU>
    constexpr explicit xabc_fixed_bytes(std::array<std::byte, SizeU> const & data) : xabc_fixed_bytes{} {
        std::ranges::copy(data | std::views::transform([](auto const byte) { return std::to_integer<xbyte_t>(byte); }), data_.begin());
    }

    friend auto operator==(xabc_fixed_bytes const & lhs, xabc_fixed_bytes const & rhs) noexcept -> bool = default;
    friend auto operator<=>(xabc_fixed_bytes const & lhs, xabc_fixed_bytes const & rhs) noexcept -> std::strong_ordering = default;

    constexpr auto operator[](size_t const index) const noexcept -> xbyte_t {
        return data_[index];
    }

    constexpr auto operator[](size_t const index) noexcept -> xbyte_t & {
        return data_[index];
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

    constexpr auto rbegin() noexcept -> reverse_iterator {
        return data_.rbegin();
    }

    [[nodiscard]] constexpr auto rbegin() const noexcept -> const_reverse_iterator {
        return data_.rbegin();
    }

    [[nodiscard]] constexpr auto crbegin() const noexcept -> const_reverse_iterator {
        return data_.crbegin();
    }

    constexpr auto rend() noexcept -> reverse_iterator {
        return data_.rend();
    }

    [[nodiscard]] constexpr auto rend() const noexcept -> const_reverse_iterator {
        return data_.rend();
    }

    [[nodiscard]] constexpr auto crend() const noexcept -> const_reverse_iterator {
        return data_.crend();
    }

    constexpr auto data() noexcept -> pointer {
        return data_.data();
    }

    [[nodiscard]] constexpr auto data() const noexcept -> const_pointer {
        return data_.data();
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return data_.empty();
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_type {
        return data_.size();
    }

    constexpr auto fill(xbyte_t const value) noexcept -> void {
        data_.fill(value);
    }

    [[nodiscard]] constexpr static auto endian() noexcept -> std::endian {
        return Endian;
    }

private:
    template <std::unsigned_integral T>
    constexpr static void to_little_endian(T const value, internal_type & data) {
        auto const bytes = std::bit_cast<std::array<xbyte_t, sizeof(T)>>(value);
        if constexpr (std::endian::native == std::endian::little) {
            std::ranges::copy_n(bytes.begin(), std::min(bytes.size(), Size), data.begin());
        }

        if constexpr (std::endian::native == std::endian::big) {
            std::ranges::copy_n(bytes | std::views::reverse, std::min(bytes.size(), Size), data.begin());
        }
    }

    template <std::unsigned_integral T>
    constexpr static void to_big_endian(T const value, internal_type & data) {
        auto bytes = std::bit_cast<std::array<xbyte_t, sizeof(T)>>(value);
        if constexpr (std::endian::native == std::endian::little) {
            // std::ranges::copy_n(bytes | std::views::reverse, std::min(bytes.size(), Size), data.begin());
            std::ranges::reverse(bytes);
            std::ranges::copy_n(bytes.begin(), std::min(bytes.size(), Size), data.begin());
        }

        if constexpr (std::endian::native == std::endian::big) {
            std::ranges::copy_n(bytes, std::min(bytes.size(), Size), data.begin());
        }
    }
};

}


#endif // !defined(ABC_DETAILS_FIXED_BYTES)
