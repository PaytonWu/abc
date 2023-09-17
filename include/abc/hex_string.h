// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_HEX_STRING)
#define ABC_HEX_STRING

#pragma once

#include <abc/byte_bit_numbering.h>
#include <abc/bytes.h>
#include <abc/error.h>
#include <abc/expected.h>
#include <abc/hex_string_format.h>
#include <abc/hex_utility.h>

#include <fmt/format.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/algorithm/reverse.hpp>

#include <bit>
#include <cassert>
#include <compare>
#include <limits>
#include <span>

namespace abc {

/// @brief Represents a hex string, like "0x1234567890abcdef" or "1234567890abcdef".
///        hex_string stores the string value in a binary format. simply speaking,
///        it treats the hex string as a `number` and stores the number in a binary
///        buffer. In the above example, hex_string saves "0x1234567890abcdef" as
///        a hex number 0x1234567890abcdef and force to save it in
///        lsb0 (least significant byte at zero position) format. This means that,
///        in the binary byte layer, the beginning byte is `ef` and the ending
///        byte is `12`. If the hex string is "0xabc", then the beginning byte is
///        `bc` and the ending byte is `0a`.
class [[nodiscard]] hex_string {
    bytes binary_data_;

public:
    hex_string() = default;

private:
    constexpr explicit hex_string(bytes && input) noexcept : binary_data_{std::move(input)} {
    }

    constexpr explicit hex_string(std::span<byte const> const input, byte_numbering const bn) : binary_data_{
        std::begin(input), std::end(input)} {
        if (bn == byte_numbering::msb0) {
            ranges::reverse(binary_data_);
        }
    }

public:
    constexpr inline static auto prefix = "0x";
    constexpr inline static auto prefix_uppercase = "0X";

public:
    // utilities
    [[nodiscard]] constexpr static auto is_hex(std::string_view string_slice) noexcept -> bool {
        if (has_hex_prefix(string_slice)) {
            string_slice = string_slice.substr(2);
        }

        return is_hex_without_prefix(string_slice);
    }

    [[nodiscard]] constexpr static auto has_hex_prefix(std::string_view const string_slice) noexcept -> bool {
        return string_slice.starts_with(prefix) || string_slice.starts_with(prefix_uppercase);
    }

    template <byte_numbering ByteNumbering>
    static auto to_bytes(std::string_view string_slice) -> expected<bytes, std::error_code> {
        if (has_hex_prefix(string_slice)) {
            string_slice = string_slice.substr(2);
        }

        if (!is_hex_without_prefix(string_slice)) {
            return make_unexpected(make_error_code(std::errc::invalid_argument));
        }

        bytes binary_data;
        binary_data.reserve((string_slice.size() + 1) / 2);
        if constexpr (ByteNumbering == byte_numbering::msb0) {
            if (string_slice.size() & 1) {
                [[maybe_unused]] auto r = hex_utility::hex_char_to_binary(string_slice.front()).transform([&binary_data, &string_slice](auto const b) mutable { binary_data.push_back(b); string_slice.remove_prefix(1); });
                assert(r.has_value());
            }

            auto const & chunks = string_slice | ranges::views::chunk(2);
            ranges::for_each(chunks, [&binary_data](ranges::viewable_range auto && compound_byte) mutable {
                byte byte{};
                for (auto const [i, nibble_byte] : compound_byte | ranges::views::reverse | ranges::views::enumerate) {
                    byte |= hex_char_to_binary(nibble_byte).value() << (4 * i);
                }
                binary_data.push_back(byte);
            });

            return binary_data;
        }

        if constexpr (ByteNumbering == byte_numbering::lsb0) {
            if (string_slice.size() & 1) {
                [[maybe_unused]] auto r = hex_utility::hex_char_to_binary(string_slice.front()).transform([&binary_data, &string_slice](auto const b) mutable { binary_data.push_back(b); string_slice.remove_prefix(1); });
                assert(r.has_value());
            }

            auto const & chunks = string_slice | ranges::views::chunk(2);
            ranges::for_each(chunks, [&binary_data](ranges::viewable_range auto && compound_byte) mutable {
                byte byte{};
                for (auto const [i, nibble_byte] : compound_byte | ranges::views::reverse | ranges::views::enumerate) {
                    byte |= hex_utility::hex_char_to_binary(nibble_byte).value() << (4 * i);
                }
                binary_data.push_back(byte);
            });

            ranges::reverse(binary_data);

            return binary_data;
        }
    }

public:
    class const_reference {
    protected:
        friend class hex_string;

        hex_string const * str_{nullptr};
        size_t byte_index_{};
        bool high_{false};

        constexpr const_reference(hex_string const * str, size_t const nibble_index) noexcept
            : str_{str}, byte_index_{nibble_index / 2}, high_{static_cast<bool>(nibble_index % 2)} {
            assert(str != nullptr);
            assert(byte_index_ < str->size());
        }

    public:
        constexpr const_reference() noexcept = delete;

        constexpr const_reference(const_reference const &) noexcept = default;
        constexpr const_reference(const_reference &&) noexcept = default;
        constexpr auto operator=(const_reference const &) noexcept -> const_reference & = default;
        constexpr auto operator=(const_reference &&) noexcept -> const_reference & = default;
        constexpr ~const_reference() noexcept = default;

        constexpr operator char() const noexcept {
            assert(str_ != nullptr);
            return hex_utility::lower_case_hex_digits[(str_->binary_data_[byte_index_] >> (static_cast<size_t>(high_) * 4)) & 0x0f];
        }
    };

    class reference : public const_reference {
        friend class hex_string;

