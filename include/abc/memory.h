// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_MEMORY
#define ABC_INCLUDE_ABC_MEMORY

#pragma once

#include "byte.h"

#include <bit>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <memory>

namespace abc {

template <std::size_t Alignment>
constexpr auto aligned_size(std::size_t size) noexcept -> std::size_t {
    static_assert(std::has_single_bit(Alignment), "Alignment must be a power of 2");
    return (size + (Alignment - 1)) & -Alignment;
}

constexpr auto aligned_size(std::size_t size, std::size_t alignment) noexcept -> std::size_t {
    assert(std::has_single_bit(alignment));
    return (size + (alignment - 1)) & -alignment;
}

template <std::size_t Alignment>
inline auto address_aligned_at(byte * ptr) noexcept -> byte * {
    static_assert(std::has_single_bit(Alignment), "Alignment must be a power of 2");
    return reinterpret_cast<byte *>((reinterpret_cast<std::uintptr_t>(ptr) + (Alignment - 1)) & -Alignment);
}

template <typename T>
inline auto address_aligned_at(byte * ptr) noexcept -> T * {
    return reinterpret_cast<T *>(address_aligned_at<alignof(T)>(ptr));
}

inline auto address_aligned_at(std::size_t alignment, byte * ptr) noexcept -> byte * {
    assert(std::has_single_bit(alignment));
    return reinterpret_cast<byte *>((reinterpret_cast<std::uintptr_t>(ptr) + (alignment - 1)) & -alignment);

}

}

#endif //ABC_INCLUDE_ABC_MEMORY
