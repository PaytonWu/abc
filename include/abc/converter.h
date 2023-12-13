// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_CONVERTER)
#define ABC_CONVERTER

#pragma once

#include "expected.h"
#include "fixed_bytes.h"
#include "hex_string.h"
#include "uint128.h"

#include <range/v3/algorithm/copy.hpp>

#include <concepts>
#include <system_error>

namespace abc {

template <typename OutputT, typename InputT>
struct converter {
    inline static auto from(InputT const & input) -> abc::expected<OutputT, std::error_code> {
        return static_cast<OutputT>(input);
    }
};

template <typename OutputT>
struct convert_to {
    template <typename InputT>
    inline static auto from(InputT const & input) -> abc::expected<OutputT, std::error_code> {
        return converter<OutputT, InputT>::from(input);
    }
};

template <size_t N, byte_numbering ByteNumbering>
struct converter<hex_string, fixed_bytes<N, ByteNumbering>> {
    inline static auto from(fixed_bytes<N, ByteNumbering> const & fixed_bytes) -> abc::expected<hex_string, std::error_code> {
        return hex_string::from<ByteNumbering>(fixed_bytes);
    }
};

template <size_t N, byte_numbering ByteNumbering>
struct converter<fixed_bytes<N, ByteNumbering>, hex_string> {
    inline static auto from(hex_string const & hex_str) -> abc::expected<fixed_bytes<N, ByteNumbering>, std::error_code> {
        if (hex_str.size() != N * 2) {
            return abc::make_unexpected(make_error_code(std::errc::invalid_argument));
        }

        std::array<byte, N> bytes{};
        ranges::copy(hex_str.bytes<ByteNumbering>(), std::begin(bytes));
        return fixed_bytes<N, ByteNumbering>::template from<ByteNumbering>(bytes);
    }
};

template <byte_numbering ByteNumbering, std::integral T>
struct converter<bytes<ByteNumbering>, T> {
    inline static auto from(T const & num) -> abc::expected<bytes<ByteNumbering>, std::error_code> {
        return bytes<ByteNumbering>::from(num);
    }
};

template <std::integral T, byte_numbering ByteNumbering>
struct converter<T, bytes<ByteNumbering>> {
    inline static auto from(bytes<ByteNumbering> const & number_bytes) -> abc::expected<T, std::error_code> {
        return number_bytes.template to<T>();
    }
};

template <std::integral T, byte_numbering ByteNumbering>
struct converter<T, bytes_view<ByteNumbering>> {
    inline static auto from(bytes_view<ByteNumbering> const & number_bytes) -> abc::expected<T, std::error_code> {
        T r{};
        if constexpr (ByteNumbering == byte_numbering::lsb0 || (ByteNumbering == byte_numbering::none && std::endian::native == std::endian::little)) {
            for (auto i = 0zu; i < number_bytes.size(); ++i) {
                r += (static_cast<T>(number_bytes[i]) << (i * 8));
            }
        } else if constexpr (ByteNumbering == byte_numbering::msb0 || (ByteNumbering == byte_numbering::none && std::endian::native == std::endian::big)) {
            for (auto i = 0zu; i < number_bytes.size(); ++i) {
                r = r << 8 | number_bytes[i];
            }
        }

        return r;
    }
};

template <byte_numbering ByteNumbering>
struct converter<uint128_t, bytes_view<ByteNumbering>> {
    inline static auto from(bytes_view<ByteNumbering> const & number_bytes) -> abc::expected<uint128_t, std::error_code> {
        uint128_t r{};
        if constexpr (ByteNumbering == byte_numbering::lsb0 || (ByteNumbering == byte_numbering::none && std::endian::native == std::endian::little)) {
            for (auto i = 0zu; i < number_bytes.size(); ++i) {
                r += (static_cast<uint8_t>(number_bytes[i]) << (i * 8));
            }
        } else if constexpr (ByteNumbering == byte_numbering::msb0 || (ByteNumbering == byte_numbering::none && std::endian::native == std::endian::big)) {
            for (auto i = 0zu; i < number_bytes.size(); ++i) {
                r = r << 8 | number_bytes[i];
            }
        }

        return r;
    }
};

}

#endif //ABC_CONVERTER
