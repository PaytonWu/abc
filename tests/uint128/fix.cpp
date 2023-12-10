#include <gtest/gtest.h>

#include <abc/uint128.h>

TEST(Arithmetic, increment){
    abc::uint128_t value(0);
    EXPECT_EQ(++value, 1);
    EXPECT_EQ(value++, 1);
    EXPECT_EQ(++value, 3);
}

TEST(Arithmetic, decrement){
    abc::uint128_t value(0);
    EXPECT_EQ(--value, abc::uint128_t(0xffffffffffffffffULL, 0xffffffffffffffffULL));
    EXPECT_EQ(value--, abc::uint128_t(0xffffffffffffffffULL, 0xffffffffffffffffULL));
    EXPECT_EQ(--value, abc::uint128_t(0xffffffffffffffffULL, 0xfffffffffffffffdULL));
}
