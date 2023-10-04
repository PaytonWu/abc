// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/bytes.h>
#include <abc/hex_string.h>

#include <gtest/gtest.h>

#include <ranges>

using namespace abc;

TEST(xbytes_endian_t, constructor_il) {
    {
        bytes_msb0_t bytes = bytes_be_t::from<byte_numbering::msb0>({ 0x01, 0x02, 0x03, 0x04 });
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

TEST(bytes_endian_t, from_negtive_int) {
    bytes_lsb0_t bytes = bytes_lsb0_t::from(-1);
    EXPECT_EQ(bytes[0], 0xff);
    EXPECT_EQ(bytes[1], 0xff);
    EXPECT_EQ(bytes[2], 0xff);
    EXPECT_EQ(bytes[3], 0xff);
    EXPECT_EQ(bytes.size(), 4);
}

TEST(bytes, operator_plus_bytes) {
    bytes_be_t lhs = bytes_be_t::from(0x1234567890);
    bytes_be_t rhs = bytes_be_t::from(0x0987654321);

    lhs = lhs + rhs;
    ASSERT_EQ(abc::hex_string::from("0x12345678900987654321").transform([](auto const & hex_string) { return hex_string.template bytes<abc::byte_numbering::msb0>(); }).value(), lhs);
}

TEST(bytes, operator_plus_byte_after) {
    bytes_be_t lhs = bytes_be_t::from(0x1234567890);

    lhs = lhs + static_cast<abc::byte>(0);
    ASSERT_EQ(abc::hex_string::from("0x123456789000").transform([](auto const & hex_string) { return hex_string.template bytes<abc::byte_numbering::msb0>(); }).value(), lhs);
}

TEST(bytes, operator_plus_byte_before) {
    bytes_be_t lhs = bytes_be_t::from(0x1234567890);

    lhs = static_cast<abc::byte>(1) + lhs;
    ASSERT_EQ(abc::hex_string::from("0x11234567890").transform([](auto const & hex_string) { return hex_string.template bytes<abc::byte_numbering::msb0>(); }).value(), lhs);
}
