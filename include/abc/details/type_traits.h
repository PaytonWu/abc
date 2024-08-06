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
    using type = decltype(can_swap<T>(0));
    static constexpr bool value = type::value;

    using use_adl_swap_boolean_type = decltype(use_adl_swap<T>(0));
    static constexpr bool use_adl_swap_v = use_adl_swap_boolean_type::value;

    using use_std_swap_boolean_type = decltype(use_std_swap<T>(0));
    static constexpr bool use_std_swap_v = use_std_swap_boolean_type::value;
};

template <typename T>
struct is_nothrow_swappable_impl : cxx11::nothrow_swappable_tester
{
    using type = decltype(can_nothrow_swap<T>(0));
    static constexpr bool value = type::value;

    using use_nothrow_adl_swap_boolean_type = decltype(use_nothrow_adl_swap<T>(0));
    static constexpr bool use_nothrow_adl_swap_v = use_nothrow_adl_swap_boolean_type::value;

    using use_nothrow_std_swap_boolean_type = decltype(use_nothrow_std_swap<T>(0));
    static constexpr bool use_nothrow_std_swap_v = use_nothrow_std_swap_boolean_type::value;
};

} // namespace abc::details

#endif // ABC_DETAILS_TYPE_TRAITS
