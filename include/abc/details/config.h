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

#define ABC_GET_NTH_ARG(_0,  _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, _11, _12, _13, _14, _15, \
                        _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31, \
                        _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47, \
                        _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, \
                        N, ...) N

#define ABC_GET_ARGS_COUNT(...)                                                     \
    ABC_GET_NTH_ARG(__VA_OPT__(,)                                                   \
                    __VA_ARGS__,                                                    \
                    63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, \
                    47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, \
                    31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, \
                    15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0)

#endif
