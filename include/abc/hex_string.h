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

class [[nodiscard]] hex_string {
    bytes binary_data_;  // force little endian

public:
    hex_string() = default;
    hex_string(hex_string const &) = default;
    hex_string(hex_string &&) = default;
    auto operator=(hex_string const &) -> hex_string & = default;
    auto operator=(hex_string &&) -> hex_string & = default;
    ~hex_string() = default;

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
            return lower_case_hex_digits[(str_->binary_data_[byte_index_] >> (static_cast<size_t>(high_) * 4)) & 0x0f];
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
            hex_char_to_binary(value).transform([this](auto && byte) {
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

    constexpr static auto from_hex_prefixed(std::string_view input) -> std::expected<hex_string, errc> {
        if (!input.starts_with(hex_prefix) && !input.starts_with(hex_prefix_uppercase)) {
            return std::unexpected{errc::invalid_hex_string };
        }

        return hex_string_to_binary<std::endian::big>(input).transform([](auto && bytes) { ranges::reverse(bytes); return hex_string{ std::move(bytes) }; });
    }

    constexpr static auto from_hex_without_prefix(std::string_view const input, byte_numbering const bn) -> std::expected<hex_string, errc> {
        if (bn == byte_numbering::none) {
            return std::unexpected{errc::invalid_byte_numbering };
        }

        if (bn == byte_numbering::msb0) {
            return hex_string_to_binary<std::endian::big>(input).transform([](auto && bytes) { return hex_string{ std::move(bytes) }; });
        } else {
            return hex_string_to_binary<std::endian::little>(input).transform([](auto && bytes) { return hex_string{ std::move(bytes) }; });
        }
    }

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
            r.append(hex_prefix);
            std::ranges::for_each(data_span | std::views::reverse | std::views::transform([&hex](auto const byte) mutable { assert(hex[2] == 0); hex[0] = lower_case_hex_digits[byte >> 4], hex[1] = lower_case_hex_digits[byte & 0x0f]; return hex.data(); }), [&r](auto const * c_str) { r.append(c_str); });
        } else if (abc::upper_case(fmt)) {
            r.append(hex_prefix_uppercase);
            std::ranges::for_each(data_span | std::views::reverse | std::views::transform([&hex](auto const byte) mutable { assert(hex[2] == 0); hex[0] = upper_case_hex_digits[byte >> 4], hex[1] = upper_case_hex_digits[byte & 0x0f]; return hex.data(); }), [&r](auto const * c_str) { r.append(c_str); });
        } else {
            assert(false);
        }

        if (r == hex_prefix || r == hex_prefix_uppercase) {
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
    constexpr auto to_bytes() const -> bytes const & {
        return binary_data_;
    }

    template <byte_numbering ByteNumbering, std::enable_if_t<ByteNumbering == byte_numbering::msb0> * = nullptr>
    constexpr auto to_bytes() const -> bytes {
        return binary_data_ | std::views::reverse | ranges::to<bytes>();
    }

    constexpr auto operator[](size_t const index) noexcept -> reference {
        return reference{this, index};
    }

    constexpr auto operator[](size_t const index) const noexcept -> const_reference {
        return const_reference{this, index};
    }

    constexpr auto least_significant_byte() const noexcept -> byte const & {
        return binary_data_.front();
    }

    constexpr auto most_significant_byte() const noexcept -> byte const & {
        return binary_data_.back();
    }
};

}

#endif
