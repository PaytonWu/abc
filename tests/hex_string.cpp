// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/hex_string.h"

#include <gtest/gtest.h>

TEST(hex_string, from_string_view) {
    using namespace abc;
    auto const hex_string1 = xhex_string_t::from("0x0123456789abcdefABCDEF");
    ASSERT_FALSE(hex_string1.empty());
    ASSERT_EQ(hex_string1.value(), "0123456789abcdefABCDEF");
    ASSERT_EQ(hex_string1.to_string(), "0x0123456789abcdefABCDEF");
    ASSERT_EQ(hex_string1.to_string(xhex_string_format_t::without_prefix), "0123456789abcdefABCDEF");
    ASSERT_EQ(hex_string1.size(), hex_string1.length());
    ASSERT_EQ(hex_string1.size(xhex_string_format_t::without_prefix), hex_string1.length(xhex_string_format_t::without_prefix));
    ASSERT_EQ(hex_string1.size(), 24);
    ASSERT_EQ(hex_string1.size(xhex_string_format_t::without_prefix), 22);

    auto const hex_string2 = xhex_string_t::from("0x0123456789abcdefabcdef");
    ASSERT_EQ(std::weak_ordering::equivalent, hex_string1 <=> hex_string2);

    // hex_string = xhex_string_t::
}
