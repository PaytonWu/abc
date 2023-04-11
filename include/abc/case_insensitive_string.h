// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_CASE_INSENSITIVE_STRING)
#define ABC_CASE_INSENSITIVE_STRING

#pragma once

#include "abc/details/char_traits_case_insensitive.h"

#include <string_view>
#include <string>

namespace abc {

template <typename CharT>
using xcase_insensitive_string_t = std::basic_string<CharT, details::xabc_case_insensitive<CharT>>;

template <typename CharT>
using xcase_insensitive_string_view_t = std::basic_string_view<CharT, details::xabc_case_insensitive<CharT>>;

}

#endif
