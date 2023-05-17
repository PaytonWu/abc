// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/hex_string.h"

#include <gtest/gtest.h>

using namespace abc;

TEST(hex_string, from_string_view) {
    auto const hex_string1 = xhex_string_t::from_hex_prefixed("0x0123456789abcdefABCDEF").value();
    ASSERT_FALSE(hex_string1.empty());
    ASSERT_EQ(hex_string1.to_string(), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string1.to_string(xhex_string_t::lower_case), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string1.to_string(xhex_string_t::upper_case), "0X0123456789ABCDEFABCDEF");
    ASSERT_EQ(hex_string1.size(), hex_string1.length());
    ASSERT_EQ(hex_string1.size(), 22);
    ASSERT_EQ(hex_string1.bytes_size(), 11);
    ASSERT_EQ(hex_string1.to_bytes<xbyte_numbering_t::msb0>(), (xbytes_t{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef }));

    auto const hex_string2 = xhex_string_t::from_hex_prefixed("0x0123456789abcdefabcdef").value();
    ASSERT_EQ(std::strong_ordering::equal, hex_string1 <=> hex_string2);

    auto const hex_string3 = xhex_string_t::from_hex_prefixed("0x123456789abcdefABCDEF").value();
    ASSERT_FALSE(hex_string3.empty());
    ASSERT_EQ(hex_string3.to_string(), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string3.to_string(xhex_string_t::lower_case), "0x0123456789abcdefabcdef");
    ASSERT_EQ(hex_string3.to_string(xhex_string_t::upper_case), "0X0123456789ABCDEFABCDEF");
    ASSERT_EQ(hex_string3.size(), hex_string3.length());
    ASSERT_EQ(hex_string3.size(), 22);
    ASSERT_EQ(hex_string3.bytes_size(), 11);
    ASSERT_EQ(hex_string3.to_bytes<xbyte_numbering_t::msb0>(), (xbytes_t{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xab, 0xcd, 0xef }));
}

TEST(hex_string, from_bytes) {
    xbytes_t const bytes{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10 };
    auto const hex_string4 = xhex_string_t::from_bytes(bytes, xbyte_numbering_t::msb0);
    ASSERT_FALSE(hex_string4.empty());
    ASSERT_EQ(hex_string4.to_string(), "0x0102030405060708090a0b0c0d0e0f10");
    ASSERT_EQ(hex_string4.to_string(xhex_string_t::lower_case), "0x0102030405060708090a0b0c0d0e0f10");
    ASSERT_EQ(hex_string4.to_string(xhex_string_t::format::upper_case), "0X0102030405060708090A0B0C0D0E0F10");
    ASSERT_EQ(hex_string4.size(), hex_string4.length());
    ASSERT_EQ(hex_string4.size(), 32);
    ASSERT_EQ(hex_string4.bytes_size(), 16);
    ASSERT_EQ(hex_string4.to_bytes<xbyte_numbering_t::msb0>(), bytes);
}

TEST(hex_string, from_bytes_zero) {
    xbytes_t const bytes{ 0x00, 0x00 };
    auto const hex_string4 = xhex_string_t::from_bytes(bytes, xbyte_numbering_t::msb0);
    ASSERT_FALSE(hex_string4.empty());
    ASSERT_EQ(hex_string4.to_string(), "0x0000");
    ASSERT_EQ(hex_string4.to_string(xhex_string_t::lower_case), "0x0000");
    ASSERT_EQ(hex_string4.to_string(xhex_string_t::format::upper_case), "0X0000");
    ASSERT_EQ(hex_string4.size(), hex_string4.length());
    ASSERT_EQ(hex_string4.size(), 4);
    ASSERT_EQ(hex_string4.bytes_size(), 2);
    ASSERT_EQ(hex_string4.to_bytes<xbyte_numbering_t::msb0>(), bytes);
}

TEST(hex_string, from_empty) {
    xbytes_t const bytes{};
    auto const hex_string4 = xhex_string_t::from_bytes(bytes, xbyte_numbering_t::lsb0);
    ASSERT_TRUE(hex_string4.empty());
    ASSERT_EQ(hex_string4.to_string(), "0x00");
    ASSERT_EQ(hex_string4.to_string(xhex_string_t::lower_case), "0x00");
    ASSERT_EQ(hex_string4.to_string(xhex_string_t::format::upper_case), "0X00");
    ASSERT_EQ(hex_string4.size(), hex_string4.length());
    ASSERT_EQ(hex_string4.size(), 0);
    ASSERT_EQ(hex_string4.bytes_size(), 0);
    ASSERT_EQ(hex_string4.to_bytes<xbyte_numbering_t::lsb0>(), bytes);
}
