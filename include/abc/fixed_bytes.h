// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_FIXED_BYTES)
#define ABC_FIXED_BYTES

#pragma once

#include "abc/byte.h"
#include "abc/byte_bit_numbering.h"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <ranges>

namespace abc {

template <size_t N, byte_numbering ByteNumbering>
class [[nodiscard]] fixed_bytes {
    static_assert(N > 0);
    static_assert(ByteNumbering == byte_numbering::lsb0 || ByteNumbering == byte_numbering::msb0);

private:
    using internal_type = std::array<byte, N>;
    internal_type data_;

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

    fixed_bytes(fixed_bytes const &) = default;
    auto operator=(fixed_bytes const &) -> fixed_bytes & = default;
    fixed_bytes(fixed_bytes &&) = default;
    auto operator=(fixed_bytes &&) -> fixed_bytes & = default;
    ~fixed_bytes() = default;

    constexpr fixed_bytes() {
        data_.fill(0);
    }

    template <std::enable_if_t<ByteNumbering == byte_numbering::lsb0 || ByteNumbering == byte_numbering::msb0> * = nullptr>
    constexpr fixed_bytes(std::unsigned_integral auto const value) : fixed_bytes{} {
        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            to_little_endian(value, data_);
        }

        if constexpr (ByteNumbering == byte_numbering::msb0) {
            to_big_endian(value, data_);
        }
    }

    constexpr explicit fixed_bytes(internal_type const & data) : data_{ data } {
    }

    constexpr explicit fixed_bytes(std::array<std::byte, N> const & data) : fixed_bytes{} {
        std::ranges::copy(data | std::views::transform([](auto const byte) { return std::to_integer<byte>(byte); }), data_.begin());
    }

    friend auto operator==(fixed_bytes const &, fixed_bytes const &) noexcept -> bool = default;
    friend auto operator<=>(fixed_bytes const &, fixed_bytes const &) noexcept -> std::strong_ordering = default;

    constexpr auto operator[](size_t const index) const noexcept -> byte {
        return data_[index];
    }

    constexpr auto operator[](size_t const index) noexcept -> byte & {
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

    constexpr auto fill(byte const value) noexcept -> void {
        data_.fill(value);
    }

    [[nodiscard]] constexpr static auto byte_numbering() noexcept -> byte_numbering {
        return ByteNumbering;
    }

    template <std::unsigned_integral T>
    [[nodiscard]] constexpr auto to() const noexcept -> T {
        T result{};
        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            for (auto i = 0u; i < std::min(sizeof(T), data_.size()); ++i) {
                result |= static_cast<T>(data_[i]) << (i * 8);
            }
        }

        if constexpr (ByteNumbering == byte_numbering::msb0) {
            for (auto i = 0u; i < std::min(sizeof(T), data_.size()); ++i) {
                result |= static_cast<T>(data_[data_.size() - i - 1]) << (i * 8);
            }
        }
        return result;
    }

    [[nodiscard]] constexpr auto zero() const noexcept -> bool {
        return std::ranges::all_of(data_, [](auto const byte) { return byte == 0; });
    }

    constexpr auto operator^=(fixed_bytes const & other) -> fixed_bytes & {
        for (auto i = 0u; i < N; ++i) {
            data_[i] ^= other.data_[i];
        }
        return *this;
    }

    constexpr auto operator^(fixed_bytes const & other) const -> fixed_bytes {
        return fixed_bytes{ *this } ^= other;
    }

    constexpr auto operator|=(fixed_bytes const & other) -> fixed_bytes & {
        for (auto i = 0u; i < N; ++i) {
            data_[i] |= other.data_[i];
        }
        return *this;
    }

    constexpr auto operator|(fixed_bytes const & other) const -> fixed_bytes {
        return fixed_bytes{ *this } |= other;
    }

    constexpr auto operator&=(fixed_bytes const & other) -> fixed_bytes & {
        for (auto i = 0u; i < N; ++i) {
            data_[i] &= other.data_[i];
        }
        return *this;
    }

    constexpr auto operator&(fixed_bytes const & other) const -> fixed_bytes {
        return fixed_bytes{ *this } &= other;
    }

    constexpr auto operator~() const -> fixed_bytes {
        fixed_bytes ret;
        for (auto i = 0u; i < N; ++i) {
            ret[i] = ~data_[i];
        }
        return ret;
    }

    constexpr auto operator++() -> fixed_bytes & {
        if constexpr (ByteNumbering == byte_numbering::msb0) {
            for (auto i = N; i > 0 && !++data_[--i];) {}
            return *this;
        }

        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            for (auto i = 0u; i < N && !++data_[i++];) {}
            return *this;
        }
    }

//    constexpr auto operator++(int) -> fixed_bytes {
//        fixed_bytes ret{*this};
//
//        if constexpr (ByteNumbering == byte_numbering::msb0) {
//            for (auto i = N; i > 0 && !++data_[--i];) {}
//            return ret;
//        }
//
//        if constexpr (ByteNumbering == byte_numbering::lsb0) {
//            for (auto i = 0u; i < N && !++data_[i++];) {}
//            return ret;
//        }
//    }

    constexpr auto clear() -> void {
        data_.fill(0);
    }

private:
    template <std::unsigned_integral T>
    constexpr static void to_little_endian(T value, internal_type & data) {
        for (auto i = 0u; i < std::min(sizeof(T), data.size()); value >>= 8, ++i) {
            T v = value & static_cast<T>(0xff);
            data[i] = static_cast<uint8_t>(v);
        }
    }

    template <std::unsigned_integral T>
    constexpr static void to_big_endian(T value, internal_type & data) {
        for (auto i = data.size(); i != 0; value >>= 8, --i) {
            T v = value & static_cast<T>(0xff);
            data[i - 1] = static_cast<uint8_t>(v);
        }
    }
};

using bytes4_msb0_t = fixed_bytes<4, byte_numbering::msb0>;
using bytes8_msb0_t = fixed_bytes<8, byte_numbering::msb0>;
using bytes16_msb0_t = fixed_bytes<16, byte_numbering::msb0>;
using bytes32_msb0_t = fixed_bytes<32, byte_numbering::msb0>;

using bytes4_be_t = bytes4_msb0_t;
using bytes8_be_t = bytes8_msb0_t;
using bytes16_be_t = bytes16_msb0_t;
using bytes32_be_t = bytes32_msb0_t;

using bytes4_lsb0_t = fixed_bytes<4, byte_numbering::lsb0>;
using bytes8_lsb0_t = fixed_bytes<8, byte_numbering::lsb0>;
using bytes16_lsb0_t = fixed_bytes<16, byte_numbering::lsb0>;
using bytes32_lsb0_t = fixed_bytes<32, byte_numbering::lsb0>;

using bytes4_le_t = bytes4_lsb0_t;
using bytes8_le_t = bytes8_lsb0_t;
using bytes16_le_t = bytes16_lsb0_t;
using bytes32_le_t = bytes32_lsb0_t;

}

#endif // ABC_FIXED_BYTES
