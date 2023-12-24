// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/bytes.h>
#include <abc/fixed_bytes.h>

#include <gtest/gtest.h>

TEST(bytes_view, bytes) {
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
        bytes_be_view_t bytes_be_view{bytes_be};

        ASSERT_EQ(bytes_be.size(), bytes_be_view.size());
        ASSERT_EQ('a', bytes_be_view[0]);
        ASSERT_EQ('b', bytes_be_view[1]);
        ASSERT_EQ('c', bytes_be_view[2]);
        ASSERT_EQ('d', bytes_be_view[3]);
    }


    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{ bytes_be.data(), bytes_be.size() };

        ASSERT_EQ(bytes_be.size(), bytes_be_view.size());
        ASSERT_EQ('a', bytes_be_view[0]);
        ASSERT_EQ('b', bytes_be_view[1]);
        ASSERT_EQ('c', bytes_be_view[2]);
        ASSERT_EQ('d', bytes_be_view[3]);
    }

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{bytes_be.data(), 2};

        ASSERT_EQ(2, bytes_be_view.size());
        ASSERT_EQ('a', bytes_be_view[0]);
        ASSERT_EQ('b', bytes_be_view[1]);
    }

    {
        bytes_be_t bytes_be{ 'a', 'b', 'c', 'd' };
        bytes_be_view_t bytes_be_view{bytes_be.last(2)};

        ASSERT_EQ(2, bytes_be_view.size());
        ASSERT_EQ('c', bytes_be_view[0]);
        ASSERT_EQ('d', bytes_be_view[1]);
    }
}

TEST(bytes_view, fixed_bytes) {
    using namespace abc;

    {
        bytes4_be_t bytes_be = bytes4_be_t::from<byte_numbering::msb0>({ 'a', 'b', 'c', 'd' }).value();
        bytes_be_view_t bytes_be_view{bytes_be};

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
    bytes_view_t bytes_view{std::span{bytes4}};

    ASSERT_EQ(bytes4.size(), bytes_view.size());
    ASSERT_EQ(bytes4[0], bytes_view[0]);
    ASSERT_EQ(bytes4[1], bytes_view[1]);
    ASSERT_EQ(bytes4[2], bytes_view[2]);
    ASSERT_EQ(bytes4[3], bytes_view[3]);
}