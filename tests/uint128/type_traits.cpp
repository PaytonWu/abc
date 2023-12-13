#include <type_traits>

#include <gtest/gtest.h>

#include <abc/uint128.h>

TEST(Type_Traits, is_arithmetic)
{
    EXPECT_FALSE(std::is_arithmetic_v<abc::uint128_t>);
}

TEST(Type_Traits, is_integral)
{
    EXPECT_FALSE(std::is_integral_v<abc::uint128_t>);
}

TEST(Type_Traits, is_unsigned)
{
    EXPECT_FALSE(std::is_unsigned_v<abc::uint128_t>);
}

TEST(Type_Traits, trivally_copyable)
{
    EXPECT_TRUE(std::is_trivially_copyable_v<abc::uint128_t>);
}
