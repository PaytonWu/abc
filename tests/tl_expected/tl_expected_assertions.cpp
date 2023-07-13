// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if defined(assert)
#undef assert
#define assert(cond) TL_ASSERT(cond)
#endif

#include "abc/expected.h"

#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#define TL_ASSERT(cond) if (!(cond)) { throw std::runtime_error(std::string("assertion failure")); }

#if defined(assert)
#undef assert
#define assert(cond) TL_ASSERT(cond)
#endif

TEST(expected, assertions) {
    // abc::expected<int,int> o1 = 42;
    // EXPECT_THROW(o1.error(), std::runtime_error);

    // abc::expected<int,int> o2 {abc::unexpect, 0};
    // EXPECT_THROW({auto v = *o2; (void)v; }, std::runtime_error);

    // struct foo { int bar; };
    // abc::expected<struct foo,int> o3 {abc::unexpect, 0};
    // EXPECT_THROW(o3->bar, std::runtime_error);
}
