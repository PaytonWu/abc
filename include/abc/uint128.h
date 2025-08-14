// uint128_t.h
// An unsigned 128 bit integer type for C++
//
// Copyright (c) 2013 - 2017 Jason Lee @ calccrypto at gmail.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
// With much help from Auston Sterling
//
// Thanks to Stefan Deigmüller for finding
// a bug in operator*.
//
// Thanks to François Dessenne for convincing me
// to do a general rewrite of this class.

// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_UINT128
#define ABC_INCLUDE_ABC_UINT128

#pragma once

#if !defined(__cplusplus) || __cplusplus < 202002L
#error "C++20 or above is required"
#endif

#include "bytes.h"
#include "details/uint128_storage.h"
#include "expected.h"
#include "hex_string.h"

#include <algorithm>
#include <bit>
#include <cassert>
#include <cctype>
#include <compare>
#include <concepts>
#include <cstdint>
#include <ostream>
#include <span>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

namespace abc
{

class uint128_t;

// The behavior of a program that adds specializations for std::is_integral or std::is_integral_v is undefined.
// https://en.cppreference.com/w/cpp/types/is_integral
// remove is_integral<T>, is_unsigned<T>, is_arithmetic<T> specializations.

class [[nodiscard]] uint128_t : private details::uint128_storage
{
public:
    using details::uint128_storage::byte_numbering;
    using details::uint128_storage::endian;
    using details::uint128_storage::uint128_storage;

    uint128_t() = default;

    // do not use prefixes (0x, 0b, etc.)
    // if the input string is too long, only right most characters are read
    constexpr uint128_t(std::string const & s, uint8_t const base)
    {
        init(s.c_str(), s.size(), base);
    }

    constexpr uint128_t(char const * s, std::size_t const len, uint8_t const base)
    {
        init(s, len, base);
    }

    constexpr explicit uint128_t(bool const b) : uint128_t{ static_cast<uint8_t>(b) }
    {
    }

    constexpr uint128_t(std::integral auto const upper_rhs, std::integral auto const lower_rhs) noexcept
        : details::uint128_storage{ static_cast<std::uint64_t>(upper_rhs), static_cast<std::uint64_t>(lower_rhs) }
    {
    }

    static auto
    from(hex_string const & hex_string) -> uint128_t
    {
        static_assert(uint128_t::byte_numbering() == abc::ByteNumbering::Lsb0 || uint128_t::byte_numbering() == abc::ByteNumbering::Msb0);

        auto && bytes = hex_string.bytes<uint128_t::byte_numbering()>();
        if constexpr (ByteNumbering::Lsb0 == uint128_t::byte_numbering())
        {
            uint128_t ret;
            std::memcpy(&ret, bytes.data(), std::min(bytes.size(), sizeof(ret)));
            return ret;
        }

        if constexpr (ByteNumbering::Msb0 == uint128_t::byte_numbering())
        {
            uint128_t ret;
            if (bytes.size() >= sizeof(uint128_t))
            {
                std::memcpy(&ret, bytes.data() + bytes.size() - sizeof(uint128_t), sizeof(uint128_t));
            }
            else
            {
                auto * dst = reinterpret_cast<byte_t *>(&ret);
                dst += (sizeof(uint128_t) - bytes.size());
                std::memcpy(dst, bytes.data(), bytes.size());
            }
            return ret;
        }
    }

    template <abc::ByteNumbering ByteNumberingV>
    static auto
    from(bytes_view<ByteNumberingV> const bytes) -> expected<uint128_t, std::error_code>
    {
        if (bytes.size() > sizeof(uint128_t))
        {
            return make_unexpected(std::make_error_code(std::errc::invalid_argument));
        }

        if constexpr (uint128_t::byte_numbering() == abc::ByteNumbering::Lsb0)
        {
            if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
            {
                uint128_t ret;
                std::memcpy(&ret, bytes.data(), bytes.size());
                return ret;
            }

            if constexpr (ByteNumberingV == ByteNumbering::Msb0)
            {
                uint128_t ret;
                auto * dst = reinterpret_cast<byte_t *>(&ret);

                ranges::copy(bytes | ranges::views::reverse, dst);
                return ret;
            }

            unreachable();
        }

        if constexpr (uint128_t::byte_numbering() == abc::ByteNumbering::Msb0)
        {
            if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
            {
                uint128_t ret;
                auto * dst = reinterpret_cast<byte_t *>(&ret);
                dst += sizeof(uint128_t);

                ranges::copy_backward(bytes | ranges::views::reverse, dst);
                return ret;
            }

            if constexpr (ByteNumberingV == ByteNumbering::Msb0)
            {
                uint128_t ret;
                auto * dst = reinterpret_cast<byte_t *>(&ret);
                dst += (sizeof(uint128_t) - bytes.size());
                std::memcpy(dst, bytes.data(), bytes.size());
                return ret;
            }

            unreachable();
        }

        unreachable();
    }

