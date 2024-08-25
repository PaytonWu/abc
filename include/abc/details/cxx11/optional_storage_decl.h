// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_DETAILS_CXX11_OPTIONAL_STORAGE_DECL
#define ABC_INCLUDE_ABC_DETAILS_CXX11_OPTIONAL_STORAGE_DECL

#pragma once

#include "abc/details/config.h"

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

// T is trivially destructible
template <typename T, bool trivially_destructible = std::is_trivially_destructible<T>::value>
struct optional_storage
{
    template <typename... Args>
    constexpr optional_storage(in_place_t, Args&&... args);

    constexpr ABC_CXX17_NODISCARD auto has_value() const & noexcept -> bool;
    constexpr ABC_CXX17_NODISCARD auto has_value() const && noexcept -> bool;

    constexpr ABC_CXX17_NODISCARD auto value() & noexcept -> T&;
    constexpr ABC_CXX17_NODISCARD auto value() const & noexcept -> const T&;
    constexpr ABC_CXX17_NODISCARD auto value() && noexcept -> T&&;
    constexpr ABC_CXX17_NODISCARD auto value() const && noexcept -> const T&&;

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

// T is not trivially destructible
template <typename T>
struct optional_storage<T, false>
{
    template <typename... Args>
    constexpr optional_storage(in_place_t, Args&&... args);

    ~optional_storage() noexcept(std::is_nothrow_destructible<T>::value);

    constexpr ABC_CXX17_NODISCARD auto has_value() const & noexcept -> bool;
    constexpr ABC_CXX17_NODISCARD auto has_value() const && noexcept -> bool;

    constexpr ABC_CXX17_NODISCARD auto value() & noexcept -> T&;
    constexpr ABC_CXX17_NODISCARD auto value() const & noexcept -> const T&;
    constexpr ABC_CXX17_NODISCARD auto value() && noexcept -> T&&;
    constexpr ABC_CXX17_NODISCARD auto value() const && noexcept -> const T&&;

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

template <typename T, bool trivially_copy_constructable = std::is_trivially_copy_constructible<T>::value>
struct optional_storage_copy_constructable : optional_storage<T>
{
    using optional_storage<T>::optional_storage;
};

template <typename T>
struct optional_storage_copy_constructable<T, false> : optional_storage<T, false>
{
    using optional_storage<T, false>::optional_storage;


};

#if defined(ABC_CXX17)
} // namespace abc::details::cxx11
#else
} // namespace cxx11
} // namespace details
} // namespace abc
#endif

#endif //ABC_INCLUDE_ABC_DETAILS_CXX11_OPTIONAL_STORAGE_DECL
