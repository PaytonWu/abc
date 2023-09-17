// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/hex_utility.h>

namespace abc::hex_utility {

auto hex_char_to_binary(char const ch) noexcept -> abc::expected<byte, std::error_code> {
    if ('0' <= ch && ch <= '9') {
        return static_cast<byte>(ch - '0');
    }

    if ('a' <= ch && ch <= 'f') {
        return static_cast<byte>(ch - 'a' + 10);
    }

    if ('A' <= ch && ch <= 'F') {
        return static_cast<byte>(ch - 'A' + 10);
    }

    return make_unexpected(make_error_code(std::errc::invalid_argument));
}

}