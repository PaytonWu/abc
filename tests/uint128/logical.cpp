#include <gtest/gtest.h>

#include <abc/uint128.h>

TEST(Logical, and){
    const abc::uint128_t A(0xffffffff);
    const abc::uint128_t B(0x00000000);

    EXPECT_EQ(A && A, true);
    EXPECT_EQ(A && B, false);
}

TEST(Logical, or){
    const abc::uint128_t A(0xffffffff);
    const abc::uint128_t B(0x00000000);

    EXPECT_EQ(A || A, true);
    EXPECT_EQ(A || B, true);
}

TEST(Logical, not){
    EXPECT_EQ(!abc::uint128_t(0xffffffff), 0);
}
