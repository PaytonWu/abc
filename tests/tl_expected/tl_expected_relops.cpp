// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/expected.h"
#include <gtest/gtest.h>

TEST(expected, relops) {
    abc::expected<int, int> o1 = 42;
    abc::expected<int, int> o2{abc::unexpect, 0};
    const abc::expected<int, int> o3 = 42;

    ASSERT_TRUE(o1 == o1);
    ASSERT_TRUE(o1 != o2);
    ASSERT_TRUE(o1 == o3);
    ASSERT_TRUE(o3 == o3);

    abc::expected<void, int> o6;

    ASSERT_TRUE(o6 == o6);
}