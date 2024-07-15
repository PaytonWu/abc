// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_DETAILS_CXX17_CONFIG
#define ABC_DETAILS_CXX17_CONFIG

#pragma once

#if __cplusplus >= 201703L
#define ABC_CPP17
#endif

#ifdef ABC_CPP17
#define ABC_CXX17_INLINE inline
#else
#define ABC_CXX17_INLINE
#endif

#endif // ABC_DETAILS_CXX17_CONFIG
