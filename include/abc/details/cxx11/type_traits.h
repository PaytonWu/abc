// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_DETAILS_CXX11_TYPE_TRAITS
#define ABC_INCLUDE_ABC_DETAILS_CXX11_TYPE_TRAITS

#pragma once

#include "config.h"

#include <utility>

// https://stackoverflow.com/questions/26744589/what-is-a-proper-way-to-implement-is-swappable-to-test-for-the-swappable-concept

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
template <typename T>
struct is_nothrow_swappable_impl;

template <typename T>
struct is_swappable_impl;

struct tag
{
};

// we can tell if an unqualified swap call will call std::swap by writing a swap function with the same signature as std::swap but a unique return type that can then be examined:
// https://stackoverflow.com/a/26745591/565635
template <typename T>
auto
swap(T &, T &) noexcept(std::is_nothrow_move_assignable<T>::value && std::is_nothrow_move_constructible<T>::value) ->
    typename std::enable_if<std::is_move_assignable<T>::value && std::is_move_constructible<T>::value, tag>::type;

template <typename T, std::size_t N>
auto swap(T (&)[N], T (&)[N]) noexcept(is_nothrow_swappable_impl<T>::value) -> typename std::enable_if<is_swappable_impl<T>::value, tag>::type;

struct swappable_tester
{
    template <typename T, typename = decltype(swap(std::declval<T &>(), std::declval<T &>()))>
    static auto
    can_swap(int) -> std::true_type;

    template <typename>
    static auto
    can_swap(...) -> std::false_type;

    // can_swap && !std::swap
    template <typename T,
              typename = typename std::enable_if<decltype(can_swap<T>(0))::value && !std::is_same<decltype(swap(std::declval<T &>(), std::declval<T &>())), tag>::value>::type>
    static auto
    use_adl_swap(int) -> std::true_type;

    template <typename>
    static auto
    use_adl_swap(...) -> std::false_type;

    template <typename T,
              typename = typename std::enable_if<decltype(can_swap<T>(0))::value && std::is_same<decltype(swap(std::declval<T &>(), std::declval<T &>())), tag>::value>::type>
    static auto
    use_std_swap(int) -> std::true_type;

    template <typename>
    static auto
    use_std_swap(...) -> std::false_type;

    template <typename T,
              typename = typename std::enable_if<decltype(can_swap<T>(0))::value && std::is_move_assignable<typename std::remove_reference<T>::type>::value &&
                                                 std::is_move_constructible<typename std::remove_reference<T>::type>::value>::type>
    static auto
    can_std_swap(int) -> std::true_type;

    template <typename>
    static auto
    can_std_swap(...) -> std::false_type;
};

struct nothrow_swappable_tester
{
    template <typename T, typename = decltype(swap(std::declval<T &>(), std::declval<T &>()))>
    static auto
    can_swap(int) noexcept(noexcept(swap(std::declval<T &>(), std::declval<T &>()))) -> std::true_type;

    template <typename>
    static auto
    can_swap(...) noexcept(false) -> std::false_type;

    template <typename T, typename = typename std::enable_if<decltype(can_swap<T>(0))::value && noexcept(can_swap<T>(0))>::type>
    static auto
    can_nothrow_swap(int) noexcept(noexcept(swap(std::declval<T &>(), std::declval<T &>()))) -> std::true_type;

    template <typename>
    static auto
    can_nothrow_swap(...) noexcept(false) -> std::false_type;

    template <
        typename T,
        typename = typename std::enable_if<decltype(can_nothrow_swap<T>(0))::value && !std::is_same<decltype(swap(std::declval<T &>(), std::declval<T &>())), tag>::value>::type>
    static auto
    use_nothrow_adl_swap(int) -> std::true_type;

    template <typename T>
    static auto
    use_nothrow_adl_swap(...) -> std::false_type;

    template <
        typename T,
        typename = typename std::enable_if<decltype(can_nothrow_swap<T>(0))::value && std::is_same<decltype(swap(std::declval<T &>(), std::declval<T &>())), tag>::value>::type>
    static auto
    use_nothrow_std_swap(int) -> std::true_type;

    template <typename T>
    static auto
    use_nothrow_std_swap(...) -> std::false_type;

    template <typename T,
              typename = typename std::enable_if<decltype(can_nothrow_swap<T>(0))::value && std::is_move_assignable<typename std::remove_reference<T>::type>::value &&
                                                 std::is_move_constructible<typename std::remove_reference<T>::type>::value>::type>
    static auto
    can_nothrow_std_swap(int) -> std::true_type;

    template <typename T>
    static auto
    can_nothrow_std_swap(...) -> std::false_type;
};

struct swappable_with_tester
{
    template <typename T, typename U, typename = decltype(swap(std::declval<T>(), std::declval<U>())), typename = decltype(swap(std::declval<U>(), std::declval<T>()))>
    static auto
    can_swap_with(int) -> std::true_type;

    template <typename /*T*/, typename /*U*/>
    static auto
    can_swap_with(...) -> std::false_type;
};

struct nothrow_swappable_with_tester
{
    template <typename T, typename U>
    static auto
    can_nothrow_swap_with(int) -> std::bool_constant<noexcept(swap(std::declval<T>(), std::declval<U>())) && noexcept(swap(std::declval<U>(), std::declval<T>()))>;

    template <typename /*T*/, typename /*U*/>
    static auto
    can_nothrow_swap_with(...) -> std::false_type;
};

#ifdef ABC_CXX17
} // namespace abc::details::cxx11
#else
} // namespace cxx11
} // namespace details
} // namespace abc
#endif

#endif // ABC_INCLUDE_ABC_DETAILS_CXX11_TYPE_TRAITS
