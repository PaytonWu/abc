// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/expected.h"

#include <gtest/gtest.h>

#include <expected>

TEST(expected, triviality) {
    ASSERT_TRUE((std::is_trivially_copy_constructible<abc::expected<int, int>>::value));
//    ASSERT_TRUE((std::is_trivially_copy_assignable<abc::expected<int, int>>::value));
    ASSERT_TRUE((std::is_trivially_move_constructible<abc::expected<int, int>>::value));
//    ASSERT_TRUE((std::is_trivially_move_assignable<abc::expected<int, int>>::value));
    ASSERT_TRUE((std::is_trivially_destructible<abc::expected<int, int>>::value));

    ASSERT_TRUE((std::is_trivially_copy_constructible<abc::expected<void, int>>::value));
    ASSERT_TRUE((std::is_trivially_move_constructible<abc::expected<void, int>>::value));
    ASSERT_TRUE((std::is_trivially_destructible<abc::expected<void, int>>::value));

//    ASSERT_TRUE((std::is_trivially_default_constructible<abc::expected<int, int>>::value));

    {
        struct T {
            T(const T &) = default;

            T(T &&) = default;

            T & operator=(const T &) = default;

            T & operator=(T &&) = default;

            ~T() = default;
        };
        ASSERT_TRUE((std::is_trivially_copy_constructible<abc::expected<T, int>>::value));
//        ASSERT_TRUE((std::is_trivially_copy_assignable<abc::expected<T, int>>::value));
        ASSERT_TRUE((std::is_trivially_move_constructible<abc::expected<T, int>>::value));
//        ASSERT_TRUE((std::is_trivially_move_assignable<abc::expected<T, int>>::value));
        ASSERT_TRUE((std::is_trivially_destructible<abc::expected<T, int>>::value));
    }

    {
        struct T {
            T(const T &) {}

            T(T &&) {}

            T & operator=(const T &) { return *this; }

            T & operator=(T &&) { return *this; }

            ~T() {}
        };
        ASSERT_TRUE((!std::is_trivially_copy_constructible<abc::expected<T, int>>::value));
        ASSERT_TRUE((!std::is_trivially_copy_assignable<abc::expected<T, int>>::value));
        ASSERT_TRUE((!std::is_trivially_move_constructible<abc::expected<T, int>>::value));
        ASSERT_TRUE((!std::is_trivially_move_assignable<abc::expected<T, int>>::value));
        ASSERT_TRUE((!std::is_trivially_destructible<abc::expected<T, int>>::value));
    }

}

TEST(expected, deletion) {
    ASSERT_TRUE((std::is_copy_constructible<abc::expected<int, int>>::value));
    ASSERT_TRUE((std::is_copy_assignable<abc::expected<int, int>>::value));
    ASSERT_TRUE((std::is_move_constructible<abc::expected<int, int>>::value));
    ASSERT_TRUE((std::is_move_assignable<abc::expected<int, int>>::value));
    ASSERT_TRUE((std::is_destructible<abc::expected<int, int>>::value));

    {
        struct T {
            T() = default;
        };

        ASSERT_TRUE((std::is_default_constructible<abc::expected<T, int>>::value));
    }

    {
        struct T {
            T(int) {}
        };

        ASSERT_TRUE((!std::is_default_constructible<abc::expected<T, int>>::value));
    }

    {
        struct T {
            T(const T &) = default;

            T(T &&) = default;

            T & operator=(const T &) = default;

            T & operator=(T &&) = default;

            ~T() = default;
        };

        ASSERT_TRUE((std::is_copy_constructible<abc::expected<T, int>>::value));
        ASSERT_TRUE((std::is_copy_assignable<abc::expected<T, int>>::value));
        ASSERT_TRUE((std::is_move_constructible<abc::expected<T, int>>::value));
        ASSERT_TRUE((std::is_move_assignable<abc::expected<T, int>>::value));
        ASSERT_TRUE((std::is_destructible<abc::expected<T, int>>::value));
    }

    {
        struct T {
            T(const T &) = delete;

            T(T &&) = delete;

            T & operator=(const T &) = delete;

            T & operator=(T &&) = delete;
        };

        ASSERT_TRUE((!std::is_copy_constructible<abc::expected<T, int>>::value));
        ASSERT_TRUE((!std::is_copy_assignable<abc::expected<T, int>>::value));
        ASSERT_TRUE((!std::is_move_constructible<abc::expected<T, int>>::value));
        ASSERT_TRUE((!std::is_move_assignable<abc::expected<T, int>>::value));
    }

    {
        struct T {
            T(const T &) = delete;

            T(T &&) = default;

            T & operator=(const T &) = delete;

            T & operator=(T &&) = default;
        };

        ASSERT_TRUE((!std::is_copy_constructible<abc::expected<T, int>>::value));
        ASSERT_TRUE((!std::is_copy_assignable<abc::expected<T, int>>::value));
        ASSERT_TRUE((std::is_move_constructible<abc::expected<T, int>>::value));
        ASSERT_TRUE((std::is_move_assignable<abc::expected<T, int>>::value));
    }

    {
        struct T {
            T(const T &) = default;

            T(T &&) = delete;

            T & operator=(const T &) = default;

            T & operator=(T &&) = delete;
        };

        ASSERT_TRUE((std::is_copy_constructible<abc::expected<T, int>>::value));
        ASSERT_TRUE((std::is_copy_assignable<abc::expected<T, int>>::value));
    }

    {
        abc::expected<int, int> e;
        ASSERT_TRUE(std::is_default_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_constructible<decltype(e)>::value);
        ASSERT_TRUE(std::is_copy_assignable<decltype(e)>::value);
        ASSERT_TRUE(std::is_move_assignable<decltype(e)>::value);
        ASSERT_TRUE(std::is_trivially_copy_constructible<decltype(e)>::value);
//        ASSERT_TRUE (std::is_trivially_copy_assignable<decltype(e)>::value);
#	if !defined(TL_EXPECTED_GCC49)
        ASSERT_TRUE(std::is_trivially_move_constructible<decltype(e)>::value);
//        ASSERT_TRUE(std::is_trivially_move_assignable<decltype(e)>::value);
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

}