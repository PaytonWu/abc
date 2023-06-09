// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/bytes.h"

#include <gtest/gtest.h>

#include <ranges>

using namespace abc;

TEST(xbytes_endian_t, constructor_il) {
    {
        bytes_msb0_t bytes{ 0x01, 0x02, 0x03, 0x04 };
        EXPECT_EQ(bytes[0], 0x01);
        EXPECT_EQ(bytes[1], 0x02);
        EXPECT_EQ(bytes[2], 0x03);
        EXPECT_EQ(bytes[3], 0x04);
    }

    {
        bytes_msb0_t bytes{ 0x01, 0x02, 0x03, 0x04 };
        bytes_msb0_t bytes2{ bytes };
        EXPECT_EQ(bytes2[0], 0x01);
        EXPECT_EQ(bytes2[1], 0x02);
        EXPECT_EQ(bytes2[2], 0x03);
        EXPECT_EQ(bytes2[3], 0x04);
    }

    {
        bytes_msb0_t bytes{ 0x01, 0x02, 0x03, 0x04 };
        bytes_msb0_t bytes2{ std::move(bytes) };
        EXPECT_EQ(bytes2[0], 0x01);
        EXPECT_EQ(bytes2[1], 0x02);
        EXPECT_EQ(bytes2[2], 0x03);
        EXPECT_EQ(bytes2[3], 0x04);
    }

    {
        bytes_lsb0_t bytes{ 0x01, 0x02, 0x03, 0x04 };
        EXPECT_EQ(bytes[0], 0x01);
        EXPECT_EQ(bytes[1], 0x02);
        EXPECT_EQ(bytes[2], 0x03);
        EXPECT_EQ(bytes[3], 0x04);
    }

    {
        bytes_lsb0_t bytes{ 0x01, 0x02, 0x03, 0x04 };
        bytes_lsb0_t bytes2{ bytes };
        EXPECT_EQ(bytes2[0], 0x01);
        EXPECT_EQ(bytes2[1], 0x02);
        EXPECT_EQ(bytes2[2], 0x03);
        EXPECT_EQ(bytes2[3], 0x04);
    }
    {
        bytes_lsb0_t bytes{ 0x01, 0x02, 0x03, 0x04 };
        bytes_lsb0_t bytes2{ std::move(bytes) };
        EXPECT_EQ(bytes2[0], 0x01);
        EXPECT_EQ(bytes2[1], 0x02);
        EXPECT_EQ(bytes2[2], 0x03);
        EXPECT_EQ(bytes2[3], 0x04);
    }
}

TEST(xbytes_endian_t, construct_uint) {
    {
        bytes_msb0_t bytes = bytes_msb0_t::from(0x0102u);
        EXPECT_EQ(bytes[0], 0x01);
        EXPECT_EQ(bytes[1], 0x02);
        EXPECT_EQ(bytes.size(), 2);
    }
    {
        bytes_msb0_t bytes = bytes_msb0_t::from(0x010203u);
        EXPECT_EQ(bytes[0], 0x01);
        EXPECT_EQ(bytes[1], 0x02);
        EXPECT_EQ(bytes[2], 0x03);
        EXPECT_EQ(bytes.size(), 3);
    }
    {
        bytes_msb0_t bytes = bytes_msb0_t::from(0x01020304u);
        EXPECT_EQ(bytes[0], 0x01);
        EXPECT_EQ(bytes[1], 0x02);
        EXPECT_EQ(bytes[2], 0x03);
        EXPECT_EQ(bytes[3], 0x04);
        EXPECT_EQ(bytes.size(), 4);
    }
    {
        bytes_lsb0_t bytes = bytes_lsb0_t::from(0x01u);
        EXPECT_EQ(bytes[0], 0x01);
        EXPECT_EQ(bytes.size(), 1);
    }
    {
        bytes_lsb0_t bytes = bytes_lsb0_t::from(0x0102u);
        EXPECT_EQ(bytes[0], 0x02);
        EXPECT_EQ(bytes[1], 0x01);
        EXPECT_EQ(bytes.size(), 2);
    }
    {
        bytes_lsb0_t bytes = bytes_lsb0_t::from(0x010203u);
        EXPECT_EQ(bytes[0], 0x03);
        EXPECT_EQ(bytes[1], 0x02);
        EXPECT_EQ(bytes[2], 0x01);
        EXPECT_EQ(bytes.size(), 3);
    }
    {
        bytes_lsb0_t bytes = bytes_lsb0_t::from(0x01020304u);
        EXPECT_EQ(bytes[0], 0x04);
        EXPECT_EQ(bytes[1], 0x03);
        EXPECT_EQ(bytes[2], 0x02);
        EXPECT_EQ(bytes[3], 0x01);
        EXPECT_EQ(bytes.size(), 4);
    }
    {
        bytes_msb0_t bytes = bytes_msb0_t::from(0x0102030405060708u);
        EXPECT_EQ(bytes[0], 0x01);
        EXPECT_EQ(bytes[1], 0x02);
        EXPECT_EQ(bytes[2], 0x03);
        EXPECT_EQ(bytes[3], 0x04);
        EXPECT_EQ(bytes[4], 0x05);
        EXPECT_EQ(bytes[5], 0x06);
        EXPECT_EQ(bytes[6], 0x07);
        EXPECT_EQ(bytes[7], 0x08);
        EXPECT_EQ(bytes.size(), 8);
    }
    {
        bytes_lsb0_t bytes = bytes_lsb0_t::from(0x0102030405060708u);
        EXPECT_EQ(bytes[0], 0x08);
        EXPECT_EQ(bytes[1], 0x07);
        EXPECT_EQ(bytes[2], 0x06);
        EXPECT_EQ(bytes[3], 0x05);
        EXPECT_EQ(bytes[4], 0x04);
        EXPECT_EQ(bytes[5], 0x03);
        EXPECT_EQ(bytes[6], 0x02);
        EXPECT_EQ(bytes[7], 0x01);
        EXPECT_EQ(bytes.size(), 8);
    }
}
