// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_UTILITY)
#define ABC_UTILITY

#pragma once

#include <utility>

namespace abc {

[[noreturn]]
inline void unreachable() {
#if defined(__cpp_lib_unreachable) && (__cpp_lib_unreachable >= 202202L)

    std::unreachable();

#else

#   if defined(__GNUC__) // GCC, Clang, ICC
    __builtin_unreachable();
#   elif defined(_MSC_VER) // MSVC
    __assume(false);
#   endif

#endif
}

}

#endif //ABC_UTILITY
