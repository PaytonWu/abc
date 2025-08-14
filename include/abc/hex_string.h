// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_STRING)
#define ABC_HEX_STRING

#pragma once

#include "hex_string_decl.h"

#include "bytes.h"
#include "hex_utility.h"

#include <fmt/format.h>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>

#include <bit>
#include <cassert>
#include <span>

namespace abc
{

constexpr hex_string::const_reference::const_reference(hex_string const * str, size_t const nibble_index) noexcept
    : str_{ str }
    , byte_index_{ nibble_index / 2 }
    , high_{ static_cast<bool>(nibble_index % 2) }
{
    assert(str != nullptr);
    assert(byte_index_ < str->size());
}

constexpr hex_string::const_reference::operator char() const noexcept
{
    assert(str_ != nullptr);
    return hex_utility::lower_case_hex_digits[(str_->binary_data_[byte_index_] >> (static_cast<size_t>(high_) * 4)) & 0x0f];
}

constexpr hex_string::reference::reference(hex_string * str, size_t const nibble_index) noexcept : const_reference{ str, nibble_index }
{
}

inline auto
hex_string::reference::operator=(char const value) -> reference &
{
    assert(str_ != nullptr);

    if (!std::isxdigit(static_cast<unsigned char>(value)))
    {
        throw_error(make_error_code(errc::invalid_hex_char));
        return *this;
    }

    assert((value >= '0' && value <= '9') || (value >= 'a' && value <= 'f') || (value >= 'A' && value <= 'F'));
    hex_utility::hex_char_to_binary(value).transform([this](auto byte) {
        if (high_)
        {
            const_cast<hex_string *>(str_)->binary_data_[byte_index_] = (str_->binary_data_[byte_index_] & 0x0f) | (byte << 4);
        }
        else
        {
            const_cast<hex_string *>(str_)->binary_data_[byte_index_] = (str_->binary_data_[byte_index_] & 0xf0) | byte;
        }
    });

    return *this;
}

constexpr hex_string::hex_string(bytes_le_t && input) noexcept : binary_data_{ std::move(input) }
{
}

constexpr hex_string::hex_string(bytes_le_view_t const input) noexcept : binary_data_{ input }
{
}

template <ByteNumbering ByteNumbering>
    requires(ByteNumbering == ByteNumbering::Lsb0 || ByteNumbering == ByteNumbering::Msb0)
constexpr auto
hex_string::from(bytes_view<ByteNumbering> const input) -> hex_string
{
    if constexpr (ByteNumbering == ByteNumbering::Msb0)
    {
        return hex_string{ bytes_le_t{ input } };
    }
    else if constexpr (ByteNumbering == ByteNumbering::Lsb0)
    {
        return hex_string{ input };
    }

    unreachable();
}

constexpr auto
hex_string::to_string(hex_string_format const fmt) const -> std::string
{
    std::span data_span{ binary_data_ };
    std::string r;
    r.reserve((data_span.size() + 1) * 2);
    std::array<char, 3> hex{ 0, 0, 0 };

    if (abc::lower_case(fmt))
    {
        r.append(hex_utility::prefix_0x);
        std::ranges::for_each(data_span | ranges::views::reverse | ranges::views::transform([&hex](auto const byte) {
                                  assert(hex[2] == 0);
                                  hex[0] = hex_utility::lower_case_hex_digits[byte >> 4], hex[1] = hex_utility::lower_case_hex_digits[byte & 0x0f];
                                  return hex.data();
                              }),
                              [&r](auto const * c_str) { r.append(c_str); });
    }
    else if (abc::upper_case(fmt))
    {
        r.append(hex_utility::prefix_0X);
        std::ranges::for_each(data_span | ranges::views::reverse | ranges::views::transform([&hex](auto const byte) mutable {
                                  assert(hex[2] == 0);
                                  hex[0] = hex_utility::upper_case_hex_digits[byte >> 4], hex[1] = hex_utility::upper_case_hex_digits[byte & 0x0f];
                                  return hex.data();
                              }),
                              [&r](auto const * c_str) { r.append(c_str); });
    }
    else
    {
        assert(false);
    }

    if (r == hex_utility::prefix_0x || r == hex_utility::prefix_0X)
    {
        r.append("00");
    }

    return r;
}

constexpr auto
hex_string::empty() const noexcept -> bool
{
    return binary_data_.empty();
}

constexpr auto
hex_string::bytes_size() const noexcept -> size_t
{
    return binary_data_.size();
}

constexpr auto
hex_string::size() const noexcept -> size_t
{
    return (binary_data_.size() << 1);
}

constexpr auto
hex_string::length() const noexcept -> size_t
{
    return size();
}

constexpr auto
hex_string::swap(hex_string & rhs) noexcept -> void
{
    binary_data_.swap(rhs.binary_data_);
}

template <ByteNumbering ByteNumberingV>
    requires(ByteNumberingV == ByteNumbering::Lsb0)
constexpr auto
hex_string::bytes() const -> abc::Bytes<ByteNumberingV> const &
{
    return binary_data_;
}

/// @brief get the byte buffer of the hex string in big endian format.
template <ByteNumbering ByteNumberingV>
    requires(ByteNumberingV == ByteNumbering::Msb0)
constexpr auto
hex_string::bytes() const -> abc::Bytes<ByteNumberingV>
{
    return abc::Bytes<ByteNumberingV>{ static_cast<bytes_le_view_t>(binary_data_) };
}

constexpr auto
hex_string::operator[](size_t const index) noexcept -> reference
{
    return reference{ this, index };
}

constexpr auto
hex_string::operator[](size_t const index) const noexcept -> const_reference
{
    return const_reference{ this, index };
}

constexpr auto
hex_string::least_significant_byte() const noexcept -> byte
{
    return binary_data_.front();
}

constexpr auto
hex_string::least_significant_byte() noexcept -> byte &
{
    return binary_data_.front();
}

constexpr auto
hex_string::most_significant_byte() const noexcept -> byte
{
    return binary_data_.back();
}

constexpr auto
hex_string::most_significant_byte() noexcept -> byte &
{
    return binary_data_.back();
}

} // namespace abc

#endif
