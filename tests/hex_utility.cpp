// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/bytes.h"
#include "abc/hex_string.h"

#include <gtest/gtest.h>

#include <random>

TEST(hex_utility, hex_string_without_prefix) {
    EXPECT_TRUE(abc::hex_string::is_hex("0"));
    EXPECT_TRUE(abc::hex_string::is_hex("1"));
    EXPECT_TRUE(abc::hex_string::is_hex("2"));
    EXPECT_TRUE(abc::hex_string::is_hex("3"));
    EXPECT_TRUE(abc::hex_string::is_hex("4"));
    EXPECT_TRUE(abc::hex_string::is_hex("5"));
    EXPECT_TRUE(abc::hex_string::is_hex("6"));
    EXPECT_TRUE(abc::hex_string::is_hex("7"));
    EXPECT_TRUE(abc::hex_string::is_hex("8"));
    EXPECT_TRUE(abc::hex_string::is_hex("9"));
    EXPECT_TRUE(abc::hex_string::is_hex("a"));
    EXPECT_TRUE(abc::hex_string::is_hex("b"));
    EXPECT_TRUE(abc::hex_string::is_hex("c"));
    EXPECT_TRUE(abc::hex_string::is_hex("d"));
    EXPECT_TRUE(abc::hex_string::is_hex("e"));
    EXPECT_TRUE(abc::hex_string::is_hex("f"));
    EXPECT_TRUE(abc::hex_string::is_hex("A"));
    EXPECT_TRUE(abc::hex_string::is_hex("B"));
    EXPECT_TRUE(abc::hex_string::is_hex("C"));
    EXPECT_TRUE(abc::hex_string::is_hex("D"));
    EXPECT_TRUE(abc::hex_string::is_hex("E"));
    EXPECT_TRUE(abc::hex_string::is_hex("F"));
    EXPECT_TRUE(abc::hex_string::is_hex("0123456789abcdefABCDEF"));
    EXPECT_FALSE(abc::hex_string::is_hex("g"));
    EXPECT_FALSE(abc::hex_string::is_hex("G"));
    EXPECT_FALSE(abc::hex_string::is_hex("0123456789abcdefABCDEFg"));
    EXPECT_FALSE(abc::hex_string::is_hex("0123456789abcdefABCDEFG"));
}

TEST(hex_utility, hex_string_with_prefix) {
    EXPECT_TRUE(abc::hex_string::is_hex("0x0"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x1"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x2"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x3"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x4"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x5"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x6"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x7"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x8"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x9"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xa"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xb"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xc"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xd"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xe"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xf"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xA"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xB"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xC"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xD"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xE"));
    EXPECT_TRUE(abc::hex_string::is_hex("0xF"));
    EXPECT_TRUE(abc::hex_string::is_hex("0x0123456789abcdefABCDEF"));
    EXPECT_FALSE(abc::hex_string::is_hex("0xg"));
    EXPECT_FALSE(abc::hex_string::is_hex("0xG"));
    EXPECT_FALSE(abc::hex_string::is_hex("0x0123456789abcdefABCDEFg"));
    EXPECT_FALSE(abc::hex_string::is_hex("0x0123456789abcdefABCDEFG"));

    EXPECT_TRUE(abc::hex_string::is_hex("0X0"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X1"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X2"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X3"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X4"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X5"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X6"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X7"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X8"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X9"));
    EXPECT_TRUE(abc::hex_string::is_hex("0Xa"));
    EXPECT_TRUE(abc::hex_string::is_hex("0Xb"));
    EXPECT_TRUE(abc::hex_string::is_hex("0Xc"));
    EXPECT_TRUE(abc::hex_string::is_hex("0Xd"));
    EXPECT_TRUE(abc::hex_string::is_hex("0Xe"));
    EXPECT_TRUE(abc::hex_string::is_hex("0Xf"));
    EXPECT_TRUE(abc::hex_string::is_hex("0XA"));
    EXPECT_TRUE(abc::hex_string::is_hex("0XB"));
    EXPECT_TRUE(abc::hex_string::is_hex("0XC"));
    EXPECT_TRUE(abc::hex_string::is_hex("0XD"));
    EXPECT_TRUE(abc::hex_string::is_hex("0XE"));
    EXPECT_TRUE(abc::hex_string::is_hex("0XF"));
    EXPECT_TRUE(abc::hex_string::is_hex("0X0123456789abcdefABCDEF"));
    EXPECT_FALSE(abc::hex_string::is_hex("0Xg"));
    EXPECT_FALSE(abc::hex_string::is_hex("0XG"));
    EXPECT_FALSE(abc::hex_string::is_hex("0X0123456789abcdefABCDEFg"));
    EXPECT_FALSE(abc::hex_string::is_hex("0X0123456789abcdefABCDEFG"));
}

TEST(hex_utility, hex_binary) {
    for (auto i = 0u; i <= 0x0f; ++i) {
        EXPECT_TRUE(abc::hex_utility::is_hex(i));
    }

    for (size_t i = 16u; i <= 0xfffffff; ++i) {
        EXPECT_FALSE(abc::hex_utility::is_hex(i));
    }
}

TEST(hex_utility, hex_char) {
    for (int i = 0; i < 256; ++i) {
        if (auto const ch = static_cast<char>(i); ('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F')) {
            EXPECT_TRUE(abc::hex_utility::is_hex(ch));
        } else {
            EXPECT_FALSE(abc::hex_utility::is_hex(ch));
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
    EXPECT_EQ(abc::hex_string::to_bytes<abc::byte_numbering::lsb0>(hex_string.to_string()), bytes);
    EXPECT_EQ(abc::hex_string::to_bytes<abc::byte_numbering::lsb0>(hex_string.to_string().substr(2)), bytes);
}
