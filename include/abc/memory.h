// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_MEMORY
#define ABC_INCLUDE_ABC_MEMORY

#pragma once

#include <cassert>
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
inline auto address_aligned_at(void * ptr) noexcept -> void * {
    static_assert(std::has_single_bit(Alignment), "Alignment must be a power of 2");
    return reinterpret_cast<void *>((reinterpret_cast<std::uintptr_t>(ptr) + (Alignment - 1)) & -Alignment);
}

inline auto address_aligned_at(std::size_t alignment, void * ptr) noexcept -> void * {
    assert(std::has_single_bit(alignment));
    return reinterpret_cast<void *>((reinterpret_cast<std::uintptr_t>(ptr) + (alignment - 1)) & -alignment);

}

}

#endif //ABC_INCLUDE_ABC_MEMORY