    template <std::integral T>
    constexpr auto
    operator=(T const rhs) -> uint128_t &
    {
        this->upper_ = 0;

        if constexpr (std::is_signed_v<T>)
        {
            if (rhs < 0)
            {
                this->upper_ = -1;
            }
        }

        this->lower_ = rhs;
        return *this;
    }

    constexpr auto
    operator=(bool const rhs) -> uint128_t &
    {
        this->upper_ = 0;
        this->lower_ = rhs;
        return *this;
    }

    // Typecast Operators
    constexpr explicit
    operator bool() const noexcept
    {
        return static_cast<bool>(this->upper_ | this->lower_);
    }

    constexpr explicit
    operator uint8_t() const noexcept
    {
        return static_cast<uint8_t>(this->lower_);
    }

    constexpr explicit
    operator uint16_t() const noexcept
    {
        return static_cast<uint16_t>(this->lower_);
    }

    constexpr explicit
    operator uint32_t() const noexcept
    {
        return static_cast<uint32_t>(this->lower_);
    }

    constexpr explicit
    operator uint64_t() const noexcept
    {
        return this->lower_;
    }

    // Bitwise Operators
    constexpr auto
    operator&(uint128_t const rhs) const noexcept -> uint128_t
    {
        return { this->upper_ & rhs.upper_, this->lower_ & rhs.lower_ };
    }

    constexpr auto
    operator&(std::integral auto const rhs) const noexcept -> uint128_t
    {
        return { 0, this->lower_ & static_cast<uint64_t>(rhs) };
    }

    constexpr auto
    operator&=(uint128_t const rhs) noexcept -> uint128_t &
    {
        this->upper_ &= rhs.upper_;
        this->lower_ &= rhs.lower_;
        return *this;
    }

    constexpr auto
    operator&=(std::integral auto const rhs) noexcept -> uint128_t &
    {
        this->upper_ = 0;
        this->lower_ &= rhs;
        return *this;
    }

    constexpr auto
    operator|(uint128_t const rhs) const noexcept -> uint128_t
    {
        return { this->upper_ | rhs.upper_, this->lower_ | rhs.lower_ };
    }

    constexpr auto
    operator|(std::integral auto const rhs) const noexcept -> uint128_t
    {
        return { this->upper_, this->lower_ | static_cast<uint64_t>(rhs) };
    }

    constexpr auto
    operator|=(uint128_t const rhs) noexcept -> uint128_t &
    {
        this->upper_ |= rhs.upper_;
        this->lower_ |= rhs.lower_;
        return *this;
    }

    constexpr auto
    operator|=(std::integral auto const rhs) noexcept -> uint128_t &
    {
        this->lower_ |= static_cast<uint64_t>(rhs);
        return *this;
    }

    constexpr auto
    operator^(uint128_t const rhs) const noexcept -> uint128_t
    {
        return { this->upper_ ^ rhs.upper_, this->lower_ ^ rhs.lower_ };
    }

    constexpr auto
    operator^(std::integral auto const rhs) const noexcept -> uint128_t
    {
        return { this->upper_, this->lower_ ^ static_cast<uint64_t>(rhs) };
    }

    constexpr auto
    operator^=(uint128_t const rhs) noexcept -> uint128_t &
    {
        this->upper_ ^= rhs.upper_;
        this->lower_ ^= rhs.lower_;
        return *this;
    }

    constexpr auto
    operator^=(std::integral auto const rhs) noexcept -> uint128_t &
    {
        this->lower_ ^= static_cast<uint64_t>(rhs);
        return *this;
    }

    constexpr auto
    operator~() const noexcept -> uint128_t
    {
        return { ~this->upper_, ~this->lower_ };
    }

    //    constexpr void export_bits(std::span<uint8_t> const ret) const noexcept {
    //        assert(ret.size() >= 16);
    //
    //        convert_to_span_big_endian(this->upper_, ret);
    //        convert_to_span_big_endian(this->lower_, ret.subspan(8));
    //    }

    template <abc::ByteNumbering ByteNumberingV>
    constexpr void
    export_bits(BasicBytes<ByteNumberingV> & ret) const
    {
        if constexpr (ByteNumberingV == ByteNumbering::Msb0)
        {
#if !defined(NDEBUG)
            auto const size_before_export = ret.size();
#endif
            convert_to_bytes<ByteNumberingV>(this->upper_, ret);
            convert_to_bytes<ByteNumberingV>(this->lower_, ret);

            assert(ret.size() - size_before_export == 16);
        }
        else if constexpr (ByteNumberingV == ByteNumbering::Lsb0)
        {
#if !defined(NDEBUG)
            auto const size_before_export = ret.size();
#endif
            convert_to_bytes_le(this->lower_, ret);
            convert_to_bytes_le(this->upper_, ret);

            assert(ret.size() - size_before_export == 16);
        }
        else
        {
            unreachable();
        }
    }

