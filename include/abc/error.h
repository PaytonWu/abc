// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_ERROR_ERROR)
#define ABC_ERROR_ERROR

#pragma once

#include "details/error.h"

#include <system_error>

namespace abc::error {

using xerror_t = details::xabc_error;
using xerrc_t = details::xabc_enum_errc;

auto abc_category() noexcept -> std::error_category const &;

void throw_error(std::error_code const & ec);
void throw_error(std::error_code const & ec, std::string_view extra_msg);

}

namespace std {

template <>
struct is_error_code_enum<abc::error::xerrc_t> : std::true_type {
};

template <>
struct is_error_condition_enum<abc::error::xerrc_t> : std::true_type {
};

}

#endif
