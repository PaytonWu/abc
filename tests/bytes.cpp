// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/bytes.h>
#include <abc/hex_string.h>

#include <gtest/gtest.h>

#include <ranges>

using namespace abc;

TEST(xbytes_endian_t, constructor_il)
{
    {
        bytes_msb0_t bytes = bytes_be_t::from<ByteNumbering::Msb0>({ 0x01, 0x02, 0x03, 0x04 });
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

TEST(xbytes_endian_t, construct_uint)
{
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

TEST(bytes_endian_t, from_negtive_int)
{
    bytes_lsb0_t bytes = bytes_lsb0_t::from(-1);
    EXPECT_EQ(bytes[0], 0xff);
    EXPECT_EQ(bytes[1], 0xff);
    EXPECT_EQ(bytes[2], 0xff);
    EXPECT_EQ(bytes[3], 0xff);
    EXPECT_EQ(bytes.size(), 4);
}

TEST(bytes, operator_plus_bytes)
{
    {
        bytes_be_t lhs = bytes_be_t::from(0x1234567890);
        bytes_be_t rhs = bytes_be_t::from(0x0987654321);

        lhs = lhs + rhs;
        ASSERT_EQ(abc::hex_string::from("0x12345678900987654321").transform([](auto const & hex_string) { return hex_string.template bytes<abc::ByteNumbering::Msb0>(); }).value(),
                  lhs);
    }

    {
        bytes_be_t lhs = bytes_be_t::from(0x1234567890);
        bytes_be_t rhs = bytes_be_t::from(0x0987654321);

        void const * lhs_ptr = std::addressof(lhs);
        lhs += rhs;
        ASSERT_EQ(abc::hex_string::from("0x12345678900987654321").transform([](auto const & hex_string) { return hex_string.template bytes<abc::ByteNumbering::Msb0>(); }).value(),
                  lhs);
        ASSERT_EQ(lhs_ptr, std::addressof(lhs));
    }
}

TEST(bytes, operator_plus_byte_after)
{
    {
        bytes_be_t lhs = bytes_be_t::from(0x1234567890);

        lhs = lhs + static_cast<abc::byte>(0);
        ASSERT_EQ(abc::hex_string::from("0x123456789000").transform([](auto const & hex_string) { return hex_string.template bytes<abc::ByteNumbering::Msb0>(); }).value(), lhs);
    }

    {
        bytes_be_t lhs = bytes_be_t::from(0x1234567890);

        void const * lhs_ptr = std::addressof(lhs);
        lhs += static_cast<abc::byte>(0);
        ASSERT_EQ(abc::hex_string::from("0x123456789000").transform([](auto const & hex_string) { return hex_string.template bytes<abc::ByteNumbering::Msb0>(); }).value(), lhs);
        ASSERT_EQ(lhs_ptr, std::addressof(lhs));
    }
}

TEST(bytes, operator_plus_byte_before)
{
    bytes_be_t lhs = bytes_be_t::from(0x1234567890);

    lhs = static_cast<abc::byte>(1) + lhs.view();
    ASSERT_EQ(abc::hex_string::from("0x11234567890").transform([](auto const & hex_string) { return hex_string.template bytes<abc::ByteNumbering::Msb0>(); }).value(), lhs);
}

TEST(bytes, operator_plus_view)
{
    bytes_be_t lhs{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
    bytes_be_t bytes{ 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };

    lhs = lhs + bytes_be_view_t{ bytes };

    for (uint8_t i = 0u; i < static_cast<uint8_t>(lhs.size()); ++i)
    {
        ASSERT_EQ(i, lhs[i]);
    }
}

TEST(bytes, to)
{
    {
        bytes_be_t bytes_be{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        auto bytes_le = bytes_be.to<ByteNumbering::Lsb0>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(bytes_be.size()); ++i)
        {
            ASSERT_EQ(i, bytes_le[bytes_be.size() - i - 1]);
        }
    }

    {
        auto bytes_le = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 }.to<ByteNumbering::Lsb0>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(8); ++i)
        {
            ASSERT_EQ(i, bytes_le[8 - i - 1]);
        }
    }

    {
        bytes_be_t bytes_be{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        auto bytes = bytes_be.to<ByteNumbering::None>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(bytes_be.size()); ++i)
        {
            ASSERT_EQ(i, bytes[i]);
        }
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 }.to<ByteNumbering::None>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(8); ++i)
        {
            ASSERT_EQ(i, bytes[i]);
        }
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        auto bytes_be = bytes_le.to<ByteNumbering::Msb0>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(bytes_le.size()); ++i)
        {
            ASSERT_EQ(i, bytes_be[bytes_be.size() - i - 1]);
        }
    }

    {
        auto bytes_be = bytes_le_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 }.to<ByteNumbering::Msb0>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(8); ++i)
        {
            ASSERT_EQ(i, bytes_be[8 - i - 1]);
        }
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        auto bytes = bytes_le.to<ByteNumbering::None>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(bytes_le.size()); ++i)
        {
            ASSERT_EQ(i, bytes[i]);
        }
    }

    {
        auto bytes = bytes_le_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 }.to<ByteNumbering::None>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(8); ++i)
        {
            ASSERT_EQ(i, bytes[i]);
        }
    }

    {
        bytes_t bytes{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        auto bytes_be = bytes.to<ByteNumbering::Msb0>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(bytes.size()); ++i)
        {
            ASSERT_EQ(i, bytes_be[i]);
        }
    }

    {
        auto bytes_be = bytes_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 }.to<ByteNumbering::Msb0>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(8); ++i)
        {
            ASSERT_EQ(i, bytes_be[i]);
        }
    }

    {
        bytes_t bytes{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        auto bytes_le = bytes.to<ByteNumbering::Lsb0>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(bytes.size()); ++i)
        {
            ASSERT_EQ(i, bytes_le[i]);
        }
    }

    {
        auto bytes_le = bytes_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 }.to<ByteNumbering::Lsb0>();
        for (uint8_t i = 0u; i < static_cast<uint8_t>(8); ++i)
        {
            ASSERT_EQ(i, bytes_le[i]);
        }
    }
}

TEST(bytes, from_bytes_view)
{
    {
        bytes_be_t bytes_be_src = bytes_be_t::from(0x01020304);
        bytes_be_view_t bytes_be_view = bytes_be_src;

        auto bytes = bytes_be_t::from(bytes_be_view);
        ASSERT_EQ(bytes_be_src, bytes);
    }

    {
        bytes_t src = abc::bytes_t::from(0x01020304);
        bytes_view_t view = src;
        auto bytes = bytes_be_t::from(view);

        ASSERT_EQ(bytes[0], 0x04);
        ASSERT_EQ(bytes[1], 0x03);
        ASSERT_EQ(bytes[2], 0x02);
        ASSERT_EQ(bytes[3], 0x01);
    }
}

TEST(bytes, to_int)
{
    {
        bytes_be_t bytes_be{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        auto u64 = bytes_be.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01020304050607);
    }

    {
        bytes_be_t bytes_be{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00 };
        auto u64 = bytes_be.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01020304050600);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x010203040506);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x010203040500);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0102030405);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0102030400);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x04 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01020304);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02, 0x03, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01020300);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02, 0x03 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x010203);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x010203);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x010200);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x010200);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x02 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0102);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x0102);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0100);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x0100);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x01 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x01);
        auto u16 = bytes.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x01);
    }

    {
        auto bytes = bytes_be_t{ 0x00, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x00);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x00);
        auto u16 = bytes.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x00);
    }

    {
        auto bytes = bytes_be_t{ 0x01 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x01);
        auto u16 = bytes.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x01);
        auto u8 = bytes.to<std::uint8_t>();
        ASSERT_EQ(u8, 0x01);
    }

    {
        auto bytes = bytes_be_t{ 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x00);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x00);
        auto u16 = bytes.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x00);
        auto u8 = bytes.to<std::uint8_t>();
        ASSERT_EQ(u8, 0x00);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07 };
        auto u64 = bytes_le.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0706050403020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x00 };
        auto u64 = bytes_le.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x06050403020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06 };
        auto u64 = bytes_le.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x06050403020100);
    }

    {
        auto bytes = bytes_le_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x050403020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05 };
        auto u64 = bytes_le.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x050403020100);
    }

    {
        auto bytes = bytes_le_t{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0403020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03, 0x04 };
        auto u64 = bytes_le.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0403020100);
    }

    {
        auto bytes = bytes_le_t{ 0x00, 0x01, 0x02, 0x03, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x03020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03 };
        auto u64 = bytes_le.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x03020100);
        auto u32 = bytes_le.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x03020100);
    }

    {
        auto bytes = bytes_le_t{ 0x00, 0x01, 0x02, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x020100);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x020100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02 };
        auto u64 = bytes_le.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x020100);
        auto u32 = bytes_le.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x020100);
    }

    {
        auto bytes = bytes_le_t{ 0x00, 0x01, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0100);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x0100);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01 };
        auto u64 = bytes_le.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x0100);
        auto u32 = bytes_le.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x0100);
        auto u16 = bytes_le.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x0100);
    }

    {
        auto bytes = bytes_le_t{ 0x00, 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x00);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x00);
        auto u16 = bytes.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x00);
    }

    {
        bytes_le_t bytes_le{ 0x01 };
        auto u64 = bytes_le.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x01);
        auto u32 = bytes_le.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x01);
        auto u16 = bytes_le.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x01);
        auto u8 = bytes_le.to<std::uint8_t>();
        ASSERT_EQ(u8, 0x01);
    }

    {
        auto bytes = bytes_le_t{ 0x00 };
        auto u64 = bytes.to<std::uint64_t>();
        ASSERT_EQ(u64, 0x00);
        auto u32 = bytes.to<std::uint32_t>();
        ASSERT_EQ(u32, 0x00);
        auto u16 = bytes.to<std::uint16_t>();
        ASSERT_EQ(u16, 0x00);
        auto u8 = bytes.to<std::uint8_t>();
        ASSERT_EQ(u8, 0x00);
    }
}

TEST(bytes, assign_from_view)
{
    {
        bytes_be_t bytes_be{ 0x00, 0x01, 0x02, 0x03 };
        bytes_be_t const bytes_be_expected{ 0x04, 0x05, 0x06, 0x07 };
        bytes_be_view_t bytes_be_view{ bytes_be_expected };

        bytes_be = bytes_be_view;
        ASSERT_EQ(bytes_be_expected, bytes_be);
    }

    {
        bytes_le_t bytes_le{ 0x00, 0x01, 0x02, 0x03 };
        bytes_le_t const bytes_le_expected{ 0x04, 0x05, 0x06, 0x07 };
        bytes_le_view_t bytes_le_view{ bytes_le_expected };

        bytes_le = bytes_le_view;
        ASSERT_EQ(bytes_le_expected, bytes_le);
    }

    {
        bytes_t bytes{ 0x00, 0x01, 0x02, 0x03 };
        bytes_t const bytes_expected{ 0x04, 0x05, 0x06, 0x07 };
        bytes_view_t bytes_view{ bytes_expected };

        bytes = bytes_view;
        ASSERT_EQ(bytes_expected, bytes);
    }
}
