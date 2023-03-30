// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_CASE_INSENSITIVE_STRING)
#define ABC_DETAILS_CASE_INSENSITIVE_STRING

#pragma once

#include "char_traits_case_insensitive.h"

#include <string_view>
#include <string>

namespace abc::details {

template <typename CharT>
using xcase_insensitive_string_t = std::basic_string<CharT, xabc_case_insensitive<CharT>>;

template <typename CharT>
using xcase_insensitive_string_view_t = std::basic_string_view<CharT, xabc_case_insensitive<CharT>>;

}

#endif
