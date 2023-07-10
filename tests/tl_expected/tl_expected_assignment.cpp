// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/result.h"

#include <gtest/gtest.h>

TEST(result, simple_assignment) {
    abc::result<int, int> e1 = 42;
    abc::result<int, int> e2 = 17;
    abc::result<int, int> e3 = 21;
    abc::result<int, int> e4 = abc::err(42);
    abc::result<int, int> e5 = abc::err(17);
    abc::result<int, int> e6 = abc::err(21);

    e1 = e2;
    ASSERT_TRUE(e1.has_value());
    ASSERT_EQ(17, e1.value());
    ASSERT_TRUE(e2.has_value());
    ASSERT_EQ(17, e2.value());

    e1 = std::move(e2);
    ASSERT_TRUE(e1.has_value());
    ASSERT_EQ(17, e1.value());
    ASSERT_TRUE(e2.has_value());
    ASSERT_EQ(17, e2.value());

    e1 = 42;
    ASSERT_TRUE(e1.has_value());
    ASSERT_EQ(42, e1.value());

    auto unex = abc::err(12);
    e1 = unex;
    ASSERT_TRUE(e1.is_err());
    ASSERT_EQ(12, e1.error());

    e1 = abc::err(42);
    ASSERT_TRUE(!e1.has_value());
    ASSERT_EQ(42, e1.error());

    e1 = e3;
    ASSERT_TRUE(e1.has_value());
    ASSERT_EQ(21, e1.value());

    e4 = e5;
    ASSERT_TRUE(!e4.has_value());
    ASSERT_EQ(17, e4.error());

    e4 = std::move(e6);
    ASSERT_TRUE(!e4.has_value());
    ASSERT_EQ(21, e4.error());

    e4 = e1;
    ASSERT_TRUE(e4.has_value());
    ASSERT_EQ(21, e4.value());
}

TEST(result, assignment_deletion) {
    struct has_all {
        has_all() = default;

        has_all(const has_all &) = default;

        has_all(has_all &&) noexcept = default;

        has_all & operator=(const has_all &) = default;
    };

    abc::result<has_all, has_all> e1 = {};
    abc::result<has_all, has_all> e2 = {};
    e1 = e2;

    struct except_move {
        except_move() = default;

        except_move(const except_move &) = default;

        except_move(except_move &&) noexcept(false) {};

        except_move & operator=(const except_move &) = default;
    };

    abc::result<except_move, except_move> e3 = {};
    abc::result<except_move, except_move> e4 = {};
    // e3 = e4;    // should not compile
}