// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_DETAILS_CXX11_OPTIONAL_STORAGE_DECL
#define ABC_INCLUDE_ABC_DETAILS_CXX11_OPTIONAL_STORAGE_DECL

#pragma once

#include "config.h"
#include "utility.h"

#include <type_traits>
#include <utility>

#if defined(ABC_CXX17)
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

template <typename T, bool trivially_destructible = std::is_trivially_destructible<T>::value>
struct optional_storage
{
    template <typename... Args>
    constexpr optional_storage(in_place_t, Args&&... args);

    constexpr auto has_value() const & noexcept -> bool;
    constexpr auto has_value() const && noexcept -> bool;

protected:
    struct alignas(T) dummy_storage
    {
        unsigned char dummy_[sizeof(T)];
    };
    union
    {
        dummy_storage storage_{};
        T value_;
    };

    bool has_value_{false};
};

template <typename T>
struct optional_storage<T, false>
{
    template <typename... Args>
    constexpr optional_storage(in_place_t, Args&&... args);

    ~optional_storage() noexcept(std::is_nothrow_destructible<T>::value);

    constexpr auto has_value() const & noexcept -> bool;
    constexpr auto has_value() const && noexcept -> bool;

protected:
    struct alignas(T) dummy_storage
    {
        unsigned char dummy_[sizeof(T)];
    };
    union
    {
        dummy_storage storage_{};
        T value_;
    };

    bool has_value_{false};
};

#if defined(ABC_CXX17)
} // namespace abc::details::cxx11
#else
} // namespace cxx11
} // namespace details
} // namespace abc
#endif

#endif //ABC_INCLUDE_ABC_DETAILS_CXX11_OPTIONAL_STORAGE_DECL