    template <abc::ByteNumbering ByteNumberingV>
    [[nodiscard]] constexpr auto
    export_bits() const noexcept -> BasicBytes<ByteNumberingV>
    {
        BasicBytes<ByteNumberingV> ret;
        ret.reserve(16);
        export_bits(ret);
        return ret;
    }

    //    constexpr auto export_bits_compact(std::span<uint8_t> const ret) const noexcept -> size_t {
    //        assert(ret.size() >= 16);
    //        auto tmp = export_bits_compact();
    //        std::ranges::copy(tmp, std::begin(ret));
    //        return tmp.size();
    //    }

    template <abc::ByteNumbering ByteNumberingV>
    void
    export_bits_compact(BasicBytes<ByteNumberingV> & ret) const noexcept
    {
        auto leading_zeros_bits_count = std::countl_zero(this->upper_);
        if (leading_zeros_bits_count == 64)
        {
            leading_zeros_bits_count = std::countl_zero(this->lower_);
            std::size_t const leading_zero_bytes_count = (leading_zeros_bits_count / 8);

            ret.reserve(leading_zero_bytes_count);

            convert_to_bytes<ByteNumberingV>(this->lower_, leading_zero_bytes_count, ret);
        }
        else
        {
            std::size_t const leading_zero_bytes_count = (leading_zeros_bits_count / 8);
            ret.reserve(leading_zero_bytes_count + 8);

            convert_to_bytes<ByteNumberingV>(this->upper_, leading_zero_bytes_count, ret);
            convert_to_bytes<ByteNumberingV>(this->lower_, ret);
        }
    }

    template <abc::ByteNumbering ByteNumberingV>
    [[nodiscard]] constexpr auto
    export_bits_compact() const -> BasicBytes<ByteNumberingV>
    {
        BasicBytes<ByteNumberingV> ret;
        ret.reserve(16);
        export_bits_compact(ret);

        return ret;
    }

    // Bit Shift Operators
    constexpr auto
    operator<<(uint128_t const rhs) const noexcept -> uint128_t
    {
        if (uint64_t const shift = rhs.lower_; static_cast<bool>(rhs.upper_) || (shift >= 128))
        {
            return { 0 };
        }
        else
        {
            if (shift == 64)
            {
                return { this->lower_, 0u };
            }

            if (shift == 0)
            {
                return *this;
            }

            if (shift < 64)
            {
                return { (this->upper_ << shift) + (this->lower_ >> (64 - shift)), this->lower_ << shift };
            }

            if ((128 > shift) && (shift > 64))
            {
                return { this->lower_ << (shift - 64), 0u };
            }

            return { 0 };
        }
    }

    constexpr auto
    operator<<(std::integral auto const rhs) const noexcept -> uint128_t
    {
        return *this << uint128_t{ rhs };
    }

    constexpr auto
    operator<<=(uint128_t const rhs) noexcept -> uint128_t &
    {
        *this = *this << rhs;
        return *this;
    }

    constexpr auto
    operator<<=(std::integral auto const rhs) noexcept -> uint128_t &
    {
        *this = *this << uint128_t{ rhs };
        return *this;
    }

    constexpr auto
    operator>>(uint128_t const rhs) const noexcept -> uint128_t
    {
        if (uint64_t const shift = rhs.lower_; static_cast<bool>(rhs.upper_) || (shift >= 128))
        {
            return { 0 };
        }
        else
        {
            if (shift == 64)
            {
                return { 0u, this->upper_ };
            }
            if (shift == 0)
            {
                return *this;
            }
            if (shift < 64)
            {
                return { this->upper_ >> shift, (this->upper_ << (64 - shift)) + (this->lower_ >> shift) };
            }
            if ((128 > shift) && (shift > 64))
            {
                return { 0u, (this->upper_ >> (shift - 64)) };
            }
            return { 0 };
        }
    }

    constexpr auto
    operator>>(std::integral auto const rhs) const noexcept -> uint128_t
    {
        return *this >> uint128_t{ rhs };
    }

    constexpr auto
    operator>>=(uint128_t const rhs) noexcept -> uint128_t &
    {
        *this = *this >> rhs;
        return *this;
    }

    constexpr auto
    operator>>=(std::integral auto const rhs) noexcept -> uint128_t &
    {
        *this = *this >> uint128_t{ rhs };
        return *this;
    }

    // Logical Operators
    constexpr bool
    operator!() const noexcept
    {
        return !static_cast<bool>(this->upper_ | this->lower_);
    }

    constexpr bool
    operator&&(uint128_t const rhs) const noexcept
    {
        return (static_cast<bool>(*this) && rhs);
    }

    constexpr bool
    operator||(uint128_t const rhs) const noexcept
    {
        return (static_cast<bool>(*this) || rhs);
    }

    constexpr bool
    operator&&(std::integral auto const rhs) const noexcept
    {
        return static_cast<bool>(*this && rhs);
    }

