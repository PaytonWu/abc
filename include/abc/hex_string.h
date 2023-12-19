// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_STRING)
#define ABC_HEX_STRING

#pragma once

#include "byte_bit_numbering.h"
#include "error.h"
#include "expected.h"
#include "hex_string_format.h"
#include "hex_utility.h"
#include "bytes_view.h"

#include <fmt/format.h>
#include <range/v3/algorithm/reverse.hpp>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/reverse.hpp>
#include <range/v3/view/transform.hpp>

#include <bit>
#include <cassert>
#include <compare>
#include <limits>
#include <span>

namespace abc
{

/// @brief Represents a hex string, like "0x1234567890abcdef" or "1234567890abcdef".
///        hex_string stores the string value in a binary format. simply speaking,
///        it treats the hex string as a `number` and stores the number in a binary
///        buffer. In the above example, hex_string saves "0x1234567890abcdef" as
///        a hex number 0x1234567890abcdef and force to save it in
///        lsb0 (least significant byte at zero position) format. This means that,
///        in the binary byte layer, the beginning byte is `ef` and the ending
///        byte is `12`. If the hex string is "0xabc", then the beginning byte is
///        `bc` and the ending byte is `0a`.
class [[nodiscard]] hex_string
{
public:
    using format = hex_string_format;
    constexpr inline static auto default_format = hex_string_format::default_format;
    constexpr inline static auto lower_case = hex_string_format::lower_case;
    constexpr inline static auto upper_case = hex_string_format::upper_case;

private:
    bytes_le_t binary_data_;

    // constructors
public:
    hex_string() = default;

    constexpr explicit hex_string(bytes_le_t && input) noexcept : binary_data_{ std::move(input) }
    {
    }

    constexpr explicit hex_string(bytes_le_view_t const input) noexcept : binary_data_{ input }
    {
    }

public:
    class [[nodiscard]] const_reference
    {
    protected:
        friend class hex_string;

        hex_string const * str_{ nullptr };
        size_t byte_index_{};
        bool high_{ false };

        constexpr const_reference(hex_string const * str, size_t const nibble_index) noexcept
            : str_{ str }, byte_index_{ nibble_index / 2 }, high_{ static_cast<bool>(nibble_index % 2) }
        {
            assert(str != nullptr);
            assert(byte_index_ < str->size());
        }

    public:
        constexpr const_reference() noexcept = delete;

        constexpr operator char() const noexcept
        {
            assert(str_ != nullptr);
            return hex_utility::lower_case_hex_digits[(str_->binary_data_[byte_index_] >> (static_cast<size_t>(high_) * 4)) & 0x0f];
        }
    };

    class [[nodiscard]] reference
        : public const_reference
    {
        friend class hex_string;

        constexpr reference(hex_string * str, size_t const nibble_index) noexcept
            : const_reference{ str, nibble_index }
        {
        }

    public:
        constexpr reference() noexcept = delete;

        inline auto
        operator=(char const value) -> reference &
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
    };

    /// @brief construct hex_string object from a hex string. always treat the input hex string msb0.
    /// @param input string in various forms even it's not a hex form.
    /// @return hex_string object or an error value.
    static auto
    from(std::string_view input) -> expected<hex_string, std::error_code>;

    /// @brief construct hex_string object from a byte buffer view.
    /// @tparam ByteNumbering specify the byte numbering of the input bytes.
    /// @param input input byte buffer.
    /// @return the constructed hex_string object.
    template <byte_numbering ByteNumbering>
    constexpr static auto
    from(bytes_view<ByteNumbering> const input) -> hex_string
    {
        if constexpr (ByteNumbering == byte_numbering::msb0)
        {
            return hex_string{ bytes_le_t{ input }};
        }
        else if constexpr (ByteNumbering == byte_numbering::lsb0)
        {
            return hex_string{ input };
        }

        unreachable();
    }

    [[nodiscard]] auto
    operator==(hex_string const &) const noexcept -> bool = default;

    [[nodiscard]] auto
    operator<=>(hex_string const &) const noexcept -> std::strong_ordering = default;

