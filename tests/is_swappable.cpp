// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/type_traits.h>

#include <gtest/gtest.h>

#include <type_traits>

namespace swappable
{

struct A
{
};

struct B
{
    B(B const &) = delete;
    auto operator=(B const &) -> B & = delete;

    B(B &&) = default;
    auto operator=(B &&) -> B & = default;
};

struct C
{
    C(C const &) = delete;
    auto operator=(C const &) -> C & = delete;

    friend void swap(C & /* other */, C &) noexcept
    {
    }
};

struct D
{
    D(D const &) = delete;
    auto operator=(D const &) -> D & = delete;
};

void swap(D & /* other */, D &) noexcept
{
}

struct E
{
    // move constructor and move assignment operator are implicitly declared as deleted
    // copy constructor and copy assignment operator are implicitly declared as defaulted
    // since copy constructor and copy assignment operator accept rvalue argument, E is move constructible and move assignable.
    // std::is_move_constructible_v<E> == true && std::is_move_assignement_v<E> == true
    // https://en.cppreference.com/w/cpp/types/is_move_constructible#Example
    ~E() = default;
};

struct Base
{
};

struct Derived : Base
{
    Derived(Derived &&) = delete;
    auto operator=(Derived &&) -> Derived & = delete;
};

void swap(Base & /* other */, Base &) noexcept
{
}

} // namespace swappable

TEST(is_swappable, swappable)
{
    static_assert(abc::is_swappable_v<swappable::A>);
    EXPECT_TRUE(abc::is_swappable_v<swappable::A>);
    #if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<swappable::A>);
    EXPECT_TRUE(std::is_swappable_v<swappable::A>);
    #endif

    static_assert(abc::is_swappable_v<swappable::B>);
    EXPECT_TRUE(abc::is_swappable_v<swappable::B>);
    #if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<swappable::B>);
    EXPECT_TRUE(std::is_swappable_v<swappable::B>);
    #endif

    static_assert(abc::is_swappable_v<swappable::C>);
    EXPECT_TRUE(abc::is_swappable_v<swappable::C>);
    #if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<swappable::C>);
    EXPECT_TRUE(std::is_swappable_v<swappable::C>);
    #endif

    static_assert(abc::is_swappable_v<swappable::D>);
    EXPECT_TRUE(abc::is_swappable_v<swappable::D>);
    #if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<swappable::D>);
    EXPECT_TRUE(std::is_swappable_v<swappable::D>);
    #endif

    static_assert(std::is_move_constructible_v<swappable::E>);
    static_assert(std::is_move_assignable_v<swappable::E>);
    static_assert(abc::is_swappable_v<swappable::E>);
    EXPECT_TRUE(abc::is_swappable_v<swappable::E>);
    #if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<swappable::E>);
    EXPECT_TRUE(std::is_swappable_v<swappable::E>);
    #endif

    static_assert(abc::is_swappable_v<swappable::Derived>);
    EXPECT_TRUE(abc::is_swappable_v<swappable::Derived>);
    #if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<swappable::Derived>);
    EXPECT_TRUE(std::is_swappable_v<swappable::Derived>);
    #endif
}

namespace non_swappable
{

struct A
{
    A() = default;
    A(const A &) = delete;
    A(A &&) = delete;
    A &operator=(const A &) = delete;
    A &operator=(A &&) = delete;
};

struct B
{
    B(B &) {}
};

struct C
{
    C(C &&) = delete;
    auto operator=(C &&) noexcept -> C &
    {
        return *this;
    }
};

struct D
{
    D(D &&) noexcept {}
    auto operator=(D &&) = delete;
};

struct E
{
    E(E const &) = delete;
    auto operator=(E const &) -> E & = delete;

    void swap(E & /* other */) noexcept
    {
    }
};

struct G
{
    G(G &&) = delete;
    auto operator=(G &&) -> G & = delete;
};

struct ADLOnlySwappableBase
{
    ADLOnlySwappableBase(ADLOnlySwappableBase &&) = delete;
    auto operator=(ADLOnlySwappableBase &&) -> ADLOnlySwappableBase & = delete;

    ADLOnlySwappableBase(G &&) {}
    auto operator=(G &&) -> ADLOnlySwappableBase & { return *this; }

    ADLOnlySwappableBase(G &) {}
    auto operator=(G &) -> ADLOnlySwappableBase & { return *this; }

    friend void swap(ADLOnlySwappableBase & /* other */, ADLOnlySwappableBase &) noexcept
    {
    }
};

} // namespace non_swappable

TEST(is_swappable, non_swappable)
{
    static_assert(!abc::is_swappable_v<non_swappable::A>);
    EXPECT_FALSE(abc::is_swappable_v<non_swappable::A>);
    #if defined(ABC_CXX17)
    static_assert(!std::is_swappable_v<non_swappable::A>);
    EXPECT_FALSE(std::is_swappable_v<non_swappable::A>);
    #endif

    static_assert(!abc::is_swappable_v<non_swappable::B>);
    EXPECT_FALSE(abc::is_swappable_v<non_swappable::B>);
    #if defined(ABC_CXX17)
    static_assert(!std::is_swappable_v<non_swappable::B>);
    EXPECT_FALSE(std::is_swappable_v<non_swappable::B>);
    #endif

    static_assert(!abc::is_swappable_v<non_swappable::C>);
    EXPECT_FALSE(abc::is_swappable_v<non_swappable::C>);
    #if defined(ABC_CXX17)
    static_assert(!std::is_swappable_v<non_swappable::C>);
    EXPECT_FALSE(std::is_swappable_v<non_swappable::C>);
    #endif

    static_assert(!abc::is_swappable_v<non_swappable::D>);
    EXPECT_FALSE(abc::is_swappable_v<non_swappable::D>);
    #if defined(ABC_CXX17)
    static_assert(!std::is_swappable_v<non_swappable::D>);
    EXPECT_FALSE(std::is_swappable_v<non_swappable::D>);
    #endif

    static_assert(!abc::is_swappable_v<non_swappable::E>);
    EXPECT_FALSE(abc::is_swappable_v<non_swappable::E>);
    #if defined(ABC_CXX17)
    static_assert(!std::is_swappable_v<non_swappable::E>);
    EXPECT_FALSE(std::is_swappable_v<non_swappable::E>);
    #endif

    static_assert(!abc::is_swappable_v<non_swappable::G>);
    EXPECT_FALSE(abc::is_swappable_v<non_swappable::G>);
    #if defined(ABC_CXX17)
    static_assert(!std::is_swappable_v<non_swappable::G>);
    EXPECT_FALSE(std::is_swappable_v<non_swappable::G>);
    #endif
}

namespace nothrow_swappable
{

struct A
{
    A(A &&) = default;
    auto operator=(A &&) -> A & = default;
};

}

