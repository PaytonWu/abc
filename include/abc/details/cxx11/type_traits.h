// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_DETAILS_CXX11_TYPE_TRAITS
#define ABC_DETAILS_CXX11_TYPE_TRAITS

#pragma once

#include "config.h"

#include <utility>

// https://stackoverflow.com/questions/26744589/what-is-a-proper-way-to-implement-is-swappable-to-test-for-the-swappable-concept

namespace abc::details::cxx11
{

struct is_std_swappable_tester
{
    template <typename T, typename = decltype(std::swap(std::declval<T &>(), std::declval<T &>()))>
    static auto
    test(int) -> std::true_type;

    template <typename>
    static auto
    test(...) -> std::false_type;
};

struct tag
{
};

template <typename T>
auto swap(T &, T &) -> tag;

template <typename T, std::size_t N>
auto swap(T (&)[N], T (&)[N]) -> tag;



struct swappable_tester
{
    template <typename T, typename = decltype(swap(std::declval<T &>(), std::declval<T &>()))>
    static auto
    test(int) -> std::true_type;

    template <typename>
    static auto
    test(...) -> std::false_type;

    template <typename T, typename = std::is_same<decltype(swap(std::declval<T &>(), std::declval<T &>())), tag>>
    static auto
    use_std_swap(int) -> std::true_type;

    template <typename>
    static auto
    use_std_swap(...) -> std::false_type;
};

struct nothrow_swappable_tester
{
    template <typename T>
    static auto
    test(int) -> std::bool_constant<noexcept(swap(std::declval<T &>(), std::declval<T &>()))>;

    template <typename T>
    static auto
    test(...) -> std::bool_constant<false>;
};

} // namespace abc::details::cxx11

#endif // ABC_DETAILS_CXX11_TYPE_TRAITS
