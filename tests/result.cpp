// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/result.h"

#include <gtest/gtest.h>

TEST(result, default_constructor) {
    enum class errc{};

#define TEST_CASE(TYPE)             \
    do {                            \
        abc::result<TYPE, errc> r;  \
        ASSERT_TRUE(r.has_value()); \
    } while (false)

    TEST_CASE(bool);
    TEST_CASE(uint16_t);
    TEST_CASE(uint32_t);
    TEST_CASE(uint64_t);
    TEST_CASE(int16_t);
    TEST_CASE(int32_t);
    TEST_CASE(int64_t);
    TEST_CASE(float);
    TEST_CASE(double);
    TEST_CASE(char);
    TEST_CASE(char8_t);
    TEST_CASE(char16_t);
    TEST_CASE(char32_t);

    struct st {
        int _;
    };

    TEST_CASE(st);

#undef TEST_CASE
}

TEST(result, constructor) {
    enum class errc {
        err1,
        err2,
        err3
    };
    abc::result<int, errc> r1{ 1 };
    ASSERT_TRUE(r1.has_value());
}

TEST(result, val_constructor) {
    struct base {
        std::string s;
        int count{0};

        base() = default;
        base(base const & rhs) : s(rhs.s), count(rhs.count + 1) {}
        base(base &&) = default;
        base & operator=(base const &) = default;
        base & operator=(base &&) = default;
        virtual ~base() = default;
    };

    struct derived : base {
        std::vector<std::string> strings;
        int countd{0};

        derived() = default;
        derived(derived const & rhs) : base(rhs), strings(rhs.strings), countd(rhs.countd + 1) {}
        derived(derived &&) = default;
        derived & operator=(derived const &) = default;
        derived & operator=(derived &&) = default;
        ~derived() override = default;
    };

    struct trival_base {
        int count{3};
    };

    {
        abc::result<base *, int> r = new derived;
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(0, r.value()->count);
    }

    {
        abc::result<base *, int> r = nullptr;
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(nullptr, r.value());
    }

    {
        abc::result<base *, int> r = new derived;
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(0, r.value()->count);

        abc::result<derived *, int> rr{dynamic_cast<derived *>(r.value())};
        ASSERT_TRUE(rr.has_value());
        ASSERT_EQ(0, rr.value()->count);
    }

    {
        abc::result<base, int> r = base{};
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(0, r.value().count);
    }

    {
        abc::result<base, int> r = derived{};
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(0, r.value().count);
    }

    {
        abc::result<trival_base, int> r = trival_base{.count = 1};
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(1, r.value().count);
    }
}

TEST(result, copy_constructor) {
    struct non_trivally_copyable {
        non_trivally_copyable() = default;
        non_trivally_copyable(non_trivally_copyable&&) = default;
        non_trivally_copyable& operator=(const non_trivally_copyable&) = default;
        non_trivally_copyable& operator=(non_trivally_copyable&&) = default;

        std::string _;
        int count{0};

        non_trivally_copyable(non_trivally_copyable const & other) : _{other._}, count{other.count + 1} {
        }
    };

    struct non_trivally_copyable2 {
        non_trivally_copyable ntc;
    };

    static_assert(!std::is_trivially_copy_constructible_v<non_trivally_copyable>);
    static_assert(!std::is_trivially_copy_constructible_v<non_trivally_copyable2>);

    {
        non_trivally_copyable ntc;
        ntc._ = "abc";

        abc::result<non_trivally_copyable, int> r1{ntc};
        abc::result<non_trivally_copyable, int> r2{r1};
        ASSERT_TRUE(r1.has_value());
        ASSERT_EQ(1, r1.value().count);
        ASSERT_EQ("abc", r1.value()._);

        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(r2.value()._, "abc");
        ASSERT_EQ(2, r2.value().count);
    }

    {
        abc::result<int, non_trivally_copyable> r1{1};
        abc::result<int, non_trivally_copyable> r2{r1};

        ASSERT_EQ(r1.value(), 1);
        ASSERT_EQ(r2.value(), 1);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
    }

    {
        non_trivally_copyable ntc;  // count == 0
        ntc._ = "abc";

        non_trivally_copyable2 ntc2;
        ntc2.ntc = ntc;            // count == 0, assignement
        ASSERT_EQ(0, ntc.count);

        abc::err<non_trivally_copyable2> e{ntc2};   // count == 1
        ASSERT_EQ(1, e.error().ntc.count);

        abc::result<non_trivally_copyable, non_trivally_copyable2> r1{e};   // count == 2
        abc::result<non_trivally_copyable, non_trivally_copyable2> r2{r1};  // count == 3

        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error().ntc.count);
        ASSERT_EQ(3, r2.error().ntc.count);
    }
}

