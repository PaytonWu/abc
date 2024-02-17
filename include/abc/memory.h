// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_INCLUDE_ABC_MEMORY)
#define ABC_INCLUDE_ABC_MEMORY

#pragma once

#include "byte.h"

#include <bit>
#include <cassert>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <type_traits>

namespace abc
{

template <std::size_t Alignment>
constexpr auto
aligned_size(std::size_t size) noexcept -> std::size_t
{
    static_assert(std::has_single_bit(Alignment), "Alignment must be a power of 2");
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
    // warning C4146: unary minus operator applied to unsigned type, result still unsigned
    return (size + (Alignment - 1)) & -Alignment;
#pragma warning(pop)
#else
    return (size + (Alignment - 1)) & -Alignment;
#endif
}

constexpr auto
aligned_size(std::size_t size, std::size_t alignment) noexcept -> std::size_t
{
    assert(std::has_single_bit(alignment));
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
    // warning C4146: unary minus operator applied to unsigned type, result still unsigned
    return (size + (alignment - 1)) & -alignment;
#pragma warning(pop)
#else
    return (size + (alignment - 1)) & -alignment;
#endif
}

template <std::size_t Alignment>
inline auto
address_aligned_at(byte * ptr) noexcept -> byte *
{
    static_assert(std::has_single_bit(Alignment), "Alignment must be a power of 2");
    return reinterpret_cast<byte *>((reinterpret_cast<std::uintptr_t>(ptr) + (Alignment - 1)) & -Alignment);
}

template <typename T>
inline auto
address_aligned_at(byte * ptr) noexcept -> T *
{
    return reinterpret_cast<T *>(address_aligned_at<alignof(T)>(ptr));
}

inline auto
address_aligned_at(std::size_t alignment, byte * ptr) noexcept -> byte *
{
    assert(std::has_single_bit(alignment));
#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4146)
    // warning C4146: unary minus operator applied to unsigned type, result still unsigned
    return reinterpret_cast<byte *>((reinterpret_cast<std::uintptr_t>(ptr) + (alignment - 1)) & -alignment);
#pragma warning(pop)
#else
    return reinterpret_cast<byte *>((reinterpret_cast<std::uintptr_t>(ptr) + (alignment - 1)) & -alignment);
#endif
}

template <typename T>
class observer_ptr
{
public:
    using element_type = T;
    using pointer_type = std::add_pointer_t<T>;
    using reference_type = std::add_lvalue_reference_t<T>;

private:
    pointer_type ptr_;

public:
    constexpr observer_ptr() noexcept : ptr_{}
    {
    }

    constexpr observer_ptr(nullptr_t) noexcept : ptr_{}
    {
    }

    constexpr explicit observer_ptr(pointer_type ptr) noexcept : ptr_{ptr}
    {
    }

    template <typename U, typename = typename std::enable_if_t<std::is_convertible<typename std::add_pointer<U>::type, pointer_type>::value>>
    constexpr observer_ptr(observer_ptr<U> p) noexcept : ptr_{p.get()}
    {
    }

    // 3.2.3, observer_ptr observers
    constexpr auto
    get() const noexcept -> pointer_type
    {
        return ptr_;
    }

    constexpr auto
    operator*() const -> reference_type
    {
        return *get();
    }

    constexpr auto
    operator->() const noexcept -> pointer_type
    {
        return get();
    }

    constexpr explicit
    operator bool() const noexcept
    {
        return get() != nullptr;
    }

    // 3.2.4, observer_ptr conversions
    constexpr explicit
    operator pointer_type() const noexcept
    {
        return get();
    }

    // 3.2.5, observer_ptr modifiers
    constexpr auto
    release() noexcept -> pointer_type
    {
        pointer_type tmp = get();
        reset();
        return tmp;
    }

    constexpr void
    reset(pointer_type ptr = nullptr) noexcept
    {
        ptr_ = ptr;
    }

    constexpr void
    swap(observer_ptr & p) noexcept
    {
        std::swap(ptr_, p.ptr_);
    }
};

template <typename T>
constexpr void
swap(observer_ptr<T> & lhs, observer_ptr<T> & rhs) noexcept
{
    lhs.swap(rhs);
}

template <typename T>
constexpr auto
make_observer(T * ptr) noexcept -> observer_ptr<T>
{
    return observer_ptr<T>(ptr);
}

template <typename T, typename U>
constexpr auto
operator==(observer_ptr<T> lhs, observer_ptr<U> rhs) -> bool
{
    return lhs.get() == rhs.get();
}

template <typename T, typename U>
constexpr auto
operator!=(observer_ptr<T> lhs, observer_ptr<U> rhs) -> bool
{
    return !(lhs == rhs);
}

template <typename T>
constexpr auto
operator==(observer_ptr<T> ptr, nullptr_t) noexcept -> bool
{
    return !ptr;
}

template <typename T>
constexpr auto
operator==(nullptr_t, observer_ptr<T> ptr) noexcept -> bool
{
    return !ptr;
}

template <typename T>
constexpr auto
operator!=(observer_ptr<T> ptr, nullptr_t) noexcept -> bool
{
    return bool(ptr);
}

template <typename T>
constexpr auto
operator!=(nullptr_t, observer_ptr<T> ptr) noexcept -> bool
{
    return bool(ptr);
}

template <typename T, typename U>
constexpr auto
operator<=>(observer_ptr<T> lhs, observer_ptr<U> rhs) -> std::strong_ordering
{
    using common_type = typename std::common_type<typename std::add_pointer<T>::type, typename std::add_pointer<U>::type>::type;
    return static_cast<common_type>(lhs.get()) <=> static_cast<common_type>(rhs.get());
}

} // namespace abc

namespace std
{

template <typename T>
struct hash<abc::observer_ptr<T>>
{
    using result_type = std::size_t;
    using argument_type = abc::observer_ptr<T>;

    constexpr auto
    operator()(abc::observer_ptr<T> const & ptr) const noexcept(noexcept(std::hash<typename add_pointer<T>::type>{}(ptr.get()))) -> std::size_t
    {
        return std::hash<typename std::add_pointer<T>::type>{}(ptr.get());
    }
};

} // namespace std

#endif // ABC_INCLUDE_ABC_MEMORY
