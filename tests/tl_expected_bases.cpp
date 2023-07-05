// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/result.h"

#include <gtest/gtest.h>

TEST(result, triviality) {
    ASSERT_TRUE((std::is_trivially_copy_constructible<abc::result<int, int>>::value));
//    ASSERT_TRUE((std::is_trivially_copy_assignable<abc::result<int, int>>::value));
    ASSERT_TRUE((std::is_trivially_move_constructible<abc::result<int, int>>::value));
//    ASSERT_TRUE((std::is_trivially_move_assignable<abc::result<int, int>>::value));
    ASSERT_TRUE((std::is_trivially_destructible<abc::result<int, int>>::value));

    ASSERT_TRUE((std::is_trivially_copy_constructible<abc::result<void, int>>::value));
    ASSERT_TRUE((std::is_trivially_move_constructible<abc::result<void, int>>::value));
    ASSERT_TRUE((std::is_trivially_destructible<abc::result<void, int>>::value));


    {
        struct T {
            T(const T &) = default;

            T(T &&) = default;

            T & operator=(const T &) = default;

            T & operator=(T &&) = default;

            ~T() = default;
        };
        ASSERT_TRUE((std::is_trivially_copy_constructible<abc::result<T, int>>::value));
//        ASSERT_TRUE((std::is_trivially_copy_assignable<abc::result<T, int>>::value));
        ASSERT_TRUE((std::is_trivially_move_constructible<abc::result<T, int>>::value));
//        ASSERT_TRUE((std::is_trivially_move_assignable<abc::result<T, int>>::value));
        ASSERT_TRUE((std::is_trivially_destructible<abc::result<T, int>>::value));
    }

    {
        struct T {
            T(const T &) {}

            T(T &&) {}

            T & operator=(const T &) { return *this; }

            T & operator=(T &&) { return *this; }

            ~T() {}
        };
        ASSERT_TRUE((!std::is_trivially_copy_constructible<abc::result<T, int>>::value));
        ASSERT_TRUE((!std::is_trivially_copy_assignable<abc::result<T, int>>::value));
        ASSERT_TRUE((!std::is_trivially_move_constructible<abc::result<T, int>>::value));
        ASSERT_TRUE((!std::is_trivially_move_assignable<abc::result<T, int>>::value));
        ASSERT_TRUE((!std::is_trivially_destructible<abc::result<T, int>>::value));
    }

}

TEST(result, deletion) {
    ASSERT_TRUE((std::is_copy_constructible<abc::result<int, int>>::value));
    ASSERT_TRUE((std::is_copy_assignable<abc::result<int, int>>::value));
    ASSERT_TRUE((std::is_move_constructible<abc::result<int, int>>::value));
    ASSERT_TRUE((std::is_move_assignable<abc::result<int, int>>::value));
    ASSERT_TRUE((std::is_destructible<abc::result<int, int>>::value));

    {
        struct T {
            T() = default;
        };

        ASSERT_TRUE((std::is_default_constructible<abc::result<T, int>>::value));
    }

    {
        struct T {
            T(int) {}
        };

        ASSERT_TRUE((!std::is_default_constructible<abc::result<T, int>>::value));
    }

    {
        struct T {
            T(const T &) = default;

            T(T &&) = default;

            T & operator=(const T &) = default;

            T & operator=(T &&) = default;

            ~T() = default;
        };

        ASSERT_TRUE((std::is_copy_constructible<abc::result<T, int>>::value));
        ASSERT_TRUE((std::is_copy_assignable<abc::result<T, int>>::value));
        ASSERT_TRUE((std::is_move_constructible<abc::result<T, int>>::value));
        ASSERT_TRUE((std::is_move_assignable<abc::result<T, int>>::value));
        ASSERT_TRUE((std::is_destructible<abc::result<T, int>>::value));
    }

    {
        struct T {
            T(const T &) = delete;

            T(T &&) = delete;

            T & operator=(const T &) = delete;

            T & operator=(T &&) = delete;
        };

        ASSERT_TRUE((!std::is_copy_constructible<abc::result<T, int>>::value));
        ASSERT_TRUE((!std::is_copy_assignable<abc::result<T, int>>::value));
        ASSERT_TRUE((!std::is_move_constructible<abc::result<T, int>>::value));
        ASSERT_TRUE((!std::is_move_assignable<abc::result<T, int>>::value));
    }

    {
        struct T {
            T(const T &) = delete;

            T(T &&) = default;

            T & operator=(const T &) = delete;

            T & operator=(T &&) = default;
        };

        ASSERT_TRUE((!std::is_copy_constructible<abc::result<T, int>>::value));
        ASSERT_TRUE((!std::is_copy_assignable<abc::result<T, int>>::value));
        ASSERT_TRUE((std::is_move_constructible<abc::result<T, int>>::value));
        ASSERT_TRUE((std::is_move_assignable<abc::result<T, int>>::value));
    }

    {
        struct T {
            T(const T &) = default;

            T(T &&) = delete;

            T & operator=(const T &) = default;

            T & operator=(T &&) = delete;
        };

        ASSERT_TRUE((std::is_copy_constructible<abc::result<T, int>>::value));
        ASSERT_TRUE((std::is_copy_assignable<abc::result<T, int>>::value));
    }

    {
        abc::result<int, int> e;
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
        abc::result<int, std::string> e;
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
        abc::result<std::string, int> e;
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
        abc::result<std::string, std::string> e;
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