    constexpr bool
    operator||(std::integral auto const rhs) const noexcept
    {
        return static_cast<bool>(*this || rhs);
    }

    // Comparison Operators
    constexpr auto operator==(uint128_t const & rhs) const noexcept -> bool = default;

    constexpr auto operator<=>(uint128_t const & rhs) const -> std::strong_ordering = default;

    constexpr bool
    operator==(std::integral auto const rhs) const noexcept
    {
        return !this->upper_ && this->lower_ == static_cast<uint64_t>(rhs);
    }

    constexpr auto
    operator<=>(std::integral auto const rhs) const noexcept -> std::strong_ordering
    {
        if (this->upper_ == 0)
        {
            return this->lower_ <=> static_cast<uint64_t>(rhs);
        }

        return std::strong_ordering::greater;
    }

    // Arithmetic Operators
    constexpr auto
    operator+(uint128_t const rhs) const noexcept -> uint128_t
    {
        return { this->upper_ + rhs.upper_ + ((this->lower_ + rhs.lower_) < this->lower_), this->lower_ + rhs.lower_ };
    }

    constexpr auto
    operator+(std::integral auto const rhs) const noexcept -> uint128_t
    {
        return { this->upper_ + ((this->lower_ + static_cast<uint64_t>(rhs)) < this->lower_), this->lower_ + static_cast<uint64_t>(rhs) };
    }

    constexpr auto
    operator+=(uint128_t const rhs) noexcept -> uint128_t &
    {
        this->upper_ += rhs.upper_ + ((this->lower_ + rhs.lower_) < this->lower_);
        this->lower_ += rhs.lower_;
        return *this;
    }

    constexpr auto
    operator+=(std::integral auto const rhs) noexcept -> uint128_t &
    {
        return *this += uint128_t{ rhs };
    }

    constexpr auto
    operator-(uint128_t const rhs) const noexcept -> uint128_t
    {
        return { this->upper_ - rhs.upper_ - ((this->lower_ - rhs.lower_) > this->lower_), this->lower_ - rhs.lower_ };
    }

    constexpr auto
    operator-(std::integral auto const rhs) const noexcept -> uint128_t
    {
        return { static_cast<uint64_t>(this->upper_ - ((this->lower_ - rhs) > this->lower_)), static_cast<uint64_t>(this->lower_ - rhs) };
    }

    constexpr auto
    operator-=(uint128_t const rhs) noexcept -> uint128_t &
    {
        *this = *this - rhs;
        return *this;
    }

    constexpr auto
    operator-=(std::integral auto const rhs) noexcept -> uint128_t &
    {
        return *this = *this - uint128_t(rhs);
    }

    constexpr auto
    operator*(uint128_t const rhs) const noexcept -> uint128_t
    {
        // split values into 4 32-bit parts
        uint64_t const top[4] = { this->upper_ >> 32, this->upper_ & 0xffffffff, this->lower_ >> 32, this->lower_ & 0xffffffff };
        uint64_t const bottom[4] = { rhs.upper_ >> 32, rhs.upper_ & 0xffffffff, rhs.lower_ >> 32, rhs.lower_ & 0xffffffff };
        uint64_t products[4][4];

        // multiply each component of the values
        for (int y = 3; y > -1; y--)
        {
            for (int x = 3; x > -1; x--)
            {
                products[3 - x][y] = top[x] * bottom[y];
            }
        }

        // first row
        uint64_t fourth32 = (products[0][3] & 0xffffffff);
        uint64_t third32 = (products[0][2] & 0xffffffff) + (products[0][3] >> 32);
        uint64_t second32 = (products[0][1] & 0xffffffff) + (products[0][2] >> 32);
        uint64_t first32 = (products[0][0] & 0xffffffff) + (products[0][1] >> 32);

        // second row
        third32 += (products[1][3] & 0xffffffff);
        second32 += (products[1][2] & 0xffffffff) + (products[1][3] >> 32);
        first32 += (products[1][1] & 0xffffffff) + (products[1][2] >> 32);

        // third row
        second32 += (products[2][3] & 0xffffffff);
        first32 += (products[2][2] & 0xffffffff) + (products[2][3] >> 32);

        // fourth row
        first32 += (products[3][3] & 0xffffffff);

        // move carry to next digit
        third32 += fourth32 >> 32;
        second32 += third32 >> 32;
        first32 += second32 >> 32;

        // remove carry from current digit
        fourth32 &= 0xffffffff;
        third32 &= 0xffffffff;
        second32 &= 0xffffffff;
        first32 &= 0xffffffff;

        // combine components
        return { (first32 << 32) | second32, (third32 << 32) | fourth32 };
    }

    constexpr auto
    operator*(std::integral auto const & rhs) const noexcept -> uint128_t
    {
        return *this * uint128_t{ rhs };
    }

    constexpr auto
    operator*=(uint128_t const rhs) noexcept -> uint128_t &
    {
        *this = *this * rhs;
        return *this;
    }