TEST(result, move_constructor) {
    struct non_trivally_moveable {
        non_trivally_moveable() = default;
        non_trivally_moveable(non_trivally_moveable const &) = default;
        non_trivally_moveable& operator=(const non_trivally_moveable&) = default;
        non_trivally_moveable& operator=(non_trivally_moveable&&) = default;

        non_trivally_moveable(non_trivally_moveable&& other) noexcept : _{std::move(other._)}, count{other.count + 1} {
        }

        std::string _;
        int count{0};
    };

    struct non_trivally_moveable2 {
        non_trivally_moveable ntm;
    };

    static_assert(!std::is_trivially_move_constructible_v<non_trivally_moveable>);
    static_assert(!std::is_trivially_move_constructible_v<non_trivally_moveable2>);

    {
        non_trivally_moveable ntm;
        ntm._ = "abc";

        abc::result<non_trivally_moveable, int> r1{std::move(ntm)};
        abc::result<non_trivally_moveable, int> r2{std::move(r1)};
        ASSERT_TRUE(r1.has_value());
        ASSERT_EQ(1, r1.value().count);
        ASSERT_TRUE(r1.value()._.empty());

        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(r2.value()._, "abc");
        ASSERT_EQ(2, r2.value().count);
    }

    {
        abc::result<int, non_trivally_moveable> r1{1};
        abc::result<int, non_trivally_moveable> r2{std::move(r1)};

        ASSERT_EQ(r1.value(), 1);
        ASSERT_EQ(r2.value(), 1);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
    }

    {
        non_trivally_moveable ntm;  // count == 0
        ntm._ = "abc";

        non_trivally_moveable2 ntm2;
        ntm2.ntm = std::move(ntm);            // count == 0, assignement
        ASSERT_EQ(0, ntm.count);

        abc::err<non_trivally_moveable2> e{std::move(ntm)};   // count == 1
        ASSERT_EQ(1, e.error().ntm.count);
    }
}

TEST(result, err_constructor) {
    {
        abc::err<int> e{1};
        abc::result<int, int> r{e};
        ASSERT_FALSE(r.has_value());
        ASSERT_EQ(1, r.error());
    }

    {
        abc::err<int> e{1};
        abc::result<int, int> r{e};
        ASSERT_FALSE(r.has_value());
        ASSERT_EQ(1, r.error());
        ASSERT_THROW({ auto v = r.value(); (void)v; }, abc::abc_error);
    }

    {
        struct non_trivally_copyable {
            non_trivally_copyable() = default;
            non_trivally_copyable(non_trivally_copyable &&) = default;
            non_trivally_copyable& operator=(const non_trivally_copyable&) = default;
            non_trivally_copyable& operator=(non_trivally_copyable&&) = default;

            std::string _;
            int count{0};

            non_trivally_copyable(non_trivally_copyable const & other) : _{other._}, count{other.count + 1} {
            }
        };

        non_trivally_copyable ntc{};
        abc::err<non_trivally_copyable> e{ntc};   // count == 1
        ASSERT_EQ(1, e.error().count);

        abc::result<int, non_trivally_copyable> r{e};   // count == 2
        ASSERT_FALSE(r.has_value());
        ASSERT_EQ(2, r.error().count);
        ASSERT_THROW({ auto v = r.value(); (void)v; }, abc::abc_error);
    }

    {
        struct non_trivally_moveable {
            non_trivally_moveable() = default;
            non_trivally_moveable(non_trivally_moveable const &) = default;
            non_trivally_moveable& operator=(const non_trivally_moveable&) = default;
            non_trivally_moveable& operator=(non_trivally_moveable&&) = default;

            non_trivally_moveable(non_trivally_moveable&& other) noexcept : _{std::move(other._)}, count{other.count + 1} {
            }

            std::string _;
            int count{0};
        };

        abc::err<non_trivally_moveable> e{non_trivally_moveable{}};   // count == 1
        abc::result<int, non_trivally_moveable> r{std::move(e)};   // count == 2
        ASSERT_FALSE(r.has_value());
        ASSERT_EQ(2, r.error().count);
        ASSERT_THROW({ auto v = r.value(); (void)v; }, abc::abc_error);
    }
}

