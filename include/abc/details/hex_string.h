// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_HEX_STRING)
#define ABC_DETAILS_HEX_STRING

#pragma once

#include "abc/bytes.h"
#include "abc/details/byte_bit_numbering.h"
#include "abc/details/error.h"
#include "abc/hex_string_format.h"
#include "abc/hex_utility.h"

#include <fmt/format.h>
#include <range/v3/range/conversion.hpp>
#include <range/v3/view/reverse.hpp>

#include <bit>
#include <cassert>
#include <compare>
#include <limits>
#include <ranges>
#include <span>

namespace abc::details {

class [[nodiscard]] xabc_hex_string {
    xbytes_t binary_data_;  // force little endian

public:
    xabc_hex_string() = default;
    xabc_hex_string(xabc_hex_string const &) = default;
    xabc_hex_string(xabc_hex_string &&) = default;
    auto operator=(xabc_hex_string const &) -> xabc_hex_string & = default;
    auto operator=(xabc_hex_string &&) -> xabc_hex_string & = default;
    ~xabc_hex_string() = default;

private:
    constexpr explicit xabc_hex_string(xbytes_t && input) noexcept : binary_data_{std::move(input)} {
    }

    constexpr explicit xabc_hex_string(std::span<xbyte_t const> const input, xabc_byte_numbering const bn) : binary_data_{
        std::begin(input), std::end(input)} {
        if (bn == xabc_byte_numbering::msb0) {
            std::ranges::reverse(binary_data_);
        }
    }

public:

    class const_reference {
    protected:
        friend class xabc_hex_string;

        xabc_hex_string const * str_{nullptr};
        size_t byte_index_{};
        bool high_{false};

        constexpr const_reference(xabc_hex_string const * str, size_t const nibble_index) noexcept
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

        constexpr operator xbyte_t() const noexcept {
            assert(str_ != nullptr);
            return (str_->binary_data_[byte_index_] >> (static_cast<size_t>(high_) * 4)) & 0x0f;
        }
    };

    class reference : public const_reference {
        friend class xabc_hex_string;

        constexpr reference(xabc_hex_string * str, size_t const nibble_index) noexcept
            : const_reference{str, nibble_index} {
        }

    public:
        constexpr reference() noexcept = delete;

        constexpr reference(reference const &) noexcept = default;
        constexpr reference(reference &&) noexcept = default;
        constexpr auto operator=(reference const &) noexcept -> reference & = default;
        constexpr auto operator=(reference &&) noexcept -> reference & = default;
        constexpr ~reference() noexcept = default;

        constexpr auto operator=(xbyte_t const value) noexcept -> reference & {
            assert(str_ != nullptr);
            assert(value <= 0x0f);

            if (high_) {
                const_cast<xabc_hex_string *>(str_)->binary_data_[byte_index_] = (str_->binary_data_[byte_index_] & 0x0f) | (value << 4);
            } else {
                const_cast<xabc_hex_string *>(str_)->binary_data_[byte_index_] = (str_->binary_data_[byte_index_] & 0xf0) | value;
            }
            return *this;
        }
    };

    using format = xabc_hex_string_format;
    constexpr inline static auto default_format = xabc_hex_string_format::default_format;
    constexpr inline static auto lower_case = xabc_hex_string_format::lower_case;
    constexpr inline static auto upper_case = xabc_hex_string_format::upper_case;

    constexpr static auto from_hex_prefixed(std::string_view const input) -> std::expected<xabc_hex_string, xerrc_t> {
        if (!input.starts_with(hex_prefix) && !input.starts_with(hex_prefix_uppercase)) {
            return std::unexpected{xerrc_t::invalid_hex_string };
        }

        return hex_string_to_binary<std::endian::big>(input).transform([](auto && bytes) { std::ranges::reverse(bytes); return xabc_hex_string{ std::move(bytes) }; });
    }

    constexpr static auto from_hex_without_prefix(std::string_view const input, xabc_byte_numbering const bn) -> std::expected<xabc_hex_string, xerrc_t> {
        if (bn == xabc_byte_numbering::none) {
            return std::unexpected{xerrc_t::invalid_byte_numbering };
        }

        if (bn == xabc_byte_numbering::msb0) {
            return hex_string_to_binary<std::endian::big>(input).transform([](auto && bytes) { return xabc_hex_string{ std::move(bytes) }; });
        } else {
            return hex_string_to_binary<std::endian::little>(input).transform([](auto && bytes) { return xabc_hex_string{ std::move(bytes) }; });
        }
    }

    constexpr static auto from_bytes(std::span<xbyte_t const> const input, xabc_byte_numbering const bn) -> xabc_hex_string {
        return xabc_hex_string{input, bn};
    }

    [[nodiscard]] auto operator==(xabc_hex_string const &) const noexcept -> bool = default;
    [[nodiscard]] auto operator<=>(xabc_hex_string const &) const noexcept -> std::strong_ordering = default;

    [[nodiscard]] constexpr auto to_string(xabc_hex_string_format const fmt = default_format) const -> std::string {
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

    constexpr void swap(xabc_hex_string & other) noexcept {
        binary_data_.swap(other.binary_data_);
    }

    template <xabc_byte_numbering ByteNumbering, std::enable_if_t<ByteNumbering == xabc_byte_numbering::lsb0> * = nullptr>
    constexpr auto to_bytes() const -> xbytes_t const & {
        return binary_data_;
    }

    template <xabc_byte_numbering ByteNumbering, std::enable_if_t<ByteNumbering == xabc_byte_numbering::msb0> * = nullptr>
    constexpr auto to_bytes() const -> xbytes_t {
        return binary_data_ | std::views::reverse | ranges::to<xbytes_t>();
    }

    constexpr auto operator[](size_t const index) noexcept -> reference {
        return reference{this, index};
    }

    constexpr auto operator[](size_t const index) const noexcept -> const_reference {
        return const_reference{this, index};
    }

    constexpr auto least_significant_byte() const noexcept -> xbyte_t const & {
        return binary_data_.front();
    }

    constexpr auto most_significant_byte() const noexcept -> xbyte_t const & {
        return binary_data_.back();
    }
};

}

#endif
