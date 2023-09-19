// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_CONVERTER)
#define ABC_CONVERTER

#pragma once

#include <abc/expected.h>
#include <abc/fixed_bytes.h>
#include <abc/hex_string.h>

#include <range/v3/algorithm/copy.hpp>

#include <system_error>

namespace abc {

template <typename OutputT, typename InputT>
struct converter {
    static auto from(InputT const & input) -> abc::expected<OutputT, std::error_code> {
        return static_cast<OutputT>(input);
    }
};

template <typename OutputT>
struct convert_to {
    template <typename InputT>
    static auto from(InputT const & input) -> abc::expected<OutputT, std::error_code> {
        return converter<OutputT, InputT>::from(input);
    }
};

template <size_t N, byte_numbering ByteNumbering>
struct converter<hex_string, fixed_bytes<N, ByteNumbering>> {
    static auto from(fixed_bytes<N, ByteNumbering> const & fixed_bytes) -> abc::expected<hex_string, std::error_code> {
        return hex_string::from<ByteNumbering>(fixed_bytes);
    }
};

template <size_t N, byte_numbering ByteNumbering>
struct converter<fixed_bytes<N, ByteNumbering>, hex_string> {
    constexpr static auto from(hex_string const & hex_str) -> abc::expected<fixed_bytes<N, ByteNumbering>, std::error_code> {
        if (hex_str.size() != N * 2) {
            return abc::make_unexpected(make_error_code(std::errc::invalid_argument));
        }

        std::array<byte, N> bytes{};
        ranges::copy(hex_str.bytes<ByteNumbering>(), std::begin(bytes));
        return fixed_bytes<N, ByteNumbering>::template from<ByteNumbering>(bytes);
    }
};

}

#endif //ABC_CONVERTER
