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

TEST(is_swappable, std_swap1)
{
    static_assert(abc::is_swappable<A>::can_swap_v);
    static_assert(abc::is_swappable<A>::can_std_swap_v);
    static_assert(!abc::is_swappable<A>::use_adl_swap_v);
    static_assert(abc::is_swappable_v<A>);
    static_assert(abc::is_swappable_v<A &>);
    static_assert(abc::is_swappable_v<A &&>);
    EXPECT_TRUE(abc::is_swappable_v<A>);

    static_assert(abc::is_nothrow_swappable<A>::can_nothrow_swap_v);
    static_assert(abc::is_nothrow_swappable<A>::can_nothrow_std_swap_v);
    static_assert(!abc::is_nothrow_swappable<A>::use_nothrow_adl_swap_v);
    static_assert(abc::is_nothrow_swappable_v<A>);
    static_assert(abc::is_nothrow_swappable_v<A &>);
    static_assert(abc::is_nothrow_swappable_v<A &&>);
    EXPECT_TRUE(abc::is_nothrow_swappable_v<A>);

#if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<A>);
    EXPECT_TRUE(std::is_swappable_v<A>);

    static_assert(std::is_nothrow_swappable_v<A>);
    EXPECT_TRUE(std::is_nothrow_swappable_v<A>);
#endif
}

struct B
{
    B(B const &) = delete;
    auto operator=(B const &) -> B & = delete;

