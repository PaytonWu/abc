// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/expected.h"

#include <gtest/gtest.h>

#include <vector>
#include <tuple>

namespace {
struct takes_init_and_variadic {
    std::vector<int> v;
    std::tuple<int, int> t;

    template <class... Args>
    takes_init_and_variadic(std::initializer_list<int> l, Args && ... args)
        : v(l), t(std::forward<Args>(args)...) {}
};
}

TEST(expected, emplace) {
    {
        abc::expected<std::unique_ptr<int>, int> e;
        e.emplace(new int{42});
        ASSERT_TRUE(e);
        ASSERT_TRUE(**e == 42);
    }

    {
        abc::expected<std::vector<int>, int> e;
        e.emplace({0, 1});
        ASSERT_TRUE(e);
        ASSERT_TRUE((*e)[0] == 0);
        ASSERT_TRUE((*e)[1] == 1);
    }

    {
        abc::expected<std::tuple<int, int>, int> e;
        e.emplace(2, 3);
        ASSERT_TRUE(e);
        ASSERT_TRUE(std::get<0>(*e) == 2);
        ASSERT_TRUE(std::get<1>(*e) == 3);
    }

    {
        abc::expected<takes_init_and_variadic, int> e = abc::make_unexpected(0);
        e.emplace({0, 1}, 2, 3);
        ASSERT_TRUE(e);
        ASSERT_TRUE(e->v[0] == 0);
        ASSERT_TRUE(e->v[1] == 1);
        ASSERT_TRUE(std::get<0>(e->t) == 2);
        ASSERT_TRUE(std::get<1>(e->t) == 3);
    }
}
