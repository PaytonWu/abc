// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_ERROR)
#define ABC_DETAILS_ERROR

#pragma once

#include "abc/details/config.h"

#include <string_view>
#include <system_error>

namespace abc::details {

void do_throw_error(std::error_code const & ec);
void do_throw_error(std::error_code const & ec, std::string_view extra_msg);

} // namespace abc::details

#endif