    B(B &&) = default;
    auto operator=(B &&) -> B & = default;
};
TEST(is_swappable, std_swap2)
{
    static_assert(abc::is_swappable<B>::can_swap_v);
    static_assert(!abc::is_swappable<B>::use_adl_swap_v);
    static_assert(abc::is_swappable<B>::can_std_swap_v);

    static_assert(abc::is_swappable_v<B>);
    static_assert(abc::is_swappable_v<B &>);
    static_assert(abc::is_swappable_v<B &&>);
    EXPECT_TRUE(abc::is_swappable_v<B>);

    static_assert(abc::is_nothrow_swappable<B>::can_nothrow_swap_v);
    static_assert(!abc::is_nothrow_swappable<B>::use_nothrow_adl_swap_v);
    static_assert(abc::is_nothrow_swappable<B>::can_nothrow_std_swap_v);
    static_assert(abc::is_nothrow_swappable_v<B>);
    static_assert(abc::is_nothrow_swappable_v<B &>);
    static_assert(abc::is_nothrow_swappable_v<B &&>);
#if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<B>);
    static_assert(std::is_swappable_v<B &>);
    static_assert(std::is_swappable_v<B &&>);
    EXPECT_TRUE(std::is_swappable_v<B>);

    static_assert(std::is_nothrow_swappable_v<B>);
    static_assert(std::is_nothrow_swappable_v<B &>);
    static_assert(std::is_nothrow_swappable_v<B &&>);
#endif
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

TEST(is_swappable, std_swap3)
{
    static_assert(abc::is_swappable<E>::can_swap_v);
    static_assert(!abc::is_swappable<E>::use_adl_swap_v);
    static_assert(abc::is_swappable<E>::can_std_swap_v);
    static_assert(abc::is_swappable_v<E>);
    static_assert(abc::is_swappable_v<E &>);
    static_assert(abc::is_swappable_v<E &&>);

    static_assert(abc::is_nothrow_swappable<E>::can_nothrow_swap_v);
    static_assert(!abc::is_nothrow_swappable<E>::use_nothrow_adl_swap_v);
    static_assert(abc::is_nothrow_swappable<E>::can_nothrow_std_swap_v);
    static_assert(abc::is_nothrow_swappable_v<E>);
    static_assert(abc::is_nothrow_swappable_v<E &>);
    static_assert(abc::is_nothrow_swappable_v<E &&>);

    EXPECT_TRUE(abc::is_swappable_v<E>);
    EXPECT_TRUE(abc::is_nothrow_swappable_v<E>);
#if defined(ABC_CXX17)
    static_assert(std::is_move_constructible_v<E>);
    static_assert(std::is_move_assignable_v<E>);

    static_assert(std::is_swappable_v<E>);
    static_assert(std::is_swappable_v<E &>);
    static_assert(std::is_swappable_v<E &&>);

    static_assert(std::is_nothrow_swappable_v<E>);
    static_assert(std::is_nothrow_swappable_v<E &>);
    static_assert(std::is_nothrow_swappable_v<E &&>);
#endif
}

struct C
{
    C(C const &) = delete;
    auto operator=(C const &) -> C & = delete;

    friend void swap(C & /* other */, C &) noexcept
    {
    }
};
TEST(is_swappable, adl_swap1)
{
    static_assert(abc::is_swappable<C>::can_swap_v);
    static_assert(!abc::is_swappable<C>::can_std_swap_v);
    static_assert(abc::is_swappable<C>::use_adl_swap_v);
    static_assert(abc::is_swappable_v<C>);
    static_assert(abc::is_swappable_v<C &>);
    static_assert(abc::is_swappable_v<C &&>);
    EXPECT_TRUE(abc::is_swappable_v<C>);

    static_assert(abc::is_nothrow_swappable<C>::can_nothrow_swap_v);
    static_assert(!abc::is_nothrow_swappable<C>::can_nothrow_std_swap_v);
    static_assert(abc::is_nothrow_swappable<C>::use_nothrow_adl_swap_v);
    static_assert(abc::is_nothrow_swappable_v<C>);
    static_assert(abc::is_nothrow_swappable_v<C &>);
    static_assert(abc::is_nothrow_swappable_v<C &&>);

#if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<C>);
    static_assert(std::is_swappable_v<C &>);
    static_assert(std::is_swappable_v<C &&>);

    static_assert(std::is_nothrow_swappable_v<C>);
    static_assert(std::is_nothrow_swappable_v<C &>);
    static_assert(std::is_nothrow_swappable_v<C &&>);
#endif
}

struct D
{
    D(D const &) = delete;
    auto operator=(D const &) -> D & = delete;
};

void swap(D & /* other */, D &) noexcept
{
}

TEST(is_swappable, adl_swap2)
{
    static_assert(abc::is_swappable<D>::can_swap_v);
    static_assert(!abc::is_swappable<D>::can_std_swap_v);
    static_assert(abc::is_swappable<D>::use_adl_swap_v);
    static_assert(abc::is_swappable_v<D>);
    static_assert(abc::is_swappable_v<D &>);
    static_assert(abc::is_swappable_v<D &&>);
    EXPECT_TRUE(abc::is_swappable_v<D>);

    static_assert(abc::is_nothrow_swappable<D>::can_nothrow_swap_v);
    static_assert(!abc::is_nothrow_swappable<D>::can_nothrow_std_swap_v);
    static_assert(abc::is_nothrow_swappable<D>::use_nothrow_adl_swap_v);
    static_assert(abc::is_nothrow_swappable_v<D>);
    static_assert(abc::is_nothrow_swappable_v<D &>);
    static_assert(abc::is_nothrow_swappable_v<D &&>);
    EXPECT_TRUE(abc::is_nothrow_swappable_v<D>);
#if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<D>);
    static_assert(std::is_swappable_v<D &>);
    static_assert(std::is_swappable_v<D &&>);

    static_assert(std::is_nothrow_swappable_v<D>);
    static_assert(std::is_nothrow_swappable_v<D &>);
    static_assert(std::is_nothrow_swappable_v<D &&>);
#endif
}

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

TEST(is_swappable, adl_swap3)
{
    static_assert(abc::is_swappable<Derived>::can_swap_v);
    static_assert(abc::is_swappable<Derived>::use_adl_swap_v);
    static_assert(!abc::is_swappable<Derived>::can_std_swap_v);
    static_assert(abc::is_swappable_v<Derived>);
    static_assert(abc::is_swappable_v<Derived &>);
    static_assert(abc::is_swappable_v<Derived &&>);
    EXPECT_TRUE(abc::is_swappable_v<Derived>);

    static_assert(abc::is_nothrow_swappable<Derived>::can_nothrow_swap_v);
    static_assert(abc::is_nothrow_swappable<Derived>::use_nothrow_adl_swap_v);
    static_assert(!abc::is_nothrow_swappable<Derived>::can_nothrow_std_swap_v);
    static_assert(abc::is_nothrow_swappable_v<Derived>);
    static_assert(abc::is_nothrow_swappable_v<Derived &>);
    static_assert(abc::is_nothrow_swappable_v<Derived &&>);
#if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<Derived>);
    static_assert(std::is_swappable_v<Derived &>);
    static_assert(std::is_swappable_v<Derived &&>);
    EXPECT_TRUE(std::is_swappable_v<Derived>);
    static_assert(std::is_nothrow_swappable_v<Derived>);
    static_assert(std::is_nothrow_swappable_v<Derived &>);
    static_assert(std::is_nothrow_swappable_v<Derived &&>);
#endif
}

struct F
{
    F(F &&) = delete;
    auto operator=(F &&) -> F & = delete;

    F(F const &) = delete;
    auto operator=(F const &) -> F & = delete;

    friend auto swap(F & /* other */, F &) -> void
    {
    }
};

TEST(is_swappable, adl_swap4)
{
    static_assert(abc::is_swappable<F>::can_swap_v);
    static_assert(abc::is_swappable<F>::use_adl_swap_v);
    static_assert(!abc::is_swappable<F>::can_std_swap_v);
    static_assert(abc::is_swappable_v<F>);
    static_assert(abc::is_swappable_v<F &>);
    static_assert(abc::is_swappable_v<F &&>);
    EXPECT_TRUE(abc::is_swappable_v<F>);

    static_assert(!abc::is_nothrow_swappable<F>::can_nothrow_swap_v);
    static_assert(!abc::is_nothrow_swappable<F>::use_nothrow_adl_swap_v);
    static_assert(!abc::is_nothrow_swappable<F>::can_nothrow_std_swap_v);
    static_assert(!abc::is_nothrow_swappable_v<F>);
    static_assert(!abc::is_nothrow_swappable_v<F &>);
    static_assert(!abc::is_nothrow_swappable_v<F &&>);

#if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<F>);
    static_assert(std::is_swappable_v<F &>);
    static_assert(std::is_swappable_v<F &&>);

    EXPECT_TRUE(std::is_swappable_v<F>);
    static_assert(!std::is_nothrow_swappable_v<F>);
    static_assert(!std::is_nothrow_swappable_v<F &>);
    static_assert(!std::is_nothrow_swappable_v<F &&>);
#endif
}

struct G
{
};
void swap(G &, G &)
{
}

TEST(is_swappable, adl_swap5)
{
    static_assert(abc::is_swappable<G>::can_swap_v);
    static_assert(abc::is_swappable<G>::use_adl_swap_v);
    static_assert(abc::is_swappable<G>::can_std_swap_v);
    static_assert(abc::is_swappable_v<G>);
    static_assert(abc::is_swappable_v<G &>);
    static_assert(abc::is_swappable_v<G &&>);
    EXPECT_TRUE(abc::is_swappable_v<G>);

    static_assert(!abc::is_nothrow_swappable<G>::can_nothrow_swap_v);
    static_assert(!abc::is_nothrow_swappable<G>::use_nothrow_adl_swap_v);
    static_assert(!abc::is_nothrow_swappable<G>::can_nothrow_std_swap_v);
    static_assert(!abc::is_nothrow_swappable_v<G>);
    static_assert(!abc::is_nothrow_swappable_v<G &>);
    static_assert(!abc::is_nothrow_swappable_v<G &&>);
    EXPECT_TRUE(!abc::is_nothrow_swappable_v<G>);

#if defined(ABC_CXX17)
    static_assert(std::is_swappable_v<G>);
    static_assert(std::is_swappable_v<G &>);
    static_assert(std::is_swappable_v<G &&>);
    static_assert(!std::is_nothrow_swappable_v<G>);
    static_assert(!std::is_nothrow_swappable_v<G &>);
    static_assert(!std::is_nothrow_swappable_v<G &&>);
#endif
}

} // namespace swappable

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

#include <string>

TEST(is_swappable, stl_type)
{
    static_assert(abc::is_swappable_v<std::string>);
    static_assert(abc::is_swappable_v<std::string &>);
    static_assert(abc::is_swappable_v<std::string &&>);
}