    constexpr auto
    operator*=(std::integral auto const rhs) noexcept -> uint128_t &
    {
        return *this = *this * uint128_t(rhs);
    }

private:
    [[nodiscard]] constexpr static auto
    divmod(uint128_t const lhs, uint128_t const rhs) -> std::pair<uint128_t, uint128_t>
    {
        // Save some calculations /////////////////////
        if (rhs == uint128_t{ 0 })
        {
            throw std::domain_error("Error: division or modulus by 0");
        }

        if (rhs == uint128_t{ 1 })
        {
            return { lhs, uint128_t{ 0 } };
        }

        if (lhs == rhs)
        {
            return { uint128_t{ 1 }, uint128_t{ 0 } };
        }

        if ((lhs == uint128_t{ 0 }) || (lhs < rhs))
        {
            return { uint128_t{ 0 }, lhs };
        }

        std::pair qr{ uint128_t{ 0 }, uint128_t{ 0 } };
        for (uint8_t x = lhs.bits(); x > 0; x--)
        {
            qr.first <<= uint128_t{ 1 };
            qr.second <<= uint128_t{ 1 };

            if ((lhs >> (x - 1U)) & 1)
            {
                ++qr.second;
            }

            if (qr.second >= rhs)
            {
                qr.second -= rhs;
                ++qr.first;
            }
        }
        return qr;
    }

    // do not use prefixes (0x, 0b, etc.)
    // if the input string is too long, only right most characters are read
    constexpr void
    init(char const * s, std::size_t len, uint8_t const base)
    {
        if (s == nullptr || !len || s[0] == '\x00')
        {
            this->lower_ = this->upper_ = 0;
            return;
        }

        while (len && *s && std::isspace(*s))
        {
            ++s;
            len--;
        }

        // no prefixes
        switch (base)
        {
            case 16:
                init_hex(s, len);
                break;
            case 10:
                init_dec(s, len);
                break;
            case 8:
                init_oct(s, len);
                break;
            case 2:
                init_bin(s, len);
                break;
            default:
                // should probably throw error here
                break;
        }
    }

    constexpr void
    init_hex(char const * s, std::size_t const len)
    {
        // 2**128 = 0x100000000000000000000000000000000.
        constexpr std::size_t MAX_LEN = 32;

        this->lower_ = this->upper_ = 0;
        if (!s || !len)
        {
            return;
        }

        std::size_t const max_len = std::min(len, MAX_LEN);
        std::size_t const starting_index = (MAX_LEN < len) ? (len - MAX_LEN) : 0;
        std::size_t constexpr double_lower = sizeof(this->lower_) * 2;
        std::size_t const lower_len = (max_len >= double_lower) ? double_lower : max_len;
        std::size_t const upper_len = (max_len >= double_lower) ? (max_len - double_lower) : 0;

        std::stringstream lower_s, upper_s;
        upper_s << std::hex << std::string(s + starting_index, upper_len);
        lower_s << std::hex << std::string(s + starting_index + upper_len, lower_len);

        // should check for errors
        upper_s >> this->upper_;
        lower_s >> this->lower_;
    }

    constexpr void
    init_dec(char const * s, std::size_t const len)
    {
        // 2**128 = 340282366920938463463374607431768211456.
        constexpr std::size_t MAX_LEN = 39;

        this->lower_ = this->upper_ = 0;
        if (!s || !len)
        {
            return;
        }

        std::size_t const max_len = std::min(len, MAX_LEN);
        std::size_t const starting_index = (MAX_LEN < len) ? (len - MAX_LEN) : 0;
        s += starting_index;

        for (std::size_t i = 0; *s && ('0' <= *s) && (*s <= '9') && (i < max_len); ++s, ++i)
        {
            *this *= 10;
            *this += *s - '0';
        }
    }

    constexpr void
    init_oct(char const * s, std::size_t const len)
    {
        // 2**128 = 0o4000000000000000000000000000000000000000000.
        constexpr std::size_t MAX_LEN = 43;

        this->lower_ = this->upper_ = 0;
        if (!s || !len)
        {
            return;
        }

        std::size_t const max_len = std::min(len, MAX_LEN);
        std::size_t const starting_index = (MAX_LEN < len) ? (len - MAX_LEN) : 0;
        s += starting_index;

        for (std::size_t i = 0; *s && ('0' <= *s) && (*s <= '7') && (i < max_len); ++s, ++i)
        {
            *this *= 8;
            *this += *s - '0';
        }
    }