    /// @brief convert the hex string to a standard string with prefix 0x or 0X.
    /// @param fmt the output format.
    /// @return a standard string with proper prefix.
    [[nodiscard]] constexpr auto
    to_string(hex_string_format const fmt = default_format) const -> std::string
    {
        std::span data_span{ binary_data_ };
        std::string r;
        r.reserve((data_span.size() + 1) * 2);
        std::array<char, 3> hex{ 0, 0, 0 };

        if (abc::lower_case(fmt))
        {
            r.append(hex_utility::prefix);
            std::ranges::for_each(data_span | ranges::views::reverse | ranges::views::transform([&hex](auto const byte) mutable {
                assert(hex[2] == 0);
                hex[0] = hex_utility::lower_case_hex_digits[byte >> 4], hex[1] = hex_utility::lower_case_hex_digits[byte & 0x0f];
                return hex.data();
            }), [&r](auto const * c_str) { r.append(c_str); });
        }
        else if (abc::upper_case(fmt))
        {
            r.append(hex_utility::prefix_uppercase);
            std::ranges::for_each(data_span | ranges::views::reverse | ranges::views::transform([&hex](auto const byte) mutable {
                assert(hex[2] == 0);
                hex[0] = hex_utility::upper_case_hex_digits[byte >> 4], hex[1] = hex_utility::upper_case_hex_digits[byte & 0x0f];
                return hex.data();
            }), [&r](auto const * c_str) { r.append(c_str); });
        }
        else
        {
            assert(false);
        }

        if (r == hex_utility::prefix || r == hex_utility::prefix_uppercase)
        {
            r.append("00");
        }

        return r;
    }

    /// @brief check if the hex string is empty.
    /// @return true if the hex string is empty, otherwise false.
    [[nodiscard]] constexpr auto
    empty() const noexcept -> bool
    {
        return binary_data_.empty();
    }

    /// @brief get the size of the hex string in bytes.
    /// @return the byte size of the hex string.
    [[nodiscard]] constexpr auto
    bytes_size() const noexcept -> size_t
    {
        return binary_data_.size();
    }

    /// @brief get the size of the hex string in nibbles.
    /// @return the nibble size of the hex string.
    [[nodiscard]] constexpr auto
    size() const noexcept -> size_t
    {
        return (binary_data_.size() << 1);
    }

    /// @brief get the length of the hex string in nibbles.
    [[nodiscard]] constexpr auto
    length() const noexcept -> size_t
    {
        return size();
    }

    /// @brief swap the content of two hex string objects.
    /// @param rhs the other hex string object.
    constexpr auto
    swap(hex_string & rhs) noexcept
    {
        binary_data_.swap(rhs.binary_data_);
    }

    /// @brief get the byte buffer of the hex string in little endian format.
    template <byte_numbering ByteNumbering>
    requires(ByteNumbering == byte_numbering::lsb0)
    [[nodiscard]] constexpr auto
    bytes() const -> abc::bytes<ByteNumbering> const &
    {
        return binary_data_;
    }

    /// @brief get the byte buffer of the hex string in big endian format.
    template <byte_numbering ByteNumbering>
    requires(ByteNumbering == byte_numbering::msb0)
    constexpr auto
    bytes() const -> abc::bytes<ByteNumbering>
    {
        return abc::bytes<ByteNumbering>{ binary_data_ };
    }

    /// @brief get the modifiable nibble at the specified index.
    /// @param index the nibble index.
    /// @return reference to the nibble.
    constexpr auto
    operator[](size_t const index) noexcept -> reference
    {
        return reference{ this, index };
    }

    /// @brief get the non-modifiable nibble at the specified index.
    /// @param index the nibble index.
    /// @return const reference to the nibble.
    constexpr auto
    operator[](size_t const index) const noexcept -> const_reference
    {
        return const_reference{ this, index };
    }

    /// @brief get the least significant byte.
    /// @return the byte value of the least significant byte.
    [[nodiscard]] constexpr auto
    least_significant_byte() const noexcept -> byte
    {
        return binary_data_.front();
    }

    /// @brief get the least significant byte.
    /// @return the byte reference of the least significant byte.
    [[nodiscard]] constexpr auto
    least_significant_byte() noexcept -> byte &
    {
        return binary_data_.front();
    }

    /// @brief get the most significant byte.
    /// @return the byte value of the most significant byte.
    [[nodiscard]] constexpr auto
    most_significant_byte() const noexcept -> byte
    {
        return binary_data_.back();
    }

    /// @brief get the most significant byte.
    /// @return the byte reference of the most significant byte.
    [[nodiscard]] constexpr auto
    most_significant_byte() noexcept -> byte &
    {
        return binary_data_.back();
    }
};

}

#endif
