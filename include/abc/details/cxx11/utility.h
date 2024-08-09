// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_DETAILS_CXX11_UTILITY
#define ABC_INCLUDE_ABC_DETAILS_CXX11_UTILITY

#pragma once

#include "abc/details/config.h"

#ifdef ABC_CXX17
namespace abc::details::cxx11
{
#else
namespace abc
{
namespace details
{
namespace cxx11
{
#endif

struct in_place_t { explicit in_place_t() = default; };

#ifdef ABC_CXX17
}
#else
}
}
}
#endif

#endif // ABC_INCLUDE_ABC_DETAILS_CXX11_UTILITY
