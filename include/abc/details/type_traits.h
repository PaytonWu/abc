// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_DETAILS_TYPE_TRAITS
#define ABC_DETAILS_TYPE_TRAITS

#pragma once

#include "config.h"
#include "cxx11/type_traits.h"

namespace abc::details
{

template <typename T>
struct is_swappable_impl : cxx11::swappable_tester
{
    using type = decltype(test<T>(0));
};

template <typename T>
struct is_nothrow_swappable_impl : cxx11::nothrow_swappable_tester
{
    using type = decltype(test<T>(0));
};

} // namespace abc::details

#endif // ABC_DETAILS_TYPE_TRAITS
