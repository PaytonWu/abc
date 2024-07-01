// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_STRING)
#define ABC_HEX_STRING

#pragma once

#include "hex_string_decl.h"

namespace abc
{

constexpr hex_string::const_reference::
const_reference(hex_string const * str, size_t const nibble_index) noexcept
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

constexpr hex_string::reference::reference(hex_string * str, size_t const nibble_index) noexcept
    : const_reference{ str, nibble_index }
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

constexpr hex_string::
hex_string(bytes_le_t && input) noexcept
    : binary_data_{ std::move(input) }
{
}

constexpr hex_string::
hex_string(bytes_le_view_t const input) noexcept
    : binary_data_{ input }
{
}

} // namespace abc

#endif
