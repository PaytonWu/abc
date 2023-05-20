// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_CONVERTER
#define ABC_CONVERTER

#pragma once

#include "abc/hex_string.h"
#include "abc/fixed_bytes.h"

#include <expected>
#include <system_error>

namespace abc {

template <typename OutputT, typename InputT>
struct converter {
    constexpr static auto from(InputT const & input) -> std::expected<OutputT, std::error_code> {
        return static_cast<OutputT>(input);
    }
};

template <typename OutputT>
struct convert_to {
    template <typename InputT>
    constexpr static auto from(InputT const & input) -> std::expected<OutputT, std::error_code> {
        return converter<OutputT, InputT>::from(input);
    }
};

template <size_t N, byte_numbering ByteNumbering>
struct converter<hex_string, fixed_bytes<N, ByteNumbering>> {
    static auto from(fixed_bytes<N, ByteNumbering> const & fixed_bytes) -> std::expected<hex_string, std::error_code> {
        return std::expected<hex_string, std::error_code>{ hex_string::from_bytes(fixed_bytes, ByteNumbering) };
    }
};

template <size_t N, byte_numbering ByteNumbering>
struct converter<fixed_bytes<N, ByteNumbering>, hex_string> {
    constexpr static auto from(hex_string const & hex_str) -> std::expected<fixed_bytes<N, ByteNumbering>, std::error_code> {
        if (hex_str.size() != N * 2) {
            return std::unexpected<std::error_code>{ make_error_code(std::errc::invalid_argument) };
        }

        std::array<byte, N> bytes{};
        hex_str.to_bytes<ByteNumbering>() | std::ranges::copy(bytes.begin());
        return std::expected<fixed_bytes<N, ByteNumbering>, std::error_code>{ fixed_bytes<N, ByteNumbering>{bytes} };
    }
};

}

#endif //ABC_CONVERTER