TEST(result, copy_assignment) {
    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{2};
        r2 = r1;
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(1, r1.value());
        ASSERT_EQ(1, r2.value());
    }

    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{2};
        r1 = r2;
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{abc::err<int>{2}};
        r1 = r2;
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(2, r2.error());
    }

    {
        abc::result<int, int> r1{abc::err<int>{1}};
        abc::result<int, int> r2{2};
        r1 = r2;
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::result<int, int> r1{abc::err<int>{1}};
        abc::result<int, int> r2{abc::err<int>{2}};
        r1 = r2;
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(2, r2.error());
    }
}

TEST(result, move_assignment) {
    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{2};
        r2 = std::move(r1);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(1, r1.value());
        ASSERT_EQ(1, r2.value());
    }

    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{2};
        r1 = std::move(r2);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{abc::err<int>{2}};
        r1 = std::move(r2);
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(2, r2.error());
    }

    {
        abc::result<int, int> r1{abc::err<int>{1}};
        abc::result<int, int> r2{2};
        r1 = std::move(r2);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::result<int, int> r1{abc::err<int>{1}};
        abc::result<int, int> r2{abc::err<int>{2}};
        r1 = std::move(r2);
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(2, r2.error());
    }
}

TEST(result, swap) {
    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{2};
        r1.swap(r2);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(1, r2.value());

        std::swap(r1, r2);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(1, r1.value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{abc::err<int>{2}};
        r1.swap(r2);
        ASSERT_FALSE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(1, r2.value());

        std::swap(r1, r2);
        ASSERT_TRUE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r1.value());
        ASSERT_EQ(2, r2.error());
    }

    {
        abc::result<int, int> r1{abc::err<int>{1}};
        abc::result<int, int> r2{2};
        r1.swap(r2);
        ASSERT_TRUE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(1, r2.error());

        std::swap(r1, r2);
        ASSERT_FALSE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(1, r1.error());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::result<int, int> r1{abc::err<int>{1}};
        abc::result<int, int> r2{abc::err<int>{2}};
        r1.swap(r2);
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(1, r2.error());

        std::swap(r1, r2);
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r1.error());
        ASSERT_EQ(2, r2.error());
    }
}

TEST(result, equal_operator) {
    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{2};
        ASSERT_FALSE(r1 == r2);
        ASSERT_TRUE(r1 != r2);
        ASSERT_FALSE(r1 == 2);
        ASSERT_TRUE(r1 != 2);
        ASSERT_FALSE(1 == r2);
        ASSERT_TRUE(1 != r2);
    }

    {
        abc::result<int, int> r1{1};
        abc::result<int, int> r2{abc::err<int>{2}};
        ASSERT_FALSE(r1 == r2);
        ASSERT_TRUE(r1 != r2);
        ASSERT_FALSE(r1 == 2);
        ASSERT_TRUE(r1 != 2);
        ASSERT_FALSE(1 == r2);
        ASSERT_TRUE(1 != r2);
    }

    {
        abc::result<int, int> r1{abc::err<int>{1}};
        abc::result<int, int> r2{2};
        ASSERT_FALSE(r1 == r2);
        ASSERT_TRUE(r1 != r2);
        ASSERT_FALSE(r1 == 2);
        ASSERT_TRUE(r1 != 2);
        ASSERT_FALSE(1 == r2);
        ASSERT_TRUE(1 != r2);
    }

    {
        abc::result<int, int> r1{abc::err<int>{1}};
        abc::result<int, int> r2{abc::err<int>{2}};
        ASSERT_FALSE(r1 == r2);
        ASSERT_TRUE(r1 != r2);
        ASSERT_FALSE(r1 == 2);
        ASSERT_TRUE(r1 != 2);
        ASSERT_FALSE(1 == r2);
        ASSERT_TRUE(1 != r2);
    }
}

TEST(result, value) {
    {
        abc::result<int, int> r1{1};
        ASSERT_EQ(1, r1.value());
        ASSERT_EQ(1, r1.value_or(2));
    }

    {
        abc::result<int, int> r1{abc::err<int>{1}};
        ASSERT_THROW({ auto v = r1.value(); (void)v; }, abc::abc_error);
        ASSERT_EQ(2, r1.value_or(2));
    }
}
