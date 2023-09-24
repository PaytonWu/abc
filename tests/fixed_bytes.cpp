// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/converter.h"
#include "abc/fixed_bytes.h"

#include <gtest/gtest.h>

#include <ranges>

using namespace abc;

TEST(fixed_bytes, default_constructor) {
    bytes16_msb0_t bytes;
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[15], 0);
}

TEST(fixed_bytes, copy_constructor) {
    bytes16_msb0_t bytes;
    bytes[0] = 1;
    bytes[15] = 2;
    bytes16_msb0_t bytes2{ bytes };
    EXPECT_EQ(bytes2.size(), 16);
    EXPECT_EQ(bytes2[0], 1);
    EXPECT_EQ(bytes2[15], 2);
}

TEST(fixed_bytes, copy_constructor2) {
    bytes16_msb0_t bytes;
    bytes[0] = 1;
    bytes[15] = 2;
    bytes16_lsb0_t bytes2{ bytes };
    EXPECT_EQ(bytes2.size(), 16);
    EXPECT_EQ(bytes2[15], 1);
    EXPECT_EQ(bytes2[0], 2);
}

TEST(fixed_bytes, move_constructor) {
    bytes16_msb0_t bytes;
    bytes[0] = 1;
    bytes[15] = 2;
    bytes16_msb0_t bytes2{ std::move(bytes) };
    EXPECT_EQ(bytes2.size(), 16);
    EXPECT_EQ(bytes2[0], 1);
    EXPECT_EQ(bytes2[15], 2);
}

TEST(fixed_bytes_msb0, from_msb0) {
    bytes16_msb0_t bytes{ bytes16_msb0_t::from<byte_numbering::msb0>(std::array<byte, 16>{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f }).value() };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[7], 7);
}

TEST(fixed_bytes_msb0, from_lsb0) {
    bytes16_msb0_t bytes{ bytes16_msb0_t::from<byte_numbering::lsb0>(std::array<byte, 16>{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f }).value() };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0x0f);
    EXPECT_EQ(bytes[7], 0x08);
}

TEST(fixed_bytes, array_constructor) {
    std::array<byte, 16> const arr{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    bytes16_msb0_t bytes = bytes16_msb0_t::from<byte_numbering::msb0>(arr).value();
    EXPECT_EQ(bytes.size(), 16);
    for (auto i : std::views::iota(1u, 16u)) {
        ASSERT_EQ(bytes[i - 1], i);
    }
}

TEST(fixed_bytes, int_constructor) {
    bytes16_msb0_t bytes{ 0x0102030405060708U };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[7], 0);
    EXPECT_EQ(bytes[8], 1);
    EXPECT_EQ(bytes[15], 8);
}

TEST(fixed_bytes_le, from) {
    bytes16_lsb0_t bytes{ 0x0102030405060708U };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 8);
    EXPECT_EQ(bytes[7], 1);

    bytes4_lsb0_t bytes2{ 0x0102030405060708U };
    EXPECT_EQ(bytes2.size(), 4);
    EXPECT_EQ(bytes2[0], 8);
    EXPECT_EQ(bytes2[3], 5);
}

TEST(fixed_bytes_be, from) {
    bytes16_msb0_t bytes{ 0x0102030405060708U };
    EXPECT_EQ(bytes.size(), 16);
    EXPECT_EQ(bytes[0], 0);
    EXPECT_EQ(bytes[7], 0);
    EXPECT_EQ(bytes[8], 1);
    EXPECT_EQ(bytes[15], 8);

    bytes4_msb0_t bytes2{ 0x0102030405060708U };
    EXPECT_EQ(bytes2.size(), 4);
    EXPECT_EQ(bytes2[0], 5);
    EXPECT_EQ(bytes2[3], 8);
}

TEST(fixed_bytes_le, to) {
    bytes16_lsb0_t bytes{ bytes16_lsb0_t::from<byte_numbering::lsb0>(std::array<byte, 16>{ 8, 7, 6, 5, 4, 3, 2, 1 }).value() };
    EXPECT_EQ(bytes.to<uint64_t>(), 0x0102030405060708U);
    EXPECT_EQ(bytes.to<uint32_t>(), 0x05060708U);
    EXPECT_EQ(bytes.to<uint16_t>(), 0x0708U);
    EXPECT_EQ(bytes.to<uint8_t>(), 0x08U);
}

