// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/expected.h"

#include <gtest/gtest.h>

struct takes_init_and_variadic {
    std::vector<int> v;
    std::tuple<int, int> t;
    template <class... Args>
    takes_init_and_variadic(std::initializer_list<int> l, Args &&... args)
        : v(l), t(std::forward<Args>(args)...) {}
};

TEST(expected, constructors) {
    {
        abc::expected<int, int> e;
        ASSERT_TRUE(e.has_value());
        ASSERT_TRUE(e == 0);
    }

    {
        abc::expected<int, int> e = abc::unexpected{0};
        ASSERT_TRUE(!e.has_value());
        ASSERT_TRUE(e.error() == 0);
    }

    {
        abc::expected<int, int> e(abc::unexpect, 0);
        ASSERT_TRUE(!e);
        ASSERT_TRUE(e.error() == 0);
    }

    {
        abc::expected<int, int> e(std::in_place, 42);
        ASSERT_TRUE(e);
        ASSERT_TRUE(e == 42);
    }

    {
        abc::expected<std::vector<int>, int> e(std::in_place, {0, 1});
        ASSERT_TRUE(e);
        ASSERT_TRUE((*e)[0] == 0);
        ASSERT_TRUE((*e)[1] == 1);
    }

    {
        abc::expected<std::tuple<int, int>, int> e(std::in_place, 0, 1);
        ASSERT_TRUE(e);
        ASSERT_TRUE(std::get<0>(*e) == 0);
        ASSERT_TRUE(std::get<1>(*e) == 1);
    }

    {
        abc::expected<takes_init_and_variadic, int> e(std::in_place, {0, 1}, 2, 3);
        ASSERT_TRUE(e);
        ASSERT_TRUE(e->v[0] == 0);
        ASSERT_TRUE(e->v[1] == 1);
        ASSERT_TRUE(std::get<0>(e->t) == 2);
        ASSERT_TRUE(std::get<1>(e->t) == 3);
    }

    {
        abc::expected<int, int> e;
        ASSERT_TRUE(std::is_default_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_assignable<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_assignable<decltype(e)>::value);
        ASSERT_TRUE(std::is_trivially_copy_constructible<decltype(e)>::value);
// ASSERT_TRUE(std::is_trivially_copy_assignable<decltype(e)>::value);
#	if !defined(TL_EXPECTED_GCC49)
        ASSERT_TRUE(std::is_trivially_move_constructible<decltype(e)>::value);
// ASSERT_TRUE(std::is_trivially_move_assignable<decltype(e)>::value);
#	endif
    }

    {
        abc::expected<int, std::string> e;
        ASSERT_TRUE(std::is_default_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_assignable<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_assignable<decltype(e)>::value);
        ASSERT_TRUE(!std::is_trivially_copy_constructible<decltype(e)>::value);
        ASSERT_TRUE(!std::is_trivially_copy_assignable<decltype(e)>::value);
#	if !defined(TL_EXPECTED_GCC49)
        ASSERT_TRUE(!std::is_trivially_move_constructible<decltype(e)>::value);
        ASSERT_TRUE(!std::is_trivially_move_assignable<decltype(e)>::value);
#	endif
    }

    {
        abc::expected<std::string, int> e;
        ASSERT_TRUE(std::is_default_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_assignable<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_assignable<decltype(e)>::value);
        ASSERT_TRUE(!std::is_trivially_copy_constructible<decltype(e)>::value);
        ASSERT_TRUE(!std::is_trivially_copy_assignable<decltype(e)>::value);
#	if !defined(TL_EXPECTED_GCC49)
        ASSERT_TRUE(!std::is_trivially_move_constructible<decltype(e)>::value);
        ASSERT_TRUE(!std::is_trivially_move_assignable<decltype(e)>::value);
#	endif
    }

    {
        abc::expected<std::string, std::string> e;
        ASSERT_TRUE(std::is_default_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_assignable<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_assignable<decltype(e)>::value);
        ASSERT_TRUE(!std::is_trivially_copy_constructible<decltype(e)>::value);
        ASSERT_TRUE(!std::is_trivially_copy_assignable<decltype(e)>::value);
#	if !defined(TL_EXPECTED_GCC49)
        ASSERT_TRUE(!std::is_trivially_move_constructible<decltype(e)>::value);
        ASSERT_TRUE(!std::is_trivially_move_assignable<decltype(e)>::value);
#	endif
    }

    {
        abc::expected<void, int> e;
        ASSERT_TRUE(e.has_value());
    }

    {
        abc::expected<void, int> e(abc::unexpect, 42);
        ASSERT_TRUE(!e);
        ASSERT_TRUE(e.error() == 42);
    }
}
