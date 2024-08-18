// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_DETAILS_CXX11_OPTIONAL_STORAGE
#define ABC_INCLUDE_ABC_DETAILS_CXX11_OPTIONAL_STORAGE

#pragma once

#include "optional_storage_decl.h"

#include "abc/details/error.h"
#include "abc/error.h"

#include <system_error>

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

template <typename T, bool trivially_destructible>
template <typename... Args>
constexpr optional_storage<T, trivially_destructible>::optional_storage(in_place_t, Args&&... args) : value_{std::forward<Args>(args)...}, has_value_{true}
{
}

template <typename T, bool trivially_destructible>
constexpr auto optional_storage<T, trivially_destructible>::has_value() const & noexcept -> bool
{
    return has_value_;
}

template <typename T, bool trivially_destructible>
constexpr auto optional_storage<T, trivially_destructible>::has_value() const && noexcept -> bool
{
    return has_value_;
}

template <typename T, bool trivially_destructible>
constexpr auto optional_storage<T, trivially_destructible>::value() & noexcept -> T&
{
    if (!has_value_)
    {
        do_throw_error(make_error_code(abc::errc::bad_optional_access));
    }

    return value_;
}

template <typename T, bool trivially_destructible>
constexpr auto optional_storage<T, trivially_destructible>::value() const & noexcept -> const T&
{
    if (!has_value_)
    {
        do_throw_error(make_error_code(abc::errc::bad_optional_access));
    }

    return value_;
}

template <typename T, bool trivially_destructible>
constexpr auto optional_storage<T, trivially_destructible>::value() && noexcept -> T&&
{
    if (!has_value_)
    {
        do_throw_error(make_error_code(abc::errc::bad_optional_access));
    }

    return std::move(value_);
}

template <typename T, bool trivially_destructible>
constexpr auto optional_storage<T, trivially_destructible>::value() const && noexcept -> const T&&
{
    if (!has_value_)
    {
        do_throw_error(make_error_code(abc::errc::bad_optional_access));
    }
    
    return std::move(value_);
}

template <typename T>
template <typename... Args>
constexpr optional_storage<T, false>::optional_storage(in_place_t, Args&&... args) : value_{std::forward<Args>(args)...}, has_value_{true}
{
}

template <typename T>
optional_storage<T, false>::~optional_storage() noexcept(std::is_nothrow_destructible<T>::value)
{
    if (has_value_)
    {
        value_.~T();
    }
}

template <typename T>
constexpr auto optional_storage<T, false>::has_value() const & noexcept -> bool
{
    return has_value_;
}

template <typename T>
constexpr auto optional_storage<T, false>::has_value() const && noexcept -> bool
{
    return has_value_;
}

#ifdef ABC_CXX17
} //namespace abc::details::cxx11
#else
} //namespace cxx11
} //namespace details
} //namespace abc
#endif

#endif //ABC_INCLUDE_ABC_DETAILS_CXX11_OPTIONAL_STORAGE