    constexpr void
    init_bin(char const * s, std::size_t const len)
    {
        // 2**128 = 0x100000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.
        constexpr std::size_t MAX_LEN = 128;

        this->lower_ = this->upper_ = 0;
        if (!s || !len)
        {
            return;
        }

        std::size_t const max_len = std::min(len, MAX_LEN);
        std::size_t const starting_index = (MAX_LEN < len) ? (len - MAX_LEN) : 0;
        std::size_t constexpr eight_lower = sizeof(this->lower_) * 8;
        std::size_t const lower_len = (max_len >= eight_lower) ? eight_lower : max_len;
        std::size_t const upper_len = (max_len >= eight_lower) ? (max_len - eight_lower) : 0;
        s += starting_index;

        for (std::size_t i = 0; *s && ('0' <= *s) && (*s <= '1') && (i < upper_len); ++s, ++i)
        {
            this->upper_ <<= 1;
            this->upper_ |= *s - '0';
        }

        for (std::size_t i = 0; *s && ('0' <= *s) && (*s <= '1') && (i < lower_len); ++s, ++i)
        {
            this->lower_ <<= 1;
            this->lower_ |= *s - '0';
        }
    }

    //    constexpr static void
    //    convert_to_span_big_endian(uint64_t const val, std::span<uint8_t> ret)
    //    {
    //        ret[0] = static_cast<uint8_t>(val >> 56);
    //        ret[1] = static_cast<uint8_t>(val >> 48);
    //        ret[2] = static_cast<uint8_t>(val >> 40);
    //        ret[3] = static_cast<uint8_t>(val >> 32);
    //        ret[4] = static_cast<uint8_t>(val >> 24);
    //        ret[5] = static_cast<uint8_t>(val >> 16);
    //        ret[6] = static_cast<uint8_t>(val >> 8);
    //        ret[7] = static_cast<uint8_t>(val);
    //    }

    template <abc::ByteNumbering ByteNumberingV>
    constexpr static void
    convert_to_bytes(uint64_t const val, BasicBytes<ByteNumberingV> & ret)
    {
        static_assert(ByteNumberingV == abc::ByteNumbering::Lsb0 || ByteNumberingV == abc::ByteNumbering::Msb0);

        if constexpr (ByteNumberingV == abc::ByteNumbering::Msb0)
        {
            ret.push_back(static_cast<uint8_t>(val >> 56));
            ret.push_back(static_cast<uint8_t>(val >> 48));
            ret.push_back(static_cast<uint8_t>(val >> 40));
            ret.push_back(static_cast<uint8_t>(val >> 32));
            ret.push_back(static_cast<uint8_t>(val >> 24));
            ret.push_back(static_cast<uint8_t>(val >> 16));
            ret.push_back(static_cast<uint8_t>(val >> 8));
            ret.push_back(static_cast<uint8_t>(val));

            return;
        }

        if constexpr (ByteNumberingV == abc::ByteNumbering::Lsb0)
        {
            ret.push_back(static_cast<uint8_t>(val));
            ret.push_back(static_cast<uint8_t>(val >> 8));
            ret.push_back(static_cast<uint8_t>(val >> 16));
            ret.push_back(static_cast<uint8_t>(val >> 24));
            ret.push_back(static_cast<uint8_t>(val >> 32));
            ret.push_back(static_cast<uint8_t>(val >> 40));
            ret.push_back(static_cast<uint8_t>(val >> 48));
            ret.push_back(static_cast<uint8_t>(val >> 56));

            return;
        }
    }

