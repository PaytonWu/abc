// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/expected.h"
#include "abc/bytes.h"
#include "abc/hex_utility.h"
#include "abc/hex_string.h"

#include <gtest/gtest.h>

#include <bit>

TEST(expected, default_constructor) {
    enum class errc{};

#define TEST_CASE(TYPE)             \
    do {                            \
        abc::expected<TYPE, errc> r;  \
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

TEST(expected, constructor) {
    enum class errc {
        err1,
        err2,
        err3
    };
    abc::expected<int, errc> r1{ 1 };
    ASSERT_TRUE(r1.has_value());
}

TEST(expected, val_constructor) {
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
        abc::expected<base *, int> r = new derived;
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(0, r.value()->count);
    }

    {
        abc::expected<base *, int> r = nullptr;
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(nullptr, r.value());
    }

    {
        abc::expected<base *, int> r = new derived;
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(0, r.value()->count);

        abc::expected<derived *, int> rr{dynamic_cast<derived *>(r.value())};
        ASSERT_TRUE(rr.has_value());
        ASSERT_EQ(0, rr.value()->count);
    }

    {
        abc::expected<base, int> r = base{};
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(0, r.value().count);
    }

    {
        abc::expected<base, int> r = derived{};
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(0, r.value().count);
    }

    {
        abc::expected<trival_base, int> r = trival_base{.count = 1};
        ASSERT_TRUE(r.has_value());
        ASSERT_EQ(1, r.value().count);
    }
}

TEST(expected, copy_constructor) {
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

        abc::expected<non_trivally_copyable, int> r1{ntc};
        abc::expected<non_trivally_copyable, int> r2{r1};
        ASSERT_TRUE(r1.has_value());
        ASSERT_EQ(1, r1.value().count);
        ASSERT_EQ("abc", r1.value()._);

        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(r2.value()._, "abc");
        ASSERT_EQ(2, r2.value().count);
    }

    {
        abc::expected<int, non_trivally_copyable> r1{1};
        abc::expected<int, non_trivally_copyable> r2{r1};

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

        abc::unexpected<non_trivally_copyable2> e{ntc2};   // count == 1
        ASSERT_EQ(1, e.error().ntc.count);

        abc::expected<non_trivally_copyable, non_trivally_copyable2> r1{e};   // count == 2
        abc::expected<non_trivally_copyable, non_trivally_copyable2> r2{r1};  // count == 3

        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error().ntc.count);
        ASSERT_EQ(3, r2.error().ntc.count);
    }
}

TEST(expected, move_constructor) {
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

        abc::expected<non_trivally_moveable, int> r1{std::move(ntm)};
        abc::expected<non_trivally_moveable, int> r2{std::move(r1)};
        ASSERT_TRUE(r1.has_value());
        ASSERT_EQ(1, r1.value().count);
        ASSERT_TRUE(r1.value()._.empty());

        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(r2.value()._, "abc");
        ASSERT_EQ(2, r2.value().count);
    }

    {
        abc::expected<int, non_trivally_moveable> r1{1};
        abc::expected<int, non_trivally_moveable> r2{std::move(r1)};

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

        abc::unexpected<non_trivally_moveable2> e{std::move(ntm)};   // count == 1
        ASSERT_EQ(1, e.error().ntm.count);
    }
}

TEST(expected, err_constructor) {
    {
        abc::unexpected<int> e{1};
        abc::expected<int, int> r{e};
        ASSERT_FALSE(r.has_value());
        ASSERT_EQ(1, r.error());
    }

    {
        abc::unexpected<int> e{1};
        abc::expected<int, int> r{e};
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
        abc::unexpected<non_trivally_copyable> e{ntc};   // count == 1
        ASSERT_EQ(1, e.error().count);

        abc::expected<int, non_trivally_copyable> r{e};   // count == 2
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

        abc::unexpected<non_trivally_moveable> e{non_trivally_moveable{}};   // count == 1
        abc::expected<int, non_trivally_moveable> r{std::move(e)};   // count == 2
        ASSERT_FALSE(r.has_value());
        ASSERT_EQ(2, r.error().count);
        ASSERT_THROW({ auto v = r.value(); (void)v; }, abc::abc_error);
    }
}

TEST(expected, copy_assignment) {
    {
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{2};
        r2 = r1;
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(1, r1.value());
        ASSERT_EQ(1, r2.value());
    }

    {
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{2};
        r1 = r2;
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{abc::unexpected<int>{2}};
        r1 = r2;
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(2, r2.error());
    }

    {
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        abc::expected<int, int> r2{2};
        r1 = r2;
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        abc::expected<int, int> r2{abc::unexpected<int>{2}};
        r1 = r2;
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(2, r2.error());
    }
}

