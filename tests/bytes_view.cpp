// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/bytes.h>
#include <abc/fixed_bytes.h>

#include <gtest/gtest.h>

TEST(bytes_view, bytes)
{
    using namespace abc;

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view = bytes_be;

        ASSERT_EQ(bytes_be.size(), bytes_be_view.size());
        ASSERT_EQ('a', bytes_be_view[0]);
        ASSERT_EQ('b', bytes_be_view[1]);
        ASSERT_EQ('c', bytes_be_view[2]);
        ASSERT_EQ('d', bytes_be_view[3]);
    }

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        auto bytes_be_view = static_cast<bytes_be_view_t>(bytes_be);

        ASSERT_EQ(bytes_be.size(), bytes_be_view.size());
        ASSERT_EQ('a', bytes_be_view[0]);
        ASSERT_EQ('b', bytes_be_view[1]);
        ASSERT_EQ('c', bytes_be_view[2]);
        ASSERT_EQ('d', bytes_be_view[3]);
    }

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{ bytes_be };

        ASSERT_EQ(bytes_be.size(), bytes_be_view.size());
        ASSERT_EQ('a', bytes_be_view[0]);
        ASSERT_EQ('b', bytes_be_view[1]);
        ASSERT_EQ('c', bytes_be_view[2]);
        ASSERT_EQ('d', bytes_be_view[3]);
    }

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{ bytes_be.data(), bytes_be.size(), byte_numbering_type<bytes_be_t::byte_numbering_value>{} };

        ASSERT_EQ(bytes_be.size(), bytes_be_view.size());
        ASSERT_EQ('a', bytes_be_view[0]);
        ASSERT_EQ('b', bytes_be_view[1]);
        ASSERT_EQ('c', bytes_be_view[2]);
        ASSERT_EQ('d', bytes_be_view[3]);
    }

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{ bytes_be.data(), 2, byte_numbering_type<bytes_be_t::byte_numbering_value>{} };

        ASSERT_EQ(2, bytes_be_view.size());
        ASSERT_EQ('a', bytes_be_view[0]);
        ASSERT_EQ('b', bytes_be_view[1]);
    }

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{ bytes_be.last(2) };

        ASSERT_EQ(2, bytes_be_view.size());
        ASSERT_EQ('c', bytes_be_view[0]);
        ASSERT_EQ('d', bytes_be_view[1]);
    }
}

TEST(bytes_view, fixed_bytes)
{
    using namespace abc;

    {
        bytes4_be_t bytes_be = bytes4_be_t::from<byte_numbering::msb0>({ 'a', 'b', 'c', 'd' }).value();
        bytes_be_view_t bytes_be_view{ bytes_be };

        ASSERT_EQ(bytes_be.size(), bytes_be_view.size());
        ASSERT_EQ('a', bytes_be_view[0]);
        ASSERT_EQ('b', bytes_be_view[1]);
        ASSERT_EQ('c', bytes_be_view[2]);
        ASSERT_EQ('d', bytes_be_view[3]);
    }
}

TEST(bytes_view, span)
{
    using namespace abc;

    bytes4_t bytes4 = bytes4_t::from<byte_numbering::none>({ 'a', 'b', 'c', 'd' }).value();
    bytes_view_t bytes_view{ bytes4 };

    ASSERT_EQ(bytes4.size(), bytes_view.size());
    ASSERT_EQ(bytes4[0], bytes_view[0]);
    ASSERT_EQ(bytes4[1], bytes_view[1]);
    ASSERT_EQ(bytes4[2], bytes_view[2]);
    ASSERT_EQ(bytes4[3], bytes_view[3]);
}

TEST(bytes_view, to_int)
{
    using namespace abc;
    {
        bytes_be_t bytes_be{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01020304050607);
    }

    {
        bytes_be_t bytes_be{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01020304050600);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x010203040506);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x010203040500);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0102030405);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x00 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0102030400);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01020304);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x00 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01020300);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x02, 0x03 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x010203);
        auto u32 = bytes_be_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x010203);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x02, 0x00 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x010200);
        auto u32 = bytes_be_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x010200);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x02 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0102);
        auto u32 = bytes_be_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x0102);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01, 0x00 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0100);
        auto u32 = bytes_be_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x0100);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x01 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01);
        auto u32 = bytes_be_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x01);
        auto u16 = bytes_be_view.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x01);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00, 0x00 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x00);
        auto u32 = bytes_be_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x00);
        auto u16 = bytes_be_view.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x00);
    }

    {
        auto bytes_be = bytes_be_t{ 0x01 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01);
        auto u32 = bytes_be_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x01);
        auto u16 = bytes_be_view.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x01);
        auto u8 = bytes_be_view.to<std::uint8_t>();
        ASSERT_EQ(u8, 0x01);
    }

    {
        auto bytes_be = bytes_be_t{ 0x00 };
        bytes_be_view_t bytes_be_view{ bytes_be };

        auto u64 = bytes_be_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x00);
        auto u32 = bytes_be_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x00);
        auto u16 = bytes_be_view.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x00);
        auto u8 = bytes_be_view.to<std::uint8_t>();
        ASSERT_EQ(u8, 0x00);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0706050403020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x06050403020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x06050403020100);
    }

    {
        auto bytes_le = bytes_le_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x050403020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x050403020100);
    }

    {
        auto bytes_le = bytes_le_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x00 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0403020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0403020100);
    }

    {
        auto bytes_le = bytes_le_t{ 0x00, 0x01, 0x02, 0x03, 0x00 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x03020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x03020100);
        auto u32 = bytes_le_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x03020100);
    }

    {
        auto bytes_le = bytes_le_t{ 0x00, 0x01, 0x02, 0x00 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x020100);
        auto u32 = bytes_le_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x020100);
        auto u32 = bytes_le_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x020100);
    }

    {
        auto bytes_le = bytes_le_t{ 0x00, 0x01, 0x00 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0100);
        auto u32 = bytes_le_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x0100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0100);
        auto u32 = bytes_le_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x0100);
        auto u16 = bytes_le_view.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x0100);
    }

    {
        auto bytes_le = bytes_le_t{ 0x00, 0x00 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x00);
        auto u32 = bytes_le_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x00);
        auto u16 = bytes_le_view.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x00);
    }

    {
        bytes_le_t bytes_le{ 0x01 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01);
        auto u32 = bytes_le_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x01);
        auto u16 = bytes_le_view.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x01);
        auto u8 = bytes_le_view.to<std::uint8_t>();
        ASSERT_EQ(u8, 0x01);
    }

    {
        auto bytes_le = bytes_le_t{ 0x00 };
        bytes_le_view_t bytes_le_view{ bytes_le };

        auto u64 = bytes_le_view.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x00);
        auto u32 = bytes_le_view.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x00);
        auto u16 = bytes_le_view.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x00);
        auto u8 = bytes_le_view.to<std::uint8_t>();
        ASSERT_EQ(u8, 0x00);
    }
}

TEST(bytes_be_view, operator_equal_equal)
{
    using namespace abc;

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{ bytes_be };

        ASSERT_TRUE(bytes_be_view == bytes_be_view);
        ASSERT_FALSE(bytes_be_view != bytes_be_view);
    }

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{ bytes_be };

        ASSERT_TRUE(bytes_be_view == bytes_be);
        ASSERT_FALSE(bytes_be_view != bytes_be);
    }

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{ bytes_be };

        ASSERT_TRUE(bytes_be == bytes_be_view);
        ASSERT_FALSE(bytes_be != bytes_be_view);
    }

    {
        bytes_be_t bytes_be1{ 'a', 'b', 'c', 'd' };
        bytes_be_t bytes_be2{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view1{ bytes_be1 };
        bytes_be_view_t bytes_be_view2{ bytes_be2 };

        ASSERT_TRUE(bytes_be_view1 == bytes_be_view2);
        ASSERT_FALSE(bytes_be_view1 != bytes_be_view2);
    }

    {
        bytes_be_t bytes_be1{ 'a', 'b', 'c', 'd' };
        bytes_be_t bytes_be2{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view1{ bytes_be1 };
        bytes_be_view_t bytes_be_view2{ bytes_be2 };

        ASSERT_TRUE(bytes_be_view1 == bytes_be2);
        ASSERT_FALSE(bytes_be_view1 != bytes_be2);
        ASSERT_TRUE(bytes_be_view2 == bytes_be1);
        ASSERT_FALSE(bytes_be_view2 != bytes_be1);
    }

    {
        bytes_be_t bytes_be1{ 'a', 'b', 'c', 'd' };
        bytes_be_t bytes_be2{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view1{ bytes_be1 };
        bytes_be_view_t bytes_be_view2{ bytes_be2 };

        ASSERT_TRUE(bytes_be1 == bytes_be_view2);
        ASSERT_FALSE(bytes_be1 != bytes_be_view2);
        ASSERT_TRUE(bytes_be2 == bytes_be_view1);
        ASSERT_FALSE(bytes_be2 != bytes_be_view1);
    }

    {
        bytes_be_t bytes_be1{ 'a', 'b', 'c', 'd' };
        bytes_be_t bytes_be2{ 'd', 'c', 'b', 'a' };
        bytes_be_view_t bytes_be_view1{ bytes_be1 };
        bytes_be_view_t bytes_be_view2{ bytes_be2 };

        ASSERT_TRUE(bytes_be_view1 != bytes_be_view2);
        ASSERT_FALSE(bytes_be_view1 == bytes_be_view2);
        ASSERT_TRUE(bytes_be_view2 != bytes_be1);
    }

    {
        bytes_be_t bytes_be1{ 'a', 'b', 'c', 'd' };
        bytes_be_t bytes_be2{ 'd', 'c', 'b', 'a' };
        bytes_be_view_t bytes_be_view1{ bytes_be1 };
        bytes_be_view_t bytes_be_view2{ bytes_be2 };

        ASSERT_TRUE(bytes_be_view1 != bytes_be2);
        ASSERT_FALSE(bytes_be_view1 == bytes_be2);
        ASSERT_TRUE(bytes_be_view2 != bytes_be1);
        ASSERT_FALSE(bytes_be_view2 == bytes_be1);
    }
}

TEST(bytes_le_view, operator_equal_equal)
{
    using namespace abc;

    {
        bytes_le_t bytes_le{ 'a', 'b', 'c', 'd' };
        bytes_le_view_t bytes_le_view{ bytes_le };

        ASSERT_TRUE(bytes_le_view == bytes_le_view);
        ASSERT_FALSE(bytes_le_view != bytes_le_view);
    }

    {
        bytes_le_t bytes_le{ 'a', 'b', 'c', 'd' };
        bytes_le_view_t bytes_le_view{ bytes_le };

        ASSERT_TRUE(bytes_le_view == bytes_le);
        ASSERT_FALSE(bytes_le_view != bytes_le);
    }

    {
        bytes_le_t bytes_le{ 'a', 'b', 'c', 'd' };
        bytes_le_view_t bytes_le_view{ bytes_le };

        ASSERT_TRUE(bytes_le == bytes_le_view);
        ASSERT_FALSE(bytes_le != bytes_le_view);
    }

    {
        bytes_le_t bytes_le1{ 'a', 'b', 'c', 'd' };
        bytes_le_t bytes_le2{ 'a', 'b', 'c', 'd' };
        bytes_le_view_t bytes_le_view1{ bytes_le1 };
        bytes_le_view_t bytes_le_view2{ bytes_le2 };

        ASSERT_TRUE(bytes_le_view1 == bytes_le_view2);
        ASSERT_FALSE(bytes_le_view1 != bytes_le_view2);
    }

    {
        bytes_le_t bytes_le1{ 'a', 'b', 'c', 'd' };
        bytes_le_t bytes_le2{ 'a', 'b', 'c', 'd' };
        bytes_le_view_t bytes_le_view1{ bytes_le1 };
        bytes_le_view_t bytes_le_view2{ bytes_le2 };

        ASSERT_TRUE(bytes_le_view1 == bytes_le2);
        ASSERT_FALSE(bytes_le_view1 != bytes_le2);
        ASSERT_TRUE(bytes_le_view2 == bytes_le1);
        ASSERT_FALSE(bytes_le_view2 != bytes_le1);
    }

    {
        bytes_le_t bytes_le1{ 'a', 'b', 'c', 'd' };
        bytes_le_t bytes_le2{ 'a', 'b', 'c', 'd' };
        bytes_le_view_t bytes_le_view1{ bytes_le1 };
        bytes_le_view_t bytes_le_view2{ bytes_le2 };

        ASSERT_TRUE(bytes_le1 == bytes_le_view2);
        ASSERT_FALSE(bytes_le1 != bytes_le_view2);
        ASSERT_TRUE(bytes_le2 == bytes_le_view1);
        ASSERT_FALSE(bytes_le2 != bytes_le_view1);
    }

    {
        bytes_le_t bytes_le1{ 'a', 'b', 'c', 'd' };
        bytes_le_t bytes_le2{ 'd', 'c', 'b', 'a' };
        bytes_le_view_t bytes_le_view1{ bytes_le1 };
        bytes_le_view_t bytes_le_view2{ bytes_le2 };

        ASSERT_TRUE(bytes_le_view1 != bytes_le_view2);
        ASSERT_FALSE(bytes_le_view1 == bytes_le_view2);
        ASSERT_TRUE(bytes_le_view2 != bytes_le1);
    }

    {
        bytes_le_t bytes_le1{ 'a', 'b', 'c', 'd' };
        bytes_le_t bytes_le2{ 'd', 'c', 'b', 'a' };
        bytes_le_view_t bytes_le_view1{ bytes_le1 };
        bytes_le_view_t bytes_le_view2{ bytes_le2 };

        ASSERT_TRUE(bytes_le_view1 != bytes_le2);
        ASSERT_FALSE(bytes_le_view1 == bytes_le2);
        ASSERT_TRUE(bytes_le_view2 != bytes_le1);
        ASSERT_FALSE(bytes_le_view2 == bytes_le1);
    }
}
