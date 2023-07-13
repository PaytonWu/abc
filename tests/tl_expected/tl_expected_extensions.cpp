//// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
//// Distributed under the MIT License (http://opensource.org/licenses/MIT)
//
//#include "abc/expected.h"
//
//#include <gtest/gtest.h>
//
//#define TOKENPASTE(x, y) x##y
//#define TOKENPASTE2(x, y) TOKENPASTE(x, y)
//#undef STATIC_REQUIRE
//#define STATIC_REQUIRE(e)
//  constexpr bool TOKENPASTE2(rqure, __LINE__) = e;
//  (void)TOKENPASTE2(rqure, __LINE__);
//  ASSERT_TRUE(e);
//
//TEST(expected, map) {
//    auto mul2 = [](int a) { return a * 2; };
//    auto ret_void = [](int a) { (void) a; };
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.map(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.map(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).map(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).map(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.map(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.map(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).map(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).map(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.map(ret_void);
//        ASSERT_TRUE(ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.map(ret_void);
//        ASSERT_TRUE(ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).map(ret_void);
//        ASSERT_TRUE(ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).map(ret_void);
//        ASSERT_TRUE(ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.map(ret_void);
//        ASSERT_TRUE(!ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.map(ret_void);
//        ASSERT_TRUE(!ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).map(ret_void);
//        ASSERT_TRUE(!ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).map(ret_void);
//        ASSERT_TRUE(!ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//
//// mapping functions which return references
//    {
//        abc::expected<int, int> e(42);
//        auto ret = e.map([](int & i) -> int & { return i; });
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(ret == 42);
//    }
//}
//
//TEST(expected, map_error) {
//    auto mul2 = [](int a) { return a * 2; };
//    auto ret_void = [](int a) { (void) a; };
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.map_error(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.map_error(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).map_error(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).map_error(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.map_error(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 42);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.map_error(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 42);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).map_error(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 42);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).map_error(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 42);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.map_error(ret_void);
//        ASSERT_TRUE(ret);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.map_error(ret_void);
//        ASSERT_TRUE(ret);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).map_error(ret_void);
//        ASSERT_TRUE(ret);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).map_error(ret_void);
//        ASSERT_TRUE(ret);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.map_error(ret_void);
//        ASSERT_TRUE(!ret);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.map_error(ret_void);
//        ASSERT_TRUE(!ret);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).map_error(ret_void);
//        ASSERT_TRUE(!ret);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).map_error(ret_void);
//        ASSERT_TRUE(!ret);
//    }
//
//}
//
//TEST(expected, and_then) {
//    auto succeed = [](int a) {
//        (void) a;
//        return abc::expected<int, int>(21 * 2);
//    };
//    auto fail = [](int a) {
//        (void) a;
//        return abc::expected<int, int>(abc::unexpect, 17);
//    };
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.and_then(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.and_then(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).and_then(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).and_then(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.and_then(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 17);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.and_then(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 17);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).and_then(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 17);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).and_then(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 17);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.and_then(succeed);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.and_then(succeed);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).and_then(succeed);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).and_then(succeed);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.and_then(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.and_then(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).and_then(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).and_then(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//}
//
//TEST(expected, or_else) {
//    using eptr = std::unique_ptr<int>;
//    auto succeed = [](int a) {
//        (void) a;
//        return abc::expected<int, int>(21 * 2);
//    };
//    auto succeedptr = [](eptr e) {
//        (void) e;
//        return abc::expected<int, eptr>(21 * 2);
//    };
//    auto fail = [](int a) {
//        (void) a;
//        return abc::expected<int, int>(abc::unexpect, 17);
//    };
//    auto failptr = [](eptr e) {
//        *e = 17;
//        return abc::expected<int, eptr>(abc::unexpect, std::move(e));
//    };
//    auto failvoid = [](int) {};
//    auto failvoidptr = [](const eptr &) { /* don't consume */};
//    auto consumeptr = [](eptr) {};
//    auto make_u_int = [](int n) { return std::unique_ptr<int>(new int(n)); };
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.or_else(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.or_else(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).or_else(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        abc::expected<int, eptr> e = 21;
//        auto ret = std::move(e).or_else(succeedptr);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).or_else(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.or_else(fail);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.or_else(fail);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).or_else(fail);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(ret == 21);
//    }
//
//
//    {
//        abc::expected<int, eptr> e = 21;
//        auto ret = std::move(e).or_else(failptr);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(ret == 21);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).or_else(fail);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.or_else(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.or_else(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).or_else(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        abc::expected<int, eptr> e(abc::unexpect, make_u_int(21));
//        auto ret = std::move(e).or_else(succeedptr);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).or_else(succeed);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.or_else(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 17);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.or_else(failvoid);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.or_else(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 17);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.or_else(failvoid);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).or_else(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 17);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).or_else(failvoid);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, eptr> e(abc::unexpect, make_u_int(21));
//        auto ret = std::move(e).or_else(failvoidptr);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(*ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, eptr> e(abc::unexpect, make_u_int(21));
//        auto ret = std::move(e).or_else(consumeptr);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == nullptr);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).or_else(fail);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 17);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).or_else(failvoid);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//}
//
//TEST(expected, tronsform) {
//    auto mul2 = [](int a) { return a * 2; };
//    auto ret_void = [](int a) { (void) a; };
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.transform(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.transform(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).transform(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).transform(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 42);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.transform(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.transform(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).transform(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).transform(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 21);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.transform(ret_void);
//        ASSERT_TRUE(ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.transform(ret_void);
//        ASSERT_TRUE(ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).transform(ret_void);
//        ASSERT_TRUE(ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).transform(ret_void);
//        ASSERT_TRUE(ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.transform(ret_void);
//        ASSERT_TRUE(!ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.transform(ret_void);
//        ASSERT_TRUE(!ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).transform(ret_void);
//        ASSERT_TRUE(!ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).transform(ret_void);
//        ASSERT_TRUE(!ret);
//        STATIC_REQUIRE(
//            (std::is_same<decltype(ret), abc::expected<void, int>>::value));
//    }
//
//
//// mapping functions which return references
//    {
//        abc::expected<int, int> e(42);
//        auto ret = e.transform([](int & i) -> int & { return i; });
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(ret == 42);
//    }
//}
//
//TEST(expected, transform_error) {
//    auto mul2 = [](int a) { return a * 2; };
//    auto ret_void = [](int a) { (void) a; };
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.transform_error(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.transform_error(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).transform_error(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).transform_error(mul2);
//        ASSERT_TRUE(ret);
//        ASSERT_TRUE(*ret == 21);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.transform_error(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 42);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.transform_error(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 42);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).transform_error(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 42);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).transform_error(mul2);
//        ASSERT_TRUE(!ret);
//        ASSERT_TRUE(ret.error() == 42);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = e.transform_error(ret_void);
//        ASSERT_TRUE(ret);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = e.transform_error(ret_void);
//        ASSERT_TRUE(ret);
//    }
//
//    {
//        abc::expected<int, int> e = 21;
//        auto ret = std::move(e).transform_error(ret_void);
//        ASSERT_TRUE(ret);
//    }
//
//    {
//        const abc::expected<int, int> e = 21;
//        auto ret = std::move(e).transform_error(ret_void);
//        ASSERT_TRUE(ret);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.transform_error(ret_void);
//        ASSERT_TRUE(!ret);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = e.transform_error(ret_void);
//        ASSERT_TRUE(!ret);
//    }
//
//    {
//        abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).transform_error(ret_void);
//        ASSERT_TRUE(!ret);
//    }
//
//    {
//        const abc::expected<int, int> e(abc::unexpect, 21);
//        auto ret = std::move(e).transform_error(ret_void);
//        ASSERT_TRUE(!ret);
//    }
//
//}
//
//struct S {
//    int x;
//};
//
//struct F {
//    int x;
//};
//
//TEST(expected, issue_14) {
//    auto res = abc::expected<S, F>{abc::unexpect, F{}};
//
//    res.map_error([](
//        F f
//    ) {
//        (void)
//            f;
//    });
//}
//
//TEST(expected, issue_32) {
//    int i = 0;
//    abc::expected<void, int> a;
//    a.map([&i] {
//        i = 42;
//    });
//    ASSERT_TRUE(i == 42);
//
//    auto x = a.map([] { return 42; });
//    ASSERT_TRUE(*x == 42);
//}
