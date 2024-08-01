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
    using can_swap_boolean_type = decltype(can_swap<T>(0));
    static constexpr bool can_swap_v = can_swap_boolean_type::value;

    using use_std_swap_boolean_type = decltype(use_std_swap<T>(0));
    static constexpr bool use_std_swap_v = use_std_swap_boolean_type::value;

    using type = std::bool_constant<can_swap_v || use_std_swap_v>;
    static constexpr bool value = type::value;
};

template <typename T>
struct is_nothrow_swappable_impl : cxx11::nothrow_swappable_tester
{
    using type = decltype(test<T>(0));
};

} // namespace abc::details

#endif // ABC_DETAILS_TYPE_TRAITS