    template <abc::ByteNumbering ByteNumberingV>
    constexpr static void
    convert_to_bytes(uint64_t const value, std::size_t leading_zero_bytes_count, BasicBytes<ByteNumberingV> & ret)
    {
        static_assert(ByteNumberingV == abc::ByteNumbering::Lsb0 || ByteNumberingV == abc::ByteNumbering::Msb0);

        if constexpr (ByteNumberingV == abc::ByteNumbering::Msb0)
        {
            assert(leading_zero_bytes_count <= 8);
            switch (leading_zero_bytes_count)
            {
                case 0:
                    ret.push_back(static_cast<uint8_t>(value >> 56));
                    [[fallthrough]];
                case 1:
                    ret.push_back(static_cast<uint8_t>(value >> 48));
                    [[fallthrough]];
                case 2:
                    ret.push_back(static_cast<uint8_t>(value >> 40));
                    [[fallthrough]];
                case 3:
                    ret.push_back(static_cast<uint8_t>(value >> 32));
                    [[fallthrough]];
                case 4:
                    ret.push_back(static_cast<uint8_t>(value >> 24));
                    [[fallthrough]];
                case 5:
                    ret.push_back(static_cast<uint8_t>(value >> 16));
                    [[fallthrough]];
                case 6:
                    ret.push_back(static_cast<uint8_t>(value >> 8));
                    [[fallthrough]];
                case 7:
                    ret.push_back(static_cast<uint8_t>(value));
                    [[fallthrough]];
                default:
                    break;
            }

            return;
        }

        if constexpr (ByteNumberingV == abc::ByteNumbering::Lsb0)
        {
            assert(leading_zero_bytes_count <= 8);

            switch (std::size_t const tail_non_zero_bytes_count = 8 - leading_zero_bytes_count)
            {
                case 8:
                    ret.push_back(static_cast<uint8_t>(value));
                    ret.push_back(static_cast<uint8_t>(value >> 8));
                    ret.push_back(static_cast<uint8_t>(value >> 16));
                    ret.push_back(static_cast<uint8_t>(value >> 24));
                    ret.push_back(static_cast<uint8_t>(value >> 32));
                    ret.push_back(static_cast<uint8_t>(value >> 40));
                    ret.push_back(static_cast<uint8_t>(value >> 48));
                    ret.push_back(static_cast<uint8_t>(value >> 56));
                    break;

                case 7:
                    ret.push_back(static_cast<uint8_t>(value));
                    ret.push_back(static_cast<uint8_t>(value >> 8));
                    ret.push_back(static_cast<uint8_t>(value >> 16));
                    ret.push_back(static_cast<uint8_t>(value >> 24));
                    ret.push_back(static_cast<uint8_t>(value >> 32));
                    ret.push_back(static_cast<uint8_t>(value >> 40));
                    ret.push_back(static_cast<uint8_t>(value >> 48));
                    break;

                case 6:
                    ret.push_back(static_cast<uint8_t>(value));
                    ret.push_back(static_cast<uint8_t>(value >> 8));
                    ret.push_back(static_cast<uint8_t>(value >> 16));
                    ret.push_back(static_cast<uint8_t>(value >> 24));
                    ret.push_back(static_cast<uint8_t>(value >> 32));
                    ret.push_back(static_cast<uint8_t>(value >> 40));
                    break;

                case 5:
                    ret.push_back(static_cast<uint8_t>(value));
                    ret.push_back(static_cast<uint8_t>(value >> 8));
                    ret.push_back(static_cast<uint8_t>(value >> 16));
                    ret.push_back(static_cast<uint8_t>(value >> 24));
                    ret.push_back(static_cast<uint8_t>(value >> 32));
                    break;

                case 4:
                    ret.push_back(static_cast<uint8_t>(value));
                    ret.push_back(static_cast<uint8_t>(value >> 8));
                    ret.push_back(static_cast<uint8_t>(value >> 16));
                    ret.push_back(static_cast<uint8_t>(value >> 24));
                    break;

                case 3:
                    ret.push_back(static_cast<uint8_t>(value));
                    ret.push_back(static_cast<uint8_t>(value >> 8));
                    ret.push_back(static_cast<uint8_t>(value >> 16));
                    break;

                case 2:
                    ret.push_back(static_cast<uint8_t>(value));
                    ret.push_back(static_cast<uint8_t>(value >> 8));
                    break;

                case 1:
                    ret.push_back(static_cast<uint8_t>(value));
                    break;

                default:
                    break;
            }

            return;
        }
    }

public:
    constexpr auto
    operator/(uint128_t const rhs) const -> uint128_t
    {
        return divmod(*this, rhs).first;
    }

    constexpr auto
    operator/(std::integral auto const rhs) const -> uint128_t
    {
        return *this / uint128_t{ rhs };
    }

    constexpr auto
    operator/=(uint128_t const rhs) -> uint128_t &
    {
        return *this = *this / rhs;
    }

    constexpr auto
    operator/=(std::integral auto const rhs) -> uint128_t &
    {
        return *this = *this / uint128_t(rhs);
    }

    constexpr auto
    operator%(uint128_t const rhs) const -> uint128_t
    {
        return divmod(*this, rhs).second;
    }

    constexpr auto
    operator%(std::integral auto const rhs) const -> uint128_t
    {
        return *this % uint128_t(rhs);
    }

    constexpr auto
    operator%=(uint128_t const rhs) -> uint128_t &
    {
        return *this = *this % rhs;
    }

    constexpr auto
    operator%=(std::integral auto const rhs) -> uint128_t &
    {
        return *this = *this % uint128_t{ rhs };
    }

    // Increment Operator
    constexpr auto
    operator++() noexcept -> uint128_t &
    {
        return *this += uint128_t{ 1 };
    }

    constexpr auto
    operator++(int) noexcept -> uint128_t
    {
        uint128_t const temp{ *this };
        ++*this;
        return temp;
    }

    // Decrement Operator
    constexpr auto
    operator--() noexcept -> uint128_t &
    {
        return *this -= uint128_t{ 1 };
    }

    constexpr auto
    operator--(int) noexcept -> uint128_t
    {
        uint128_t const temp{ *this };
        --*this;
        return temp;
    }

    // Nothing done since promotion doesn't work here
    constexpr auto
    operator+() const noexcept -> uint128_t
    {
        return *this;
    }

    // two's complement
    constexpr auto
    operator-() const noexcept -> uint128_t
    {
        return ~*this + uint128_t{ 1 };
    }

    // Get private values
    [[nodiscard]] constexpr auto
    upper() const noexcept -> uint64_t
    {
        return this->upper_;
    }

    [[nodiscard]] constexpr auto
    lower() const noexcept -> uint64_t
    {
        return this->lower_;
    }

