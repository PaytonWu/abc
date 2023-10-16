// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/details/config.h>

#include <gtest/gtest.h>

TEST(macro, get_args_count) {
    {
        auto const n = ABC_GET_ARGS_COUNT(1, 2, 3, 4, 5);
        ASSERT_EQ(5, n);
    }
    {
        auto const n = ABC_GET_ARGS_COUNT();
        ASSERT_EQ(0, n);
    }
    {
        auto const n = ABC_GET_ARGS_COUNT("1");
        ASSERT_EQ(1, n);
    }
}