        constexpr reference(hex_string * str, size_t const nibble_index) noexcept
            : const_reference{str, nibble_index} {
        }

    public:
        constexpr reference() noexcept = delete;

        constexpr reference(reference const &) noexcept = default;
        constexpr reference(reference &&) noexcept = default;
        constexpr auto operator=(reference const &) noexcept -> reference & = default;
        constexpr auto operator=(reference &&) noexcept -> reference & = default;
        constexpr ~reference() noexcept = default;

        constexpr auto operator=(char const value) -> reference & {
            assert(str_ != nullptr);

            if (!std::isxdigit(static_cast<unsigned char>(value))) {
                throw_error(make_error_code(errc::invalid_hex_char));
                return *this;
            }

            assert((value >= '0' && value <= '9') || (value >= 'a' && value <= 'f') || (value >= 'A' && value <= 'F'));
            hex_utility::hex_char_to_binary(value).transform([this](auto byte) {
                if (high_) {
                    const_cast<hex_string *>(str_)->binary_data_[byte_index_] = (str_->binary_data_[byte_index_] & 0x0f) | (byte << 4);
                } else {
                    const_cast<hex_string *>(str_)->binary_data_[byte_index_] = (str_->binary_data_[byte_index_] & 0xf0) | byte;
                }
            });

            return *this;
        }
    };

    using format = hex_string_format;
    constexpr inline static auto default_format = hex_string_format::default_format;
    constexpr inline static auto lower_case = hex_string_format::lower_case;
    constexpr inline static auto upper_case = hex_string_format::upper_case;

    /// @brief construct hex_string object from a hex string.
    /// @param input string in various forms even it's not a hex form.
    /// @return hex_string object or an error value.
    static auto from(std::string_view input) -> expected<hex_string, std::error_code>;

    constexpr static auto from_bytes(std::span<byte const> const input, byte_numbering const bn) -> hex_string {
        return hex_string{input, bn};
    }

    [[nodiscard]] auto operator==(hex_string const &) const noexcept -> bool = default;
    [[nodiscard]] auto operator<=>(hex_string const &) const noexcept -> std::strong_ordering = default;

    [[nodiscard]] constexpr auto to_string(hex_string_format const fmt = default_format) const -> std::string {
        std::span data_span{binary_data_};
        std::string r;
        r.reserve((data_span.size() + 1) * 2);
        std::array<char, 3> hex{0, 0, 0};

        if (abc::lower_case(fmt)) {
            r.append(prefix);
            std::ranges::for_each(data_span | std::views::reverse | std::views::transform([&hex](auto const byte) mutable { assert(hex[2] == 0); hex[0] = hex_utility::lower_case_hex_digits[byte >> 4], hex[1] = hex_utility::lower_case_hex_digits[byte & 0x0f]; return hex.data(); }), [&r](auto const * c_str) { r.append(c_str); });
        } else if (abc::upper_case(fmt)) {
            r.append(prefix_uppercase);
            std::ranges::for_each(data_span | std::views::reverse | std::views::transform([&hex](auto const byte) mutable { assert(hex[2] == 0); hex[0] = hex_utility::upper_case_hex_digits[byte >> 4], hex[1] = hex_utility::upper_case_hex_digits[byte & 0x0f]; return hex.data(); }), [&r](auto const * c_str) { r.append(c_str); });
        } else {
            assert(false);
        }

        if (r == prefix || r == prefix_uppercase) {
            r.append("00");
        }

        return r;
    }

    [[nodiscard]] constexpr auto empty() const noexcept -> bool {
        return binary_data_.empty();
    }

    [[nodiscard]] constexpr auto bytes_size() const noexcept -> size_t {
        return binary_data_.size();
    }

    [[nodiscard]] constexpr auto size() const noexcept -> size_t {
        return (binary_data_.size() << 1);
    }

    [[nodiscard]] constexpr auto length() const noexcept -> size_t {
        return size();
    }

    constexpr void swap(hex_string & other) noexcept {
        binary_data_.swap(other.binary_data_);
    }

    template <byte_numbering ByteNumbering, std::enable_if_t<ByteNumbering == byte_numbering::lsb0> * = nullptr>
    [[nodiscard]] constexpr auto to_bytes() const -> bytes const & {
        return binary_data_;
    }

    template <byte_numbering ByteNumbering, std::enable_if_t<ByteNumbering == byte_numbering::msb0> * = nullptr>
    constexpr auto to_bytes() const -> bytes {
        return binary_data_ | ranges::views::reverse | ranges::to<bytes>();
    }

    constexpr auto operator[](size_t const index) noexcept -> reference {
        return reference{this, index};
    }

    constexpr auto operator[](size_t const index) const noexcept -> const_reference {
        return const_reference{this, index};
    }

    [[nodiscard]] constexpr auto least_significant_byte() const noexcept -> byte {
        return binary_data_.front();
    }

    [[nodiscard]] constexpr auto most_significant_byte() const noexcept -> byte {
        return binary_data_.back();
    }

private:
    [[nodiscard]] constexpr static auto is_hex_without_prefix(std::string_view const string_slice) noexcept -> bool {   // "0123456789abcdefABCDEF"
        return std::ranges::all_of(string_slice,
                                   [](auto const ch) {
                                       return hex_utility::is_hex(ch);
                                   });
    }

    [[nodiscard]] constexpr static auto is_hex_with_prefix(std::string_view const string_slice) noexcept -> bool { // "0x0123456789abcdefABCDEF" or "0X0123456789abcdefABCDEF"
        if (!has_hex_prefix(string_slice)) {
            return false;
        }

        return is_hex_without_prefix(string_slice.substr(2));
    }
};

}

#endif
