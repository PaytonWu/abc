// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/hex_string.h"

#include <gtest/gtest.h>

using namespace abc;

TEST(hex_string, from_string_view) {
    auto const hex_string1 = hex_string::from("0x0123456789abcdefABCDEF").value();
    ASSERT_FALSE(hex_string1.empty());
    ASSERT_EQ(hex_string1.to_string(), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string1.to_string(hex_string::lower_case), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string1.to_string(hex_string::upper_case), "0X0123456789ABCDEFABCDEF");
    ASSERT_EQ(hex_string1.size(), hex_string1.length());
    ASSERT_EQ(hex_string1.size(), 22);
    ASSERT_EQ(hex_string1.bytes_size(), 11);
    ASSERT_EQ(hex_string1.bytes<byte_numbering::msb0>(), (bytes{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef }));

    auto const hex_string2 = hex_string::from("0x0123456789abcdefabcdef").value();
    ASSERT_EQ(std::strong_ordering::equal, hex_string1 <=> hex_string2);

    auto const hex_string3 = hex_string::from("0x123456789abcdefABCDEF").value();
    ASSERT_FALSE(hex_string3.empty());
    ASSERT_EQ(hex_string3.to_string(), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string3.to_string(hex_string::lower_case), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string3.to_string(hex_string::upper_case), "0X0123456789ABCDEFABCDEF");
    ASSERT_EQ(hex_string3.size(), hex_string3.length());
    ASSERT_EQ(hex_string3.size(), 22);
    ASSERT_EQ(hex_string3.bytes_size(), 11);
    ASSERT_EQ(hex_string3.bytes<byte_numbering::msb0>(), (bytes{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef }));
}

TEST(hex_string, from_bytes) {
    bytes const bytes{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 };
    auto const hex_string4 = hex_string::from_bytes(bytes, byte_numbering::msb0);
    ASSERT_FALSE(hex_string4.empty());
    ASSERT_EQ(hex_string4.to_string(), "0x0102030405060708090a0b0c0d0e0f10");
    ASSERT_EQ(hex_string4.to_string(hex_string::lower_case), "0x0102030405060708090a0b0c0d0e0f10");
    ASSERT_EQ(hex_string4.to_string(hex_string::format::upper_case), "0X0102030405060708090A0B0C0D0E0F10");
    ASSERT_EQ(hex_string4.size(), hex_string4.length());
    ASSERT_EQ(hex_string4.size(), 32);
    ASSERT_EQ(hex_string4.bytes_size(), 16);
    ASSERT_EQ(hex_string4.bytes<byte_numbering::msb0>(), bytes);
}

TEST(hex_string, from_bytes_zero) {
    bytes const bytes{ 0x00, 0x00 };
    auto const hex_string4 = hex_string::from_bytes(bytes, byte_numbering::msb0);
    ASSERT_FALSE(hex_string4.empty());
    ASSERT_EQ(hex_string4.to_string(), "0x0000");
    ASSERT_EQ(hex_string4.to_string(hex_string::lower_case), "0x0000");
    ASSERT_EQ(hex_string4.to_string(hex_string::format::upper_case), "0X0000");
    ASSERT_EQ(hex_string4.size(), hex_string4.length());
    ASSERT_EQ(hex_string4.size(), 4);
    ASSERT_EQ(hex_string4.bytes_size(), 2);
    ASSERT_EQ(hex_string4.bytes<byte_numbering::msb0>(), bytes);
}

TEST(hex_string, from_empty) {
    bytes const bytes{};
    auto const hex_string4 = hex_string::from_bytes(bytes, byte_numbering::lsb0);
    ASSERT_TRUE(hex_string4.empty());
    ASSERT_EQ(hex_string4.to_string(), "0x00");
    ASSERT_EQ(hex_string4.to_string(hex_string::lower_case), "0x00");
    ASSERT_EQ(hex_string4.to_string(hex_string::format::upper_case), "0X00");
    ASSERT_EQ(hex_string4.size(), hex_string4.length());
    ASSERT_EQ(hex_string4.size(), 0);
    ASSERT_EQ(hex_string4.bytes_size(), 0);
    ASSERT_EQ(hex_string4.bytes<byte_numbering::lsb0>(), bytes);
}

TEST(hex_string, operator_index) {
    auto const hex_string1 = hex_string::from("0x0123456789abcdefABCDEF").value();
    ASSERT_EQ(hex_string1[0], 'f');
    ASSERT_EQ(hex_string1[1], 'e');
    ASSERT_EQ(hex_string1[2], 'd');
    ASSERT_EQ(hex_string1[3], 'c');
    ASSERT_EQ(hex_string1[4], 'b');
    ASSERT_EQ(hex_string1[5], 'a');
    ASSERT_EQ(hex_string1[6], 'f');
    ASSERT_EQ(hex_string1[7], 'e');
    ASSERT_EQ(hex_string1[8], 'd');
    ASSERT_EQ(hex_string1[9], 'c');
    ASSERT_EQ(hex_string1[10], 'b');
    ASSERT_EQ(hex_string1[11], 'a');
    ASSERT_EQ(hex_string1[12], '9');
    ASSERT_EQ(hex_string1[13], '8');
    ASSERT_EQ(hex_string1[14], '7');
    ASSERT_EQ(hex_string1[15], '6');
    ASSERT_EQ(hex_string1[16], '5');
    ASSERT_EQ(hex_string1[17], '4');
    ASSERT_EQ(hex_string1[18], '3');
    ASSERT_EQ(hex_string1[19], '2');
    ASSERT_EQ(hex_string1[20], '1');
    ASSERT_EQ(hex_string1[21], '0');

    auto hex_string2 = hex_string::from("0x0123456789abcdefABCDEF").value();
    hex_string2[0] = '0';
    hex_string2[1] = '1';
    hex_string2[2] = '2';
    hex_string2[3] = '3';
    hex_string2[4] = '4';
    hex_string2[5] = '5';
    hex_string2[6] = '6';
    hex_string2[7] = '7';
    hex_string2[8] = '8';
    hex_string2[9] = '9';
    hex_string2[10] = 'a';
    hex_string2[11] = 'b';
    hex_string2[12] = 'c';
    hex_string2[13] = 'd';
    hex_string2[14] = 'e';
    hex_string2[15] = 'f';
    hex_string2[16] = 'A';
    hex_string2[17] = 'B';
    hex_string2[18] = 'C';
    hex_string2[19] = 'D';
    hex_string2[20] = 'E';
    hex_string2[21] = 'F';

    ASSERT_EQ(hex_string2.to_string(), "0xfedcbafedcba9876543210");

    auto hex_string3 = hex_string::from("0x0123456789abcdefABCDEF").value();
    ASSERT_THROW(hex_string3[20] = 'g', abc::abc_error);
}

TEST(hex_string, from) {
    auto const hex_string1 = hex_string::from("0123456789abcdefABCDEF").value();
    ASSERT_FALSE(hex_string1.empty());
    ASSERT_EQ(hex_string1.to_string(), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string1.to_string(hex_string::lower_case), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string1.to_string(hex_string::upper_case), "0X0123456789ABCDEFABCDEF");
    ASSERT_EQ(hex_string1.size(), hex_string1.length());
    ASSERT_EQ(hex_string1.size(), 22);
    ASSERT_EQ(hex_string1.bytes_size(), 11);
    ASSERT_EQ(hex_string1.bytes<byte_numbering::msb0>(), (bytes{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef }));
}

TEST(hex_string, from_invalid) {
    auto const result = hex_string::from("0123456789abcdefABCDEFX");
    ASSERT_FALSE(result.has_value());
}