TEST(expected, move_assignment) {
    {
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{2};
        r2 = std::move(r1);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(1, r1.value());
        ASSERT_EQ(1, r2.value());
    }

    {
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{2};
        r1 = std::move(r2);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{abc::unexpected<int>{2}};
        r1 = std::move(r2);
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(2, r2.error());
    }

    {
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        abc::expected<int, int> r2{2};
        r1 = std::move(r2);
        ASSERT_TRUE(r1.has_value());
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r1.value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        abc::expected<int, int> r2{abc::unexpected<int>{2}};
        r1 = std::move(r2);
        ASSERT_FALSE(r1.has_value());
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(2, r1.error());
        ASSERT_EQ(2, r2.error());
    }
}

TEST(expected, swap) {
    {
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{2};
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
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{abc::unexpected<int>{2}};
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
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        abc::expected<int, int> r2{2};
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
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        abc::expected<int, int> r2{abc::unexpected<int>{2}};
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

TEST(expected, equal_operator) {
    {
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{2};
        ASSERT_FALSE(r1 == r2);
        ASSERT_TRUE(r1 != r2);
        ASSERT_FALSE(r1 == 2);
        ASSERT_TRUE(r1 != 2);
        ASSERT_FALSE(1 == r2);
        ASSERT_TRUE(1 != r2);
    }

    {
        abc::expected<int, int> r1{1};
        abc::expected<int, int> r2{abc::unexpected<int>{2}};
        ASSERT_FALSE(r1 == r2);
        ASSERT_TRUE(r1 != r2);
        ASSERT_FALSE(r1 == 2);
        ASSERT_TRUE(r1 != 2);
        ASSERT_FALSE(1 == r2);
        ASSERT_TRUE(1 != r2);
    }

    {
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        abc::expected<int, int> r2{2};
        ASSERT_FALSE(r1 == r2);
        ASSERT_TRUE(r1 != r2);
        ASSERT_FALSE(r1 == 2);
        ASSERT_TRUE(r1 != 2);
        ASSERT_FALSE(1 == r2);
        ASSERT_TRUE(1 != r2);
    }

    {
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        abc::expected<int, int> r2{abc::unexpected<int>{2}};
        ASSERT_FALSE(r1 == r2);
        ASSERT_TRUE(r1 != r2);
        ASSERT_FALSE(r1 == 2);
        ASSERT_TRUE(r1 != 2);
        ASSERT_FALSE(1 == r2);
        ASSERT_TRUE(1 != r2);
    }
}

TEST(expected, value) {
    {
        abc::expected<int, int> r1{1};
        ASSERT_EQ(1, r1.value());
        ASSERT_EQ(1, r1.value_or(2));
    }

    {
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        ASSERT_THROW({ auto v = r1.value(); (void)v; }, abc::abc_error);
        ASSERT_EQ(2, r1.value_or(2));
    }
}

TEST(expected, value_or) {
    {
        abc::expected<int, int> r1{1};
        ASSERT_EQ(1, r1.value_or(2));
    }

    {
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        ASSERT_EQ(2, r1.value_or(2));
    }

    ASSERT_EQ(1, (abc::expected<int, int>{1}.value_or(2)));
    ASSERT_EQ(2, (abc::expected<int, int>{abc::unexpected<int>{1}}.value_or(2)));

    struct base {
        int i;

        base() = default;
        base(base const&) = default;
        auto operator=(base const&) -> base & = default;
        base(base &&) = default;
        auto operator=(base &&) -> base & = default;
        ~base() = default;

        base(int i) : i(i) {}
    };

    struct derived : base {
        int j;

        derived() = default;
        derived(derived const&) = default;
        auto operator=(derived const&) -> derived & = default;
        derived(derived &&) = default;
        auto operator=(derived &&) -> derived & = default;
        ~derived() = default;

        derived(int i, int j) : base(i), j(j) {}
    };

    {
        abc::expected<base, int> r1{derived{1,2}};
        auto r2 = r1.value_or(derived{3,4});
        ASSERT_EQ(1, r2.i);
    }

    {
        abc::expected<int,  base> r1{abc::unexpected<derived>{derived{1,2}}};
        auto r2 = r1.value_or(3);
        ASSERT_EQ(3, r2);
    }
}

TEST(expected, map_copyable_value) {
    {
        abc::expected<int, int> r1{1};
        auto r2 = r1.map([](int v) { return v + 1; });
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::expected<int, int> const r1{1};
        auto r2 = r1.map([](int v) { return v + 1; });
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r2.value());
    }

    {
        auto r2 = abc::expected<int, int>{1}.map([](int v) { return v + 1; });
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r2.value());
    }

    {
        auto r2 = static_cast<abc::expected<int, int> const &&>(abc::expected<int, int>{1}).map([](int v) { return v + 1; });
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(2, r2.value());
    }

    {
        abc::expected<int, int> r1{abc::unexpected<int>{1}};
        auto r2 = r1.map([](int v) { return v + 1; });
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r2.error());
    }

    {
        abc::expected<int, int> const r1{abc::unexpected<int>{1}};
        auto r2 = r1.map([](int v) { return v + 1; });
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r2.error());
    }

    {
        auto r2 = abc::expected<int, int>{abc::unexpected<int>{1}}.map([](int v) { return v + 1; });
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r2.error());
    }

    {
        auto r2 = static_cast<abc::expected<int, int> const &&>(abc::expected<int, int>{abc::unexpected<int>{1}}).map([](int v) { return v + 1; });
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r2.error());
    }
}

TEST(expected, map_non_copyable_value) {
    struct noncopyable {
    private:
        std::string str_;

    public:
        noncopyable();
        noncopyable(noncopyable const &) = delete;
        auto operator=(noncopyable const &) -> noncopyable & = delete;
        noncopyable(noncopyable &&) = default;
        auto operator=(noncopyable &&) -> noncopyable & = default;
        ~noncopyable() = default;

        constexpr noncopyable(std::string_view str) : str_{str} {}

        constexpr auto value() & -> std::string & { return str_; }
        constexpr auto value() const & -> std::string const & { return str_; }
        constexpr auto value() && -> std::string && { return std::move(str_); }
        constexpr auto value() const && -> std::string const && { return std::move(str_); }
    };

    static_assert(!std::is_copy_constructible_v<noncopyable>);
    static_assert(!std::is_copy_assignable_v<noncopyable>);

    {
        abc::expected<noncopyable, int> r1{"abc"};
        auto r2 = r1.map([](auto & v) { return v.value(); });
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(r1.value().value(), r2.value());
    }

    {
        abc::expected<noncopyable, int> const r1{"a"};
        auto r2 = r1.map([](auto const & v) { return v.value(); });
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ(r1.value().value(), r2.value());
    }

    {
        auto r2 = abc::expected<noncopyable, int>{"abc"}.map([](auto && v) { return std::move(v).value(); });
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ("abc", r2.value());
    }

    {
        auto r2 = static_cast<abc::expected<noncopyable, int> const &&>(abc::expected<noncopyable, int>{"abc"}).map([](auto const && v) { return std::move(v).value(); });
        ASSERT_TRUE(r2.has_value());
        ASSERT_EQ("abc", r2.value());
    }

    {
        abc::expected<noncopyable, int> r1{abc::unexpected<int>{1}};
        auto r2 = r1.map([](auto & v) { return v.value().length() + 1; });
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r2.error());
    }

    {
        abc::expected<noncopyable, int> const r1{abc::unexpected<int>{1}};
        auto r2 = r1.map([](auto const & v) { return v.value().length() + 1; });
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r2.error());
    }

    {
        auto r2 = abc::expected<noncopyable, int>{abc::unexpected<int>{1}}.map([](auto && v) { return std::move(v).value().length() + 1; });
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r2.error());
    }

    {
        auto r2 = static_cast<abc::expected<noncopyable, int> const &&>(abc::expected<noncopyable, int>{abc::unexpected<int>{1}}).map([](auto const && v) { return std::move(v).value().length() + 1; });
        ASSERT_FALSE(r2.has_value());
        ASSERT_EQ(1, r2.error());
    }
}

TEST(expected, is_ok_and) {
    {
        auto r1 = abc::expected<int, int>{1};
        ASSERT_TRUE(r1.is_ok_and([](int v) { return v == 1; }));
        ASSERT_FALSE(r1.is_ok_and([](int v) { return v == 2; }));
    }

    {
        auto r1 = abc::expected<int, int>{abc::unexpected<int>{1}};
        ASSERT_FALSE(r1.is_ok_and([](int v) { return v == 1; }));
        ASSERT_FALSE(r1.is_ok_and([](int v) { return v == 2; }));
    }
}

TEST(expected, is_err_and) {
    {
        auto r1 = abc::expected<int, int>{1};
        ASSERT_FALSE(r1.is_err_and([](int v) { return v == 1; }));
        ASSERT_FALSE(r1.is_err_and([](int v) { return v == 2; }));
    }

    {
        auto r1 = abc::expected<int, int>{abc::unexpected<int>{1}};
        ASSERT_TRUE(r1.is_err_and([](int v) { return v == 1; }));
        ASSERT_FALSE(r1.is_err_and([](int v) { return v == 2; }));
    }
}

TEST(expected, ok) {
    {
        auto r1 = abc::expected<int, int>{1};
        ASSERT_TRUE(r1.ok().has_value());
        ASSERT_EQ(1, r1.ok().value());
    }

    {
        auto r1 = abc::expected<int, int>{abc::unexpected<int>{1}};
        ASSERT_FALSE(r1.ok().has_value());
    }
}

TEST(expected, err) {
    {
        auto r1 = abc::expected<int, int>{1};
        ASSERT_EQ(std::nullopt, r1.err());
    }

    {
        auto r1 = abc::expected<int, int>{abc::unexpected<int>{1}};
        ASSERT_TRUE(r1.err().has_value());
        ASSERT_EQ(1, r1.err().value());
    }
}

namespace wrapper {
template <std::endian Endian>
constexpr auto hex_string_to_binary(std::string_view string_slice) -> abc::expected<abc::bytes, abc::errc> {
    if (abc::hex_string::has_hex_prefix(string_slice)) {
        string_slice.remove_prefix(2);
    }

    if (!abc::hex_string::is_hex(string_slice)) {
        return abc::unexpected{abc::errc::invalid_hex_char};
    }

    abc::bytes binary_data;
    binary_data.reserve((string_slice.size() + 1) / 2);
    if constexpr (Endian == std::endian::big) {
        if (string_slice.size() & 1) {
            [[maybe_unused]] auto r = abc::hex_utility::hex_char_to_binary(string_slice.front()).transform([&binary_data, &string_slice](auto const b) mutable {
                binary_data.push_back(b);
                string_slice.remove_prefix(1);
            });
            assert(r.has_value());
        }

        auto const & chunks = string_slice | ranges::views::chunk(2);
        ranges::for_each(chunks, [&binary_data](ranges::viewable_range auto && compound_byte) mutable {
            abc::byte byte{};
            for (auto const [i, nibble_byte]: compound_byte | ranges::views::reverse | ranges::views::enumerate) {
                byte |= abc::hex_utility::hex_char_to_binary(nibble_byte).value() << (4 * i);
            }
            binary_data.push_back(byte);
        });

        return binary_data;
    }

    if constexpr (Endian == std::endian::little) {
        if (string_slice.size() & 1) {
            [[maybe_unused]] auto r = abc::hex_utility::hex_char_to_binary(string_slice.back()).transform([&binary_data, &string_slice](auto const b) mutable {
                binary_data.push_back(b);
                string_slice.remove_suffix(1);
            });
            assert(r.has_value());
        }

        auto const & chunks = string_slice | ranges::views::reverse | ranges::views::chunk(2);
        ranges::for_each(chunks, [&binary_data](ranges::viewable_range auto && compound_byte) mutable {
            abc::byte byte{};
            for (auto const [i, nibble_byte]: compound_byte | ranges::views::enumerate) {
                byte |= hex_char_to_binary(nibble_byte).value() << (4 * i);
            }
            binary_data.push_back(byte);
        });
        return binary_data;
    }
}

constexpr static auto from_hex_prefixed(std::string_view input) -> abc::expected<abc::hex_string, abc::errc> {
    if (!input.starts_with(abc::hex_string::prefix) && !input.starts_with(abc::hex_string::prefix_uppercase)) {
        return abc::unexpected{abc::errc::invalid_hex_string};
    }

    return hex_string_to_binary<std::endian::big>(input).transform([](auto && bytes) {
        ranges::reverse(bytes);
        return abc::hex_string::from<abc::byte_numbering::msb0>(std::move(bytes));
    });
}

}  // namespace wrapper

inline auto func(std::string_view) -> abc::expected<abc::bytes, abc::errc> {
    abc::bytes bytes{1, 2, 3};
    return bytes;
}

TEST(expected, transform_lambda) {
    {
        auto r1 = wrapper::hex_string_to_binary<std::endian::big>("0x1234").transform([](auto && v) { ranges::reverse(v); return v; });
        ASSERT_TRUE(r1.has_value());
    }
}
