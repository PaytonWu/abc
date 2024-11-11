// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_OPTIONAL_STORAGE_FWD_DECL_H
#define ABC_OPTIONAL_STORAGE_FWD_DECL_H

#pragma once

#include "config.h"

#include <type_traits>

namespace abc::details
{

template <typename T, bool trivially_destructible = std::is_trivially_destructible<T>::value>
struct optional_storage;

}

#endif // ABC_OPTIONAL_STORAGE_FWD_DECL_H
