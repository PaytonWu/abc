// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/bytes.h"
#include "abc/hex_string.h"
#include "abc/hex_utility.h"

#include <gtest/gtest.h>

#include <random>

TEST(hex_utility, hex_string_without_prefix) {
    EXPECT_TRUE(abc::hex_string_without_prefix("0"));
    EXPECT_TRUE(abc::hex_string_without_prefix("1"));
    EXPECT_TRUE(abc::hex_string_without_prefix("2"));
    EXPECT_TRUE(abc::hex_string_without_prefix("3"));
    EXPECT_TRUE(abc::hex_string_without_prefix("4"));
    EXPECT_TRUE(abc::hex_string_without_prefix("5"));
    EXPECT_TRUE(abc::hex_string_without_prefix("6"));
    EXPECT_TRUE(abc::hex_string_without_prefix("7"));
    EXPECT_TRUE(abc::hex_string_without_prefix("8"));
    EXPECT_TRUE(abc::hex_string_without_prefix("9"));
    EXPECT_TRUE(abc::hex_string_without_prefix("a"));
    EXPECT_TRUE(abc::hex_string_without_prefix("b"));
    EXPECT_TRUE(abc::hex_string_without_prefix("c"));
    EXPECT_TRUE(abc::hex_string_without_prefix("d"));
    EXPECT_TRUE(abc::hex_string_without_prefix("e"));
    EXPECT_TRUE(abc::hex_string_without_prefix("f"));
    EXPECT_TRUE(abc::hex_string_without_prefix("A"));
    EXPECT_TRUE(abc::hex_string_without_prefix("B"));
    EXPECT_TRUE(abc::hex_string_without_prefix("C"));
    EXPECT_TRUE(abc::hex_string_without_prefix("D"));
    EXPECT_TRUE(abc::hex_string_without_prefix("E"));
    EXPECT_TRUE(abc::hex_string_without_prefix("F"));
    EXPECT_TRUE(abc::hex_string_without_prefix("0123456789abcdefABCDEF"));
    EXPECT_FALSE(abc::hex_string_without_prefix("g"));
    EXPECT_FALSE(abc::hex_string_without_prefix("G"));
    EXPECT_FALSE(abc::hex_string_without_prefix("0123456789abcdefABCDEFg"));
    EXPECT_FALSE(abc::hex_string_without_prefix("0123456789abcdefABCDEFG"));
}

TEST(hex_utility, hex_string_with_prefix) {
    EXPECT_TRUE(abc::hex_string_with_prefix("0x0"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x1"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x2"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x3"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x4"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x5"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x6"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x7"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x8"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x9"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xa"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xb"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xc"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xd"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xe"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xf"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xA"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xB"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xC"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xD"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xE"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0xF"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0x0123456789abcdefABCDEF"));
    EXPECT_FALSE(abc::hex_string_with_prefix("0xg"));
    EXPECT_FALSE(abc::hex_string_with_prefix("0xG"));
    EXPECT_FALSE(abc::hex_string_with_prefix("0x0123456789abcdefABCDEFg"));
    EXPECT_FALSE(abc::hex_string_with_prefix("0x0123456789abcdefABCDEFG"));

    EXPECT_TRUE(abc::hex_string_with_prefix("0X0"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X1"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X2"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X3"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X4"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X5"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X6"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X7"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X8"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X9"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0Xa"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0Xb"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0Xc"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0Xd"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0Xe"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0Xf"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0XA"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0XB"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0XC"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0XD"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0XE"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0XF"));
    EXPECT_TRUE(abc::hex_string_with_prefix("0X0123456789abcdefABCDEF"));
    EXPECT_FALSE(abc::hex_string_with_prefix("0Xg"));
    EXPECT_FALSE(abc::hex_string_with_prefix("0XG"));
    EXPECT_FALSE(abc::hex_string_with_prefix("0X0123456789abcdefABCDEFg"));
    EXPECT_FALSE(abc::hex_string_with_prefix("0X0123456789abcdefABCDEFG"));
}

TEST(hex_utility, hex_binary) {
    for (auto i = 0u; i <= 0x0f; ++i) {
        EXPECT_TRUE(abc::hex_binary(i));
    }

    for (size_t i = 16u; i <= 0xfffffff; ++i) {
        EXPECT_FALSE(abc::hex_binary(i));
    }
}

TEST(hex_utility, hex_char) {
    for (int i = 0; i < 256; ++i) {
        if (auto const ch = static_cast<char>(i); ('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F')) {
            EXPECT_TRUE(abc::hex_char(ch));
        } else {
            EXPECT_FALSE(abc::hex_char(ch));
        }
    }
}

TEST(hex_utility, hex_string_to_binary) {
    std::random_device rd;
    std::uniform_int_distribution distribution{ 0, 255 };
    abc::bytes bytes;

    bytes.resize(std::uniform_int_distribution<size_t>{0u, 5000u}(rd));
    for (auto & byte : bytes) {
        byte = static_cast<abc::byte>(distribution(rd));
    }

    auto const hex_string = abc::hex_string::from_bytes(bytes, abc::byte_numbering::lsb0);
    EXPECT_EQ(abc::hex_string_to_binary<std::endian::little>(hex_string.to_string()), bytes);
    EXPECT_EQ(abc::hex_string_to_binary<std::endian::little>(hex_string.to_string().substr(2)), bytes);
}
