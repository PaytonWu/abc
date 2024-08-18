// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_DETAILS_ERROR
#define ABC_INCLUDE_ABC_DETAILS_ERROR

#pragma once

#include "abc/details/config.h"

#include <system_error>

#ifdef ABC_CXX17
namespace abc::details {
#else
namespace abc { namespace details {
#endif

[[noreturn]] auto
do_throw_error(std::error_code const & ec) -> void;

[[noreturn]] auto
do_throw_error(std::error_code const & ec, std::string_view const extra_msg) -> void;

#ifdef ABC_CXX17
} // namespace abc::details
#else
} } // namespace abc::details
#endif

#endif // ABC_INCLUDE_ABC_DETAILS_ERROR
