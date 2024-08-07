// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_INCLUDE_ABC_HEX_STRING_DECL)
#define ABC_INCLUDE_ABC_HEX_STRING_DECL

#pragma once

#include "hex_string_fwd_decl.h"

#include "bytes_decl.h"
#include "bytes_view_decl.h"
#include "expected.h"
#include "hex_string_format.h"

#include <compare>

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
class hex_string
{
public:
    using format = hex_string_format;
    constexpr static auto default_format = hex_string_format::default_format;
    constexpr static auto lower_case = hex_string_format::lower_case;
    constexpr static auto upper_case = hex_string_format::upper_case;

private:
    bytes_le_t binary_data_;

public:
    hex_string() = default;

    constexpr explicit hex_string(bytes_le_t && input) noexcept;

    constexpr explicit hex_string(bytes_le_view_t input) noexcept;

public:
    class [[nodiscard]] const_reference
    {
    protected:
        friend class hex_string;

        hex_string const * str_{ nullptr };
        size_t byte_index_{};
        bool high_{ false };

        constexpr const_reference(hex_string const * str, size_t nibble_index) noexcept;

    public:
        constexpr const_reference() noexcept = delete;

        constexpr operator char() const noexcept;
    };

    class [[nodiscard]] reference
        : public const_reference
    {
        friend class hex_string;

        constexpr reference(hex_string * str, size_t nibble_index) noexcept;

    public:
        constexpr reference() noexcept = delete;

        auto
        operator=(char value) -> reference &;
    };

    /// @brief construct hex_string object from a hex string. always treat the input hex string msb0, i.e., "0x1234567890abcdefABCDEF" or "1234567890abcdefABCDEF".
    /// @param input string in various forms even it's not a hex form.
    /// @return hex_string object or an error value.
    static auto
    from(std::string_view input) -> expected<hex_string, std::error_code>;

    /// @brief construct hex_string object from a byte buffer view.
    /// @tparam ByteNumbering specify the byte numbering of the input bytes.
    /// @param input input byte buffer.
    /// @return the constructed hex_string object.
    template <byte_numbering ByteNumbering> requires(ByteNumbering == byte_numbering::lsb0 || ByteNumbering == byte_numbering::msb0)
    constexpr static auto
    from(bytes_view<ByteNumbering> input) -> hex_string;

    [[nodiscard]] auto
    operator==(hex_string const &) const noexcept -> bool = default;

    [[nodiscard]] auto
    operator<=>(hex_string const &) const noexcept -> std::strong_ordering = default;

    /// @brief convert the hex string to a standard string with prefix 0x or 0X.
    /// @param fmt the output format.
    /// @return a standard string with proper prefix.
    [[nodiscard]] constexpr auto
    to_string(hex_string_format fmt = default_format) const -> std::string;

    /// @brief check if the hex string is empty.
    /// @return true if the hex string is empty, otherwise false.
    [[nodiscard]] constexpr auto
    empty() const noexcept -> bool;

    /// @brief get the size of the hex string in bytes.
    /// @return the byte size of the hex string.
    [[nodiscard]] constexpr auto
    bytes_size() const noexcept -> size_t;

    /// @brief get the size of the hex string in nibbles.
    /// @return the nibble size of the hex string.
    [[nodiscard]] constexpr auto
    size() const noexcept -> size_t;

    /// @brief get the length of the hex string in nibbles.
    [[nodiscard]] constexpr auto
    length() const noexcept -> size_t;

    /// @brief swap the content of two hex string objects.
    /// @param rhs the other hex string object.
    constexpr auto
    swap(hex_string & rhs) noexcept -> void;

    /// @brief get the byte buffer of the hex string in little endian format.
    template <byte_numbering ByteNumbering>
    requires(ByteNumbering == byte_numbering::lsb0)
    [[nodiscard]] constexpr auto
    bytes() const -> abc::bytes<ByteNumbering> const &;

    /// @brief get the byte buffer of the hex string in big endian format.
    template <byte_numbering ByteNumbering>
    requires(ByteNumbering == byte_numbering::msb0)
    constexpr auto
    bytes() const -> abc::bytes<ByteNumbering>;

    /// @brief get the modifiable nibble at the specified index.
    /// @param index the nibble index.
    /// @return reference to the nibble.
    constexpr auto
    operator[](size_t index) noexcept -> reference;

    /// @brief get the non-modifiable nibble at the specified index.
    /// @param index the nibble index.
    /// @return const reference to the nibble.
    constexpr auto
    operator[](size_t index) const noexcept -> const_reference;

    /// @brief get the least significant byte.
    /// @return the byte value of the least significant byte.
    [[nodiscard]] constexpr auto
    least_significant_byte() const noexcept -> byte;

    /// @brief get the least significant byte.
    /// @return the byte reference of the least significant byte.
    [[nodiscard]] constexpr auto
    least_significant_byte() noexcept -> byte &;

    /// @brief get the most significant byte.
    /// @return the byte value of the most significant byte.
    [[nodiscard]] constexpr auto
    most_significant_byte() const noexcept -> byte;

    /// @brief get the most significant byte.
    /// @return the byte reference of the most significant byte.
    [[nodiscard]] constexpr auto
    most_significant_byte() noexcept -> byte &;
};

}

#endif // ABC_INCLUDE_ABC_HEX_STRING_DECL
