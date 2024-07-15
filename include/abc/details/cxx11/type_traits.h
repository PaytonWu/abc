// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_DETAILS_CXX11_TYPE_TRAITS
#define ABC_DETAILS_CXX11_TYPE_TRAITS

#pragma once

#include "config.h"

#include <utility>

namespace abc::details::cxx11
{

using std::swap;

struct swappable_tester
{
    template <typename T, typename = decltype(swap(std::declval<T &>(), std::declval<T &>()))>
    static auto
    test(int) -> std::true_type;

    template <typename>
    static auto
    test(...) -> std::false_type;
};

} // namespace abc::details::cxx11

#endif // ABC_DETAILS_CXX11_TYPE_TRAITS
