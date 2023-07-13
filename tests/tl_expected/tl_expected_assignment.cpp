// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/expected.h"

#include <gtest/gtest.h>

TEST(expected, simple_assignment) {
    abc::expected<int, int> e1 = 42;
    abc::expected<int, int> e2 = 17;
    abc::expected<int, int> e3 = 21;
    abc::expected<int, int> e4 = abc::unexpected(42);
    abc::expected<int, int> e5 = abc::unexpected(17);
    abc::expected<int, int> e6 = abc::unexpected(21);

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

    auto unex = abc::unexpected(12);
    e1 = unex;
    ASSERT_TRUE(e1.is_err());
    ASSERT_EQ(12, e1.error());

    e1 = abc::unexpected(42);
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

TEST(expected, assignment_deletion) {
    struct has_all {
        has_all() = default;

        has_all(const has_all &) = default;

        has_all(has_all &&) noexcept = default;

        has_all & operator=(const has_all &) = default;
    };

    abc::expected<has_all, has_all> e1 = {};
    abc::expected<has_all, has_all> e2 = {};
    e1 = e2;

    struct except_move {
        except_move() = default;

        except_move(const except_move &) = default;

        except_move(except_move &&) noexcept(false) {};

        except_move & operator=(const except_move &) = default;
    };

    abc::expected<except_move, except_move> e3 = {};
    abc::expected<except_move, except_move> e4 = {};
    // e3 = e4;    // should not compile
}