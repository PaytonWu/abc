// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_CASE_INSENSITIVE_STRING)
#define ABC_CASE_INSENSITIVE_STRING

#pragma once

#include "details/case_insensitive_string.h"

namespace abc {

template <typename CharT>
using xcase_insensitive_string_t = details::xcase_insensitive_string_t<CharT>;

template <typename CharT>
using xcase_insensitive_string_view_t = details::xcase_insensitive_string_view_t<CharT>;

}

#endif
