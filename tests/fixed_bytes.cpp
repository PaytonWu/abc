// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/fixed_bytes.h"

#include <gtest/gtest.h>

#include <ranges>

using namespace abc;

TEST(fixed_bytes, default_constructor) {
    xbytes16_t bytes;
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[15], 0);
}

TEST(fixed_bytes, copy_constructor) {
    xbytes16_t bytes;
    bytes[0] = 1;
    bytes[15] = 2;
    xbytes16_t bytes2{ bytes };
    EXPECT_EQ(bytes2.size(), 16);
    EXPECT_EQ(bytes2[0], 1);
    EXPECT_EQ(bytes2[15], 2);
}

TEST(fixed_bytes, move_constructor) {
    xbytes16_t bytes;
    bytes[0] = 1;
    bytes[15] = 2;
    xbytes16_t bytes2{ std::move(bytes) };
    EXPECT_EQ(bytes2.size(), 16);
    EXPECT_EQ(bytes2[0], 1);
    EXPECT_EQ(bytes2[15], 2);
}

TEST(fixed_bytes, initializer_list_constructor) {
    xbytes16_t bytes{ std::array<xbyte_t, 16>{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f } };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[7], 7);
}

TEST(fixed_bytes, array_constructor) {
    std::array<xbyte_t, 16> const arr{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    xbytes16_t bytes{ arr };
    EXPECT_EQ(bytes.size(), 16);
    for (auto i : std::views::iota(1u, 16u)) {
        ASSERT_EQ(bytes[i - 1], i);
    }
}

TEST(fixed_bytes, from) {
    xbytes16_t bytes{ 0x0102030405060708U };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[7], 0);
    EXPECT_EQ(bytes[8], 1);
    EXPECT_EQ(bytes[15], 8);
}

TEST(fixed_bytes_le, from) {
    xbytes16_le_t bytes{ 0x0102030405060708U };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 8);
    EXPECT_EQ(bytes[7], 1);

    xbytes4_le_t bytes2{ 0x0102030405060708U };
    EXPECT_EQ(bytes2.size(), 4);
    EXPECT_EQ(bytes2[0], 8);
    EXPECT_EQ(bytes2[3], 5);
}

TEST(fixed_bytes_be, from) {
    xbytes16_be_t bytes{ 0x0102030405060708U };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[7], 0);
    EXPECT_EQ(bytes[8], 1);
    EXPECT_EQ(bytes[15], 8);

    xbytes4_be_t bytes2{ 0x0102030405060708U };
    EXPECT_EQ(bytes2.size(), 4);
    EXPECT_EQ(bytes2[0], 5);
    EXPECT_EQ(bytes2[3], 8);
}

TEST(fixed_bytes_le, to) {
    xbytes16_le_t bytes{ std::array<xbyte_t, 16>{ 8, 7, 6, 5, 4, 3, 2, 1 } };
    EXPECT_EQ(bytes.to<uint64_t>(), 0x0102030405060708U);
    EXPECT_EQ(bytes.to<uint32_t>(), 0x05060708U);
    EXPECT_EQ(bytes.to<uint16_t>(), 0x0708U);
    EXPECT_EQ(bytes.to<uint8_t>(), 0x08U);
}

TEST(fixed_bytes_be, to) {
    xbytes16_be_t bytes{ std::array<xbyte_t, 16>{ 1, 2, 3, 4, 5, 6, 7, 8 } };  // 0x01020304050607080000000000000000
    EXPECT_EQ(bytes.to<uint64_t>(), 0x00);
    EXPECT_EQ(bytes.to<uint32_t>(), 0x00);
    EXPECT_EQ(bytes.to<uint16_t>(), 0x00);
    EXPECT_EQ(bytes.to<uint8_t>(), 0x00);
}

TEST(fixed_bytes_be, zero) {
    xbytes32_be_t bytes;
    EXPECT_TRUE(bytes.zero());

    bytes[0] = 1;
    EXPECT_FALSE(bytes.zero());
}

TEST(fixed_bytes_le, zero) {
    xbytes32_le_t bytes;
    EXPECT_TRUE(bytes.zero());

    bytes[0] = 1;
    EXPECT_FALSE(bytes.zero());
}

TEST(fixed_bytes_le, prefix_plusplus) {
    xfixed_bytes_t<2, std::endian::little> bytes;
    for (uint16_t i : std::views::iota(0u, std::numeric_limits<uint16_t>::max())) {
        ASSERT_EQ(i, bytes.to<uint16_t>());
        ++bytes;
    }
}

TEST(fixed_bytes_be, prefix_plusplus) {
    xfixed_bytes_t<2, std::endian::big> bytes;
    for (uint16_t i : std::views::iota(0u, std::numeric_limits<uint16_t>::max())) {
        ASSERT_EQ(i, bytes.to<uint16_t>());
        ++bytes;
    }
}

TEST(fixed_bytes_be, hex_string) {
    auto bytes = xbytes16_be_t{0x0123456789abcdefu};
    auto const & hex_str = bytes.hex_string();
    EXPECT_EQ("0x00000000000000000123456789abcdef", hex_str.to_string());
}

TEST(fixed_bytes_le, hex_string) {
    auto bytes = xbytes16_le_t{0x0123456789abcdefu};
    auto const & hex_str = bytes.hex_string();
    EXPECT_EQ("0x00000000000000000123456789abcdef", hex_str.to_string());
}