// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/converter.h>

#include <gtest/gtest.h>

TEST(converters, hex_string_to_fixed_bytes) {
    abc::bytes32_be_t key_data = abc::convert_to<abc::bytes32_be_t>::from(abc::hex_string::from("c67a31aca1e2bad8469003930c6d08f80f5087720d2276d3c85ad74d3297adec").value()).value();
    EXPECT_EQ(0xc6, key_data[0]);
    EXPECT_EQ(0x7a, key_data[1]);
    EXPECT_EQ(0x31, key_data[2]);
    EXPECT_EQ(0xec, key_data[31]);
    EXPECT_EQ(0xad, key_data[30]);
    EXPECT_EQ(0x97, key_data[29]);
}

TEST(converters, uint128_to_bytes) {
    abc::uint128_t value = abc::hex_string::from("0x1234567890abcdef1234567890abcdef").transform([](auto && hex_string) { return abc::uint128_t::from(hex_string); }).value();
    abc::bytes_be_t bytes = value.export_bits<abc::byte_numbering::msb0>();
    ASSERT_EQ(16, bytes.size());

    EXPECT_EQ(0x12, bytes[0]);
    EXPECT_EQ(0x34, bytes[1]);
    EXPECT_EQ(0x56, bytes[2]);
    EXPECT_EQ(0x78, bytes[3]);
    EXPECT_EQ(0x90, bytes[4]);
    EXPECT_EQ(0xab, bytes[5]);
    EXPECT_EQ(0xcd, bytes[6]);
    EXPECT_EQ(0xef, bytes[7]);
    EXPECT_EQ(0x12, bytes[8]);
    EXPECT_EQ(0x34, bytes[9]);
    EXPECT_EQ(0x56, bytes[10]);
    EXPECT_EQ(0x78, bytes[11]);
    EXPECT_EQ(0x90, bytes[12]);
    EXPECT_EQ(0xab, bytes[13]);
    EXPECT_EQ(0xcd, bytes[14]);
    EXPECT_EQ(0xef, bytes[15]);
}