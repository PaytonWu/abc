#include <gtest/gtest.h>

#include <abc/uint128.h>

TEST(Arithmetic, unary_plus){
    const abc::uint128_t value(0x12345ULL);
    EXPECT_EQ(+value, value);
}

TEST(Arithmetic, unary_minus){
    const abc::uint128_t val(1);
    const abc::uint128_t neg = -val;
    EXPECT_EQ(-val, neg);
    EXPECT_EQ(-neg, val);
    EXPECT_EQ(neg, abc::uint128_t(0xffffffffffffffffULL, 0xffffffffffffffffULL));
}
