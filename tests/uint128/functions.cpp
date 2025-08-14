#include <abc/bytes.h>
#include <abc/uint128.h>

#include <gtest/gtest.h>

#include <map>

static std::map<uint8_t, std::string> const tests = {
    std::make_pair(static_cast<uint8_t>(2), "10000100000101011000010101101100"),
    std::make_pair(static_cast<uint8_t>(3), "12201102210121112101"),
    std::make_pair(static_cast<uint8_t>(4), "2010011120111230"),
    std::make_pair(static_cast<uint8_t>(5), "14014244043144"),
    std::make_pair(static_cast<uint8_t>(6), "1003520344444"),
    std::make_pair(static_cast<uint8_t>(7), "105625466632"),
    std::make_pair(static_cast<uint8_t>(8), "20405302554"),
    std::make_pair(static_cast<uint8_t>(9), "5642717471"),
    std::make_pair(static_cast<uint8_t>(10), "2216002924"),
    std::make_pair(static_cast<uint8_t>(11), "a3796a883"),
    std::make_pair(static_cast<uint8_t>(12), "51a175124"),
    std::make_pair(static_cast<uint8_t>(13), "294145645"),
    std::make_pair(static_cast<uint8_t>(14), "170445352"),
    std::make_pair(static_cast<uint8_t>(15), "ce82d6d4"),
    std::make_pair(static_cast<uint8_t>(16), "8415856c"),
    // std::make_pair(256, "abc::uint128_t"),
};

TEST(Function, str)
{
    // number of leading 0s
    std::string::size_type const leading = 5;

    // make sure all of the test strings create the ASCII version of the string
    abc::uint128_t const original(2216002924);
    for (std::pair<uint8_t const, std::string> t : tests)
    {
        EXPECT_EQ(original.str(t.first), t.second);
    }

    // add leading zeros
    for (uint8_t base = 2; base <= 16; base++)
    {
        EXPECT_EQ(original.str(base, static_cast<uint32_t>(tests.at(base).size() + leading)), std::string(leading, '0') + tests.at(base));
    }
}

TEST(Function, export_bits)
{
    constexpr uint64_t u64 = 0x0123456789abcdefULL;
    abc::uint128_t const value = u64;

    EXPECT_EQ(value, u64);

    abc::BigEndianBytes const full{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };

    abc::BigEndianBytes bits;
    value.export_bits(bits);
    EXPECT_EQ(bits, full);

    auto const bits2 = value.export_bits<abc::ByteNumbering::Msb0>();
    EXPECT_EQ(bits2, full);
}

TEST(Function, export_bits_compact)
{
    constexpr uint64_t u64 = 0x0123456789abcdefULL;
    abc::uint128_t const value = u64;

    EXPECT_EQ(value, u64);

    abc::BigEndianBytes const compact{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef };

    abc::BigEndianBytes bits;
    value.export_bits_compact(bits);
    EXPECT_EQ(bits, compact);

    auto const bits2 = value.export_bits_compact<abc::ByteNumbering::Msb0>();
    EXPECT_EQ(bits2, compact);
}

TEST(Function, export_bits_compact_zero)
{
    constexpr uint64_t u64 = 0;
    abc::uint128_t const value = u64;

    EXPECT_EQ(value, u64);

    abc::BigEndianBytes const compact{};

    auto const bits = value.export_bits_compact<abc::ByteNumbering::Msb0>();
    EXPECT_EQ(bits, compact);
}

TEST(Function, export_bits_compact_little)
{
    constexpr uint64_t u64 = 0x0123456789abcdefULL;
    abc::uint128_t const value = u64;

    EXPECT_EQ(value, u64);

    abc::LittleEndianBytes const compact{ 0xef, 0xcd, 0xab, 0x89, 0x67, 0x45, 0x23, 0x01 };

    auto const bits = value.export_bits_compact<abc::ByteNumbering::Lsb0>();
    EXPECT_EQ(bits, compact);

    abc::LittleEndianBytes bits3;
    bits3.reserve(32);
    value.export_bits_compact(bits3);
    EXPECT_EQ(bits3, compact);
}

TEST(Function, export_bits_compact_zero_little)
{
    constexpr uint64_t u64 = 0;
    abc::uint128_t const value = u64;

    EXPECT_EQ(value, u64);

    abc::LittleEndianBytes const compact{};

    auto const bits = value.export_bits_compact<abc::ByteNumbering::Lsb0>();
    EXPECT_EQ(bits, compact);

    abc::LittleEndianBytes bits3;
    bits3.reserve(32);
    value.export_bits_compact(bits3);
    EXPECT_EQ(bits3, compact);
}

TEST(External, ostream)
{
    abc::uint128_t const value(0xfedcba9876543210ULL);

    // write out octal abc::uint128_t
    std::stringstream oct;
    oct << std::oct << value;
    EXPECT_EQ(oct.str(), "1773345651416625031020");

    // write out decimal abc::uint128_t
    std::stringstream dec;
    dec << std::dec << value;
    EXPECT_EQ(dec.str(), "18364758544493064720");

    // write out hexadecimal abc::uint128_t
    std::stringstream hex;
    hex << std::hex << value;
    EXPECT_EQ(hex.str(), "fedcba9876543210");

    // zero
    std::stringstream zero;
    zero << abc::uint128_t();
    EXPECT_EQ(zero.str(), "0");
}
