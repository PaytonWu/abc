// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/clonable.h>

#include <gtest/gtest.h>

#include <memory>

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

TEST(clonable, clone_to_other_type)
{
    struct foo : abc::clonable<foo, int>
    {
        [[nodiscard]] auto clone() const -> int override
        {
            return 42;
        }
    };

    foo f;
    auto f2 = f.clone();
    EXPECT_EQ(42, f2);
}

TEST(clonable, clone_to_other_type_with_members)
{
    struct foo : abc::clonable<foo, int>
    {
        int x = 42;

        [[nodiscard]] auto clone() const -> int override
        {
            return x;
        }
    };

    foo f;
    auto f2 = f.clone();
    EXPECT_EQ(42, f2);
}

TEST(clonable, clone_to_shared_ptr)
{
    struct foo : abc::clonable<foo, std::shared_ptr<foo>>
    {
        [[nodiscard]] auto clone() const -> std::shared_ptr<foo> override
        {
            return std::make_shared<foo>();
        }
    };

    foo f;
    auto f2 = f.clone();
    EXPECT_NE(&f, f2.get());
}

TEST(clonable, clone_to_unique_ptr)
{
    struct foo : abc::clonable<foo, std::unique_ptr<foo>>
    {
        [[nodiscard]] auto clone() const -> std::unique_ptr<foo> override
        {
            return std::make_unique<foo>();
        }
    };

    foo f;
    auto f2 = f.clone();
    EXPECT_NE(&f, f2.get());
}