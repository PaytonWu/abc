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
    xbytes16_t bytes{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
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
}

TEST(fixed_bytes, from) {
    xbytes16_t bytes = xbytes16_t::from(0x0102030405060708U);
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[7], 0);
    EXPECT_EQ(bytes[8], 1);
    EXPECT_EQ(bytes[15], 8);
}

TEST(fixed_bytes_le, from) {
    xbytes16_le_t bytes = xbytes16_le_t::from(0x0102030405060708U);
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 8);
    EXPECT_EQ(bytes[7], 1);

    xbytes4_le_t bytes2 = xbytes4_le_t::from(0x0102030405060708U);
    EXPECT_EQ(bytes2.size(), 4);
    EXPECT_EQ(bytes2[0], 8);
    EXPECT_EQ(bytes2[3], 5);
}

TEST(fixed_bytes_be, from) {
    xbytes16_be_t bytes = xbytes16_be_t::from(0x0102030405060708U);
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[7], 0);
    EXPECT_EQ(bytes[8], 1);
    EXPECT_EQ(bytes[15], 8);

    xbytes4_be_t bytes2 = xbytes4_be_t::from(0x0102030405060708U);
    EXPECT_EQ(bytes2.size(), 4);
    EXPECT_EQ(bytes2[0], 5);
    EXPECT_EQ(bytes2[3], 8);
}