    // Get bit size of value
    [[nodiscard]] constexpr auto
    bits() const -> uint8_t
    {
        if (this->upper_)
        {
            return 64 + static_cast<uint8_t>(std::bit_width(this->upper_));
        }

        return static_cast<uint8_t>(std::bit_width(this->lower_));
    }

    // Get string representation of value
    [[nodiscard]] constexpr auto
    str(uint8_t const base = 10, unsigned int const len = 0) const -> std::string
    {
        if ((base < 2) || (base > 16))
        {
            throw std::invalid_argument("Base must be in the range [2, 16]");
        }

        std::string out;
        if (!(*this))
        {
            out = "0";
        }
        else
        {
            std::pair qr{ *this, uint128_t{ 0 } };
            do
            {
                qr = divmod(qr.first, base);
                out = "0123456789abcdef"[static_cast<uint8_t>(qr.second)] + out;
            } while (qr.first);
        }
        if (out.size() < len)
        {
            out = std::string(len - out.size(), '0') + out;
        }
        return out;
    }
};

// useful values
inline constexpr uint128_t uint128_0{ 0 };
inline constexpr uint128_t uint128_1{ 1 };

// lhs type T as first argument
// If the output is not a bool, casts to type T

// Bitwise Operators
constexpr auto
operator&(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t
{
    return rhs & lhs;
}

template <std::integral T>
constexpr auto
operator&=(T & lhs, uint128_t const rhs) noexcept -> T &
{
    return lhs = static_cast<T>(rhs & lhs);
}

constexpr auto
operator|(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t
{
    return rhs | lhs;
}

template <std::integral T>
constexpr auto
operator|=(T & lhs, uint128_t const rhs) noexcept -> T &
{
    return lhs = static_cast<T>(rhs | lhs);
}

constexpr auto
operator^(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t
{
    return rhs ^ lhs;
}

template <std::integral T>
constexpr auto
operator^=(T & lhs, uint128_t const rhs) noexcept -> T &
{
    return lhs = static_cast<T>(rhs ^ lhs);
}

// Bit shift operators
constexpr auto
operator<<(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t
{
    return uint128_t{ lhs } << rhs;
}

template <std::integral T>
constexpr auto
operator<<=(T & lhs, uint128_t const rhs) noexcept -> T &
{
    return lhs = static_cast<T>(uint128_t{ lhs } << rhs);
}

constexpr auto
operator>>(std::integral auto const lhs, uint128_t const rhs) noexcept -> uint128_t
{
    return uint128_t{ lhs } >> rhs;
}

template <std::integral T>
constexpr auto
operator>>=(T & lhs, uint128_t const rhs) noexcept -> T &
{
    return lhs = static_cast<T>(uint128_t{ lhs } >> rhs);
}

// Arithmetic Operators
constexpr auto
operator+(std::integral auto const lhs, uint128_t const rhs) -> uint128_t
{
    return rhs + lhs;
}

template <std::integral T>
constexpr auto
operator+=(T & lhs, uint128_t const rhs) -> T &
{
    return lhs = static_cast<T>(rhs + lhs);
}

constexpr auto
operator-(std::integral auto const lhs, uint128_t const rhs) -> uint128_t
{
    return -(rhs - lhs);
}

template <std::integral T>
constexpr auto
operator-=(T & lhs, uint128_t const rhs) -> T &
{
    return lhs = static_cast<T>(-(rhs - lhs));
}

constexpr auto
operator*(std::integral auto const lhs, uint128_t const rhs) -> uint128_t
{
    return rhs * lhs;
}

template <std::integral T>
constexpr auto
operator*=(T & lhs, uint128_t const rhs) -> T &
{
    return lhs = static_cast<T>(rhs * lhs);
}

constexpr auto
operator/(std::integral auto const lhs, uint128_t const rhs) -> uint128_t
{
    return uint128_t(lhs) / rhs;
}

template <std::integral T>
constexpr auto
operator/=(T & lhs, uint128_t const rhs) -> T &
{
    return lhs = static_cast<T>(uint128_t(lhs) / rhs);
}

constexpr auto
operator%(std::integral auto const lhs, uint128_t const rhs) -> uint128_t
{
    return uint128_t{ lhs } % rhs;
}

template <std::integral T>
constexpr auto
operator%=(T & lhs, uint128_t const rhs) -> T &
{
    return lhs = static_cast<T>(uint128_t{ lhs } % rhs);
}

// IO Operator
inline std::ostream &
operator<<(std::ostream & stream, uint128_t const rhs)
{
    if (stream.flags() & std::ios_base::oct)
    {
        stream << rhs.str(8);
    }
    else if (stream.flags() & std::ios_base::dec)
    {
        stream << rhs.str(10);
    }
    else if (stream.flags() & std::ios_base::hex)
    {
        stream << rhs.str(16);
    }
    return stream;
}

} // namespace abc

#endif // ABC_INCLUDE_ABC_UINT128
