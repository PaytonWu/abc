// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_HEX_STRING_FORMAT)
#define ABC_DETAILS_HEX_STRING_FORMAT

#pragma once

namespace abc::details {

enum class [[nodiscard]] xabc_hex_string_format : uint8_t {
    default_format = 0b00000000u,
    lower_case = 0b00000000u,
    upper_case = 0b00000010u,
};

}

#endif
