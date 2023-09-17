// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/hex_string.h>

namespace abc {

auto hex_string::from(std::string_view input) -> expected<hex_string, std::error_code> {
    return to_bytes<byte_numbering::lsb0>(input).transform([](auto && bytes) { return hex_string{ std::move(bytes) }; });
}

}