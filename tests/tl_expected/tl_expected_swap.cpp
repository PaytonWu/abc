// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/expected.h"
#include <gtest/gtest.h>

struct no_throw {
    no_throw(std::string i) : i(i) {}

    std::string i;
};

struct canthrow_move {
    canthrow_move(std::string i) : i(i) {}

    canthrow_move(canthrow_move const &) = default;

    canthrow_move(canthrow_move && other) noexcept(false) : i(other.i) {}

    canthrow_move & operator=(canthrow_move &&) = default;

    std::string i;
};

bool should_throw = false;

// #ifdef TL_EXPECTED_EXCEPTIONS_ENABLED
struct willthrow_move {
    willthrow_move(std::string i) : i(i) {}

    willthrow_move(willthrow_move const &) = default;

    willthrow_move(willthrow_move && other) : i(other.i) {
        if (should_throw)
            throw 0;
    }

    willthrow_move & operator=(willthrow_move &&) = default;

    std::string i;
};
// #endif // TL_EXPECTED_EXCEPTIONS_ENABLED

static_assert(std::is_swappable<no_throw>::value);

template <class T1, class T2>
void swap_test() {
    std::string s1 = "abcdefghijklmnopqrstuvwxyz";
    std::string s2 = "zyxwvutsrqponmlkjihgfedcba";

    abc::expected<T1, T2> a{s1};
    abc::expected<T1, T2> b{s2};
    swap(a, b);
    ASSERT_TRUE(a->i == s2);
    ASSERT_TRUE(b->i == s1);

    a = s1;
    b = abc::unexpected<T2>(s2);
    swap(a, b);
    ASSERT_TRUE(a.error().i == s2);
    ASSERT_TRUE(b->i == s1);

    a = abc::unexpected<T2>(s1);
    b = s2;
    swap(a, b);
    ASSERT_TRUE(a->i == s2);
    ASSERT_TRUE(b.error().i == s1);

    a = abc::unexpected<T2>(s1);
    b = abc::unexpected<T2>(s2);
    swap(a, b);
    ASSERT_TRUE(a.error().i == s2);
    ASSERT_TRUE(b.error().i == s1);

    a = s1;
    b = s2;
    a.swap(b);
    ASSERT_TRUE(a->i == s2);
    ASSERT_TRUE(b->i == s1);

    a = s1;
    b = abc::unexpected<T2>(s2);
    a.swap(b);
    ASSERT_TRUE(a.error().i == s2);
    ASSERT_TRUE(b->i == s1);

    a = abc::unexpected<T2>(s1);
    b = s2;
    a.swap(b);
    ASSERT_TRUE(a->i == s2);
    ASSERT_TRUE(b.error().i == s1);

    a = abc::unexpected<T2>(s1);
    b = abc::unexpected<T2>(s2);
    a.swap(b);
    ASSERT_TRUE(a.error().i == s2);
    ASSERT_TRUE(b.error().i == s1);
}

// #ifdef TL_EXPECTED_EXCEPTIONS_ENABLED
TEST(expected, tl_swap) {

    swap_test<no_throw, no_throw>();
    swap_test<no_throw, canthrow_move>();
    swap_test<canthrow_move, no_throw>();

    std::string s1 = "abcdefghijklmnopqrstuvwxyz";
    std::string s2 = "zyxwvutsrqponmlkjihgfedcbaxxx";
    abc::expected<no_throw, willthrow_move> a{s1};
    abc::expected<no_throw, willthrow_move> b{abc::unexpect, s2};
    should_throw = 1;

#ifdef _MSC_VER
    //this seems to break catch on GCC and Clang
    // REQUIRE_THROWS(swap(a, b));
    ASSERT_ANY_THROW(swap(a, b));
#endif

    ASSERT_TRUE(a->i == s1);
    ASSERT_TRUE(b.error().i == s2);
}
// #endif // TL_EXPECTED_EXCEPTIONS_ENABLED