TEST(fixed_bytes_be, to) {
    bytes16_msb0_t bytes{ bytes16_msb0_t::from<byte_numbering::msb0>(std::array<byte, 16>{ 1, 2, 3, 4, 5, 6, 7, 8 }).value() };  // 0x01020304050607080000000000000000
    EXPECT_EQ(bytes.to<uint64_t>(), 0x00);
    EXPECT_EQ(bytes.to<uint32_t>(), 0x00);
    EXPECT_EQ(bytes.to<uint16_t>(), 0x00);
    EXPECT_EQ(bytes.to<uint8_t>(), 0x00);
}

TEST(fixed_bytes_be, zero) {
    bytes32_msb0_t bytes;
    EXPECT_TRUE(bytes.zero());

    bytes[0] = 1;
    EXPECT_FALSE(bytes.zero());
}

TEST(fixed_bytes_le, zero) {
    bytes32_lsb0_t bytes;
    EXPECT_TRUE(bytes.zero());

    bytes[0] = 1;
    EXPECT_FALSE(bytes.zero());
}

TEST(fixed_bytes_le, prefix_plusplus) {
    fixed_bytes<2, byte_numbering::lsb0> bytes;
    for (uint16_t i : std::views::iota(0u, std::numeric_limits<uint16_t>::max())) {
        ASSERT_EQ(i, bytes.to<uint16_t>());
        ++bytes;
    }
}

TEST(fixed_bytes_be, prefix_plusplus) {
    fixed_bytes<2, byte_numbering::msb0> bytes;
    for (uint16_t i : std::views::iota(0u, std::numeric_limits<uint16_t>::max())) {
        ASSERT_EQ(i, bytes.to<uint16_t>());
        ++bytes;
    }
}

TEST(fixed_bytes_be, hex_string) {
    auto bytes = bytes16_msb0_t{0x0123456789abcdefu};
    auto const hex_str = convert_to<hex_string>::from(bytes).value();
    EXPECT_EQ("0x00000000000000000123456789abcdef", hex_str.to_string());
}

TEST(fixed_bytes_le, hex_string) {
    auto bytes = bytes16_lsb0_t{0x0123456789abcdefu};
    auto const hex_str = convert_to<hex_string>::from(bytes).value();
    EXPECT_EQ("0x00000000000000000123456789abcdef", hex_str.to_string());
}

TEST(fixed_bytes_none, from) {
    auto bytes = hex_string::from("0x1234567890").transform([](auto && hexstr) { return hexstr.template bytes<byte_numbering::msb0>(); }).value();
    auto result = fixed_bytes<5, byte_numbering::none>::from<byte_numbering::none>(bytes);
    EXPECT_TRUE(result.has_value());
    auto const & fixed_bytes = result.value();
    EXPECT_EQ(static_cast<byte>(0x12), fixed_bytes[0]);
    EXPECT_EQ(static_cast<byte>(0x34), fixed_bytes[1]);
    EXPECT_EQ(static_cast<byte>(0x56), fixed_bytes[2]);
    EXPECT_EQ(static_cast<byte>(0x78), fixed_bytes[3]);
    EXPECT_EQ(static_cast<byte>(0x90), fixed_bytes[4]);
}

TEST(fixed_bytes, subbytes) {
    auto bytes15 = hex_string::from("0x123456789012345678901234567890").transform([](auto && hex) { return hex.template bytes<byte_numbering::msb0>(); })
                                                                       .and_then([](auto && bytes) { return fixed_bytes<15, byte_numbering::msb0>::from<byte_numbering::msb0>(bytes); });
    ASSERT_TRUE(bytes15.has_value());
    auto bytes8 = bytes15.and_then([](auto && bytes) { return bytes.subbytes(7); }).value();
    // 5678901234567890
    EXPECT_EQ(0x56, bytes8[0]);
    EXPECT_EQ(0x78, bytes8[1]);
    EXPECT_EQ(0x90, bytes8[2]);
    EXPECT_EQ(0x12, bytes8[3]);
    EXPECT_EQ(0x34, bytes8[4]);
    EXPECT_EQ(0x56, bytes8[5]);
    EXPECT_EQ(0x78, bytes8[6]);
    EXPECT_EQ(0x90, bytes8[7]);
}

TEST(fixed_bytes, subbytes_error) {
    auto bytes15 = hex_string::from("0x123456789012345678901234567890").transform([](auto && hex) { return hex.template bytes<byte_numbering::msb0>(); })
                                                                       .and_then([](auto && bytes) { return fixed_bytes<15, byte_numbering::msb0>::from<byte_numbering::msb0>(bytes); });
    ASSERT_TRUE(bytes15.has_value());
    auto bytes8 = bytes15.and_then([](auto && bytes) { return bytes.subbytes(15); });
    ASSERT_TRUE(bytes8.is_err());
}

TEST(fixed_bytes, be_from_be) {
    auto bytes32 = hex_string::from("0x123456789012345678901234567890").transform([](auto && hex) { return hex.template bytes<byte_numbering::msb0>(); })
                                                                       .and_then([](auto && bytes) { return bytes32_be_t::from<byte_numbering::msb0>(bytes); })
                                                                       .value();
    // 5678901234567890
    EXPECT_EQ(0x56, bytes32[24]);
    EXPECT_EQ(0x78, bytes32[25]);
    EXPECT_EQ(0x90, bytes32[26]);
    EXPECT_EQ(0x12, bytes32[27]);
    EXPECT_EQ(0x34, bytes32[28]);
    EXPECT_EQ(0x56, bytes32[29]);
    EXPECT_EQ(0x78, bytes32[30]);
    EXPECT_EQ(0x90, bytes32[31]);
}

TEST(fixed_bytes, be_from_le) {
    auto bytes32 = hex_string::from("0x123456789012345678901234567890").transform([](auto && hex) { return hex.template bytes<byte_numbering::lsb0>(); })
                                                                       .and_then([](auto && bytes) { return bytes32_be_t::from<byte_numbering::lsb0>(bytes); })
                                                                       .value();
    // 5678901234567890
    EXPECT_EQ(0x56, bytes32[24]);
    EXPECT_EQ(0x78, bytes32[25]);
    EXPECT_EQ(0x90, bytes32[26]);
    EXPECT_EQ(0x12, bytes32[27]);
    EXPECT_EQ(0x34, bytes32[28]);
    EXPECT_EQ(0x56, bytes32[29]);
    EXPECT_EQ(0x78, bytes32[30]);
    EXPECT_EQ(0x90, bytes32[31]);
}

TEST(fixed_bytes, le_from_be) {
    auto bytes32 = hex_string::from("0x123456789012345678901234567890").transform([](auto && hex) { return hex.template bytes<byte_numbering::msb0>(); })
                                                                       .and_then([](auto && bytes) { return bytes32_le_t::from<byte_numbering::msb0>(bytes); })
                                                                       .value();

    EXPECT_EQ(0x90, bytes32[0]);
    EXPECT_EQ(0x78, bytes32[1]);
    EXPECT_EQ(0x56, bytes32[2]);
    EXPECT_EQ(0x34, bytes32[3]);
    EXPECT_EQ(0x12, bytes32[4]);
    EXPECT_EQ(0x90, bytes32[5]);
    EXPECT_EQ(0x78, bytes32[6]);
    EXPECT_EQ(0x56, bytes32[7]);
    EXPECT_EQ(0x34, bytes32[8]);
    EXPECT_EQ(0x12, bytes32[9]);
    EXPECT_EQ(0x90, bytes32[10]);
    EXPECT_EQ(0x78, bytes32[11]);
    EXPECT_EQ(0x56, bytes32[12]);
    EXPECT_EQ(0x34, bytes32[13]);
    EXPECT_EQ(0x12, bytes32[14]);
    EXPECT_EQ(0x00, bytes32[15]);
    EXPECT_EQ(0x00, bytes32[16]);
}

TEST(fixed_bytes, le_from_le) {
    auto bytes32 = hex_string::from("0x123456789012345678901234567890").transform([](auto && hex) { return hex.template bytes<byte_numbering::lsb0>(); })
                                                                       .and_then([](auto && bytes) { return bytes32_le_t::from<byte_numbering::lsb0>(bytes); })
                                                                       .value();

    EXPECT_EQ(0x90, bytes32[0]);
    EXPECT_EQ(0x78, bytes32[1]);
    EXPECT_EQ(0x56, bytes32[2]);
    EXPECT_EQ(0x34, bytes32[3]);
    EXPECT_EQ(0x12, bytes32[4]);
    EXPECT_EQ(0x90, bytes32[5]);
    EXPECT_EQ(0x78, bytes32[6]);
    EXPECT_EQ(0x56, bytes32[7]);
    EXPECT_EQ(0x34, bytes32[8]);
    EXPECT_EQ(0x12, bytes32[9]);
    EXPECT_EQ(0x90, bytes32[10]);
    EXPECT_EQ(0x78, bytes32[11]);
    EXPECT_EQ(0x56, bytes32[12]);
    EXPECT_EQ(0x34, bytes32[13]);
    EXPECT_EQ(0x12, bytes32[14]);
    EXPECT_EQ(0x00, bytes32[15]);
    EXPECT_EQ(0x00, bytes32[16]);
}