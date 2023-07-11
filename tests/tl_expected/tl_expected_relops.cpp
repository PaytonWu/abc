// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/result.h"
#include <gtest/gtest.h>

TEST(result, relops) {
    abc::result<int, int> o1 = 42;
    // abc::result<int, int> o2{tl::unexpect, 0};
    const abc::result<int, int> o3 = 42;

    ASSERT_TRUE(o1 == o1);
    // ASSERT_TRUE(o1 != o2);
    ASSERT_TRUE(o1 == o3);
    ASSERT_TRUE(o3 == o3);

    abc::result<void, int> o6;

    ASSERT_TRUE(o6 == o6);
}