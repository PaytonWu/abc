// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_DETAILS_OPTIONAL_STORAGE
#define ABC_INCLUDE_ABC_DETAILS_OPTIONAL_STORAGE

#pragma once

#include "config.h"
#include "optional_storage_decl.h"

namespace abc
{
namespace details
{
namespace cxx11
{

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

} //namespace cxx11
} //namespace details
} //namespace abc


#endif //ABC_INCLUDE_ABC_DETAILS_OPTIONAL_STORAGE
