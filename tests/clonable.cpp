// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/clonable.h>

#include <gtest/gtest.h>

TEST(clonable, clone_no_members)
{
struct foo : abc::clonable<foo>
    {
        [[nodiscard]] auto clone() const -> foo override
        {
            return foo{};
        }
    };

    foo f;
    auto f2 = f.clone();
    EXPECT_NE(&f, &f2);
}

TEST(clonable, clone_with_members)
{
    struct foo : abc::clonable<foo>
    {
        int x = 42;

        [[nodiscard]] auto clone() const -> foo override
        {
            return foo{x};
        }

        foo() = default;
        foo(int x) : x{x} {}
    };

    foo f;
    auto f2 = f.clone();
    EXPECT_NE(&f, &f2);
    EXPECT_EQ(f.x, f2.x);
}
