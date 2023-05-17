// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_CONFIG)
#define ABC_DETAILS_CONFIG

#pragma once

#if !defined(__cplusplus)
#   error abc requires a C++ compiler, please use a C++ compiler to compile abc
#endif

#if __cplusplus >= 202002L
#   define ABC_CPP20
#endif

// ABC_OS_XXXX define OS platform
#if defined (_WIN32)
#   define ABC_OS_WINDOWS
#elif defined (__linux__)
#   if defined (__ANDROID__)
#       define ABC_OS_ANDROID
#   else
#       define ABC_OS_LINUX
#   endif
#elif defined (__APPLE__) && defined (__MACH__)
#   include <TargetConditionals.h>
#   if TARGET_IPHONE_SIMULATOR == 1
#       define ABC_OS_IPHONE_SIMULATOR
#   elif TARGET_OS_IPHONE == 1
#       define ABC_OS_IPHONE
#   elif TARGET_OS_MAC == 1
#       define ABC_OS_MAC
#   else
#       error unknown Apple OS
#   endif
#else
#   error unknown OS
#endif

// ABC_COMPILER_XXXX define compiler toolchain
#if defined (ABC_OS_WINDOWS)
#   if defined (_MSC_VER)
#       define ABC_COMPILER_MSVC
#   elif defined (__clang__)
#       define ABC_COMPILER_CLANG
#   elif defined (__GNUC__)
#       define ABC_COMPILER_GCC
#   else
#       error not supported compiler on Windows
#   endif
#elif defined (ABC_OS_LINUX)
#   if defined (__clang__)
#       define ABC_COMPILER_CLANG
#   elif defined (__GNUC__)
#       define ABC_COMPILER_GCC
#   else
#       error not supported compiler on Linux
#   endif
#elif defined (ABC_OS_APPLE)
#   if defined (__clang__)
#       define ABC_COMPILER_CLANG
#   else
#       error not supported compiler on Apple platform
#   endif
#endif

#if (defined (ABC_OS_LINUX) || defined (ABC_OS_APPLE))
#   if !defined(_GNU_SOURCE)
#       define _GNU_SOURCE
#   endif
#endif

#endif
