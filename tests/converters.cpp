// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/converter.h>

#include <gtest/gtest.h>

TEST(converters, hex_string_to_fixed_bytes) {
    abc::bytes32_t key_data = abc::convert_to<abc::bytes32_t>::from(abc::hex_string::from("c67a31aca1e2bad8469003930c6d08f80f5087720d2276d3c85ad74d3297adec").value()).value();
    EXPECT_EQ(0xc6, key_data[0]);
    EXPECT_EQ(0x7a, key_data[1]);
    EXPECT_EQ(0x31, key_data[2]);
    EXPECT_EQ(0xec, key_data[31]);
    EXPECT_EQ(0xad, key_data[30]);
    EXPECT_EQ(0x97, key_data[29]);
}