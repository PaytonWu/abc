// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_HEX_STRING_FORMAT)
#define ABC_DETAILS_HEX_STRING_FORMAT

#pragma once

namespace abc::details {

enum class [[nodiscard]] xabc_hex_string_format : uint8_t {
    lower_case_without_prefix = 0b00000000u,
    upper_case_without_prefix = 0b00010000u,
    mixed_case_without_prefix = 0b00100000u,

    lower_case_with_prefix = 0b00000001u,
    upper_case_with_prefix = 0b00010001u,
    mixed_case_with_prefix = 0b00100001u,
};

}

#endif
