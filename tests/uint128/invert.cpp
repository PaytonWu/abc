#include <gtest/gtest.h>

#include <abc/uint128.h>

TEST(BitWise, invert){
    EXPECT_EQ(~abc::uint128_t(0x0000000000000000ULL, 0x0000000000000000ULL), abc::uint128_t(0xffffffffffffffffULL, 0xffffffffffffffffULL));
    EXPECT_EQ(~abc::uint128_t(0x0000000000000000ULL, 0xffffffffffffffffULL), abc::uint128_t(0xffffffffffffffffULL, 0x0000000000000000ULL));
    EXPECT_EQ(~abc::uint128_t(0xffffffffffffffffULL, 0xffffffffffffffffULL), abc::uint128_t(0x0000000000000000ULL, 0x0000000000000000ULL));
}
