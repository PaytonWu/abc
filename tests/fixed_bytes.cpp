// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/fixed_bytes.h"

#include <gtest/gtest.h>

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
    xbytes16_t bytes{ std::byte{0}, std::byte{1}, std::byte{2}, std::byte{3}, std::byte{4}, std::byte{5}, std::byte{6}, std::byte{7}, std::byte{8}, std::byte{9}, std::byte{10}, std::byte{11}, std::byte{12}, std::byte{13}, std::byte{14}, std::byte{15} };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[7], 7);
}

TEST(fixed_bytes, array_constructor) {
    std::array<xbyte_t, 8> const arr{ 1, 2, 3, 4, 5, 6, 7, 8 };
    xbytes16_t bytes{ arr };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 1);
    EXPECT_EQ(bytes[7], 8);

    std::array const arr2{ std::byte{1}, std::byte{2}, std::byte{3}, std::byte{4}, std::byte{5}, std::byte{6}, std::byte{7}, std::byte{8} };
    xbytes16_t bytes2{ arr2 };
    EXPECT_EQ(bytes2.size(), 16);
    EXPECT_EQ(bytes2[0], 1);
    EXPECT_EQ(bytes2[7], 8);
}

TEST(fixed_bytes, integral_constructor) {
    xbytes16_t bytes{ 0x0102030405060708ULL };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 1);
    EXPECT_EQ(bytes[7], 8);
}

TEST(fixed_bytes_le, integral_constructor) {
    xbytes16_le_t bytes{ 0x0102030405060708ULL };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 8);
    EXPECT_EQ(bytes[7], 1);

    xbytes4_le_t bytes2{ 0x0102030405060708U };
    EXPECT_EQ(bytes2.size(), 4);
    EXPECT_EQ(bytes2[0], 8);
    EXPECT_EQ(bytes2[3], 5);
}
