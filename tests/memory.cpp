// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/memory.h>

#include <gtest/gtest.h>

#include <bit>
#include <limits>

TEST(aligned_size, template) {
    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ(sz, abc::aligned_size<1>(sz));
    }
    ASSERT_EQ(std::numeric_limits<size_t>::max(), abc::aligned_size<1>(std::numeric_limits<size_t>::max()));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ((sz + 1) & ~1, abc::aligned_size<2>(sz));
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size<2>(std::numeric_limits<size_t>::max()));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 1, abc::aligned_size<2>(std::numeric_limits<size_t>::max() - 1));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ(abc::aligned_size<4>(sz), (sz + 3) & ~3);
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size<4>(std::numeric_limits<size_t>::max()));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 3, abc::aligned_size<4>(std::numeric_limits<size_t>::max() - 3));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ(abc::aligned_size<8>(sz), (sz + 7) & ~7);
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size<8>(std::numeric_limits<size_t>::max()));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 7, abc::aligned_size<8>(std::numeric_limits<size_t>::max() - 7));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ(abc::aligned_size<16>(sz), (sz + 15) & ~15);
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size<16>(std::numeric_limits<size_t>::max()));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 15, abc::aligned_size<16>(std::numeric_limits<size_t>::max() - 15));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ(abc::aligned_size<32>(sz), (sz + 31) & ~31);
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size<32>(std::numeric_limits<size_t>::max()));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 31, abc::aligned_size<32>(std::numeric_limits<size_t>::max() - 31));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ(abc::aligned_size<64>(sz), (sz + 63) & ~63);
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size<64>(std::numeric_limits<size_t>::max()));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 63, abc::aligned_size<64>(std::numeric_limits<size_t>::max() - 63));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ(abc::aligned_size<128>(sz), (sz + 127) & ~127);
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size<128>(std::numeric_limits<size_t>::max()));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 127, abc::aligned_size<128>(std::numeric_limits<size_t>::max() - 127));
}

TEST(aligned_size, normal) {
    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ(sz, abc::aligned_size(sz, 1));
    }
    ASSERT_EQ(std::numeric_limits<size_t>::max(), abc::aligned_size(std::numeric_limits<size_t>::max(), 1));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ((sz + 1) & ~1, abc::aligned_size(sz, 2));
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size(std::numeric_limits<size_t>::max(), 2));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 1, abc::aligned_size(std::numeric_limits<size_t>::max() - 1, 2));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ((sz + 3) & ~3, abc::aligned_size(sz, 4));
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size(std::numeric_limits<size_t>::max(), 4));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 3, abc::aligned_size(std::numeric_limits<size_t>::max() - 3, 4));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ((sz + 7) & ~7, abc::aligned_size(sz, 8));
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size(std::numeric_limits<size_t>::max(), 8));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 7, abc::aligned_size(std::numeric_limits<size_t>::max() - 7, 8));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ((sz + 15) & ~15, abc::aligned_size(sz, 16));
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size(std::numeric_limits<size_t>::max(), 16));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 15, abc::aligned_size(std::numeric_limits<size_t>::max() - 15, 16));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ((sz + 31) & ~31, abc::aligned_size(sz, 32));
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size(std::numeric_limits<size_t>::max(), 32));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 31, abc::aligned_size(std::numeric_limits<size_t>::max() - 31, 32));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ((sz + 63) & ~63, abc::aligned_size(sz, 64));
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size(std::numeric_limits<size_t>::max(), 64));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 63, abc::aligned_size(std::numeric_limits<size_t>::max() - 63, 64));

    for (auto sz = 0zu; sz < 100; ++sz) {
        ASSERT_EQ((sz + 127) & ~127, abc::aligned_size(sz, 128));
    }
    // overflow
    ASSERT_EQ(0, abc::aligned_size(std::numeric_limits<size_t>::max(), 128));
    ASSERT_EQ(std::numeric_limits<size_t>::max() - 127, abc::aligned_size(std::numeric_limits<size_t>::max() - 127, 128));
}