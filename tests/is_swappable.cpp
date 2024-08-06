// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/type_traits.h>

#include <gtest/gtest.h>

#include <cstdint>
#include <type_traits>

#define ENSURE_SWAPPABLE(T) static_assert(abc::is_swappable_v<T>, "abc::is_swappable_v<" #T "> == false")

#define ENSURE_NOTHROW_SWAPPABLE(T) static_assert(abc::is_nothrow_swappable_v<T>, "abc::is_nothrow_swappable_v<" #T "> == false")

#define ENSURE_NOT_SWAPPABLE(T) static_assert(!abc::is_swappable_v<T>, "abc::is_swappable_v<" #T "> == true")

#define ENSURE_NOT_NOTHROW_SWAPPABLE(T) static_assert(!abc::is_nothrow_swappable_v<T>, "abc::is_nothrow_swappable_v<" #T "> == true")

#define SWAPPABLE_AS_STD(T)                                                                                                                                                        \
    static_assert(abc::is_swappable_v<T> == std::is_swappable_v<T>, "abc::is_swappable_v<" #T "> != std::is_swappable_v<" #T ">");                                                 \
    static_assert(abc::is_nothrow_swappable_v<T> == std::is_nothrow_swappable_v<T>, "abc::is_nothrow_swappable_v<" #T "> != std::is_nothrow_swappable_v<" #T ">")

namespace swappable
{
struct A
{
};

TEST(is_swappable, std_swap1)
{
    static_assert(abc::details::is_swappable_impl<A>::use_std_swap_v);
    static_assert(!abc::details::is_swappable_impl<A>::use_adl_swap_v);
    static_assert(abc::is_swappable_v<A>);
    static_assert(abc::is_swappable_v<A &>);
    static_assert(abc::is_swappable_v<A &&>);
    EXPECT_TRUE(abc::is_swappable_v<A>);

    static_assert(abc::details::is_nothrow_swappable_impl<A>::use_nothrow_std_swap_v);
    static_assert(!abc::details::is_nothrow_swappable_impl<A>::use_nothrow_adl_swap_v);
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
    auto
    operator=(B const &) -> B & = delete;

    B(B &&) = default;
    auto
    operator=(B &&) -> B & = default;
};

TEST(is_swappable, std_swap2)
{
    static_assert(!abc::details::is_swappable_impl<B>::use_adl_swap_v);
    static_assert(abc::details::is_swappable_impl<B>::use_std_swap_v);

    static_assert(abc::is_swappable_v<B>);
    static_assert(abc::is_swappable_v<B &>);
    static_assert(abc::is_swappable_v<B &&>);
    EXPECT_TRUE(abc::is_swappable_v<B>);

    static_assert(!abc::details::is_nothrow_swappable_impl<B>::use_nothrow_adl_swap_v);
    static_assert(abc::details::is_nothrow_swappable_impl<B>::use_nothrow_std_swap_v);
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
    ~
    E() = default;
};

TEST(is_swappable, std_swap3)
{
    static_assert(!abc::details::is_swappable_impl<E>::use_adl_swap_v);
    static_assert(abc::details::is_swappable_impl<E>::use_std_swap_v);
    static_assert(abc::is_swappable_v<E>);
    static_assert(abc::is_swappable_v<E &>);
    static_assert(abc::is_swappable_v<E &&>);

    static_assert(!abc::details::is_nothrow_swappable_impl<E>::use_nothrow_adl_swap_v);
    static_assert(abc::details::is_nothrow_swappable_impl<E>::use_nothrow_std_swap_v);
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
    auto
    operator=(C const &) -> C & = delete;

    friend void
    swap(C & /* other */, C &) noexcept
    {
    }
};

TEST(is_swappable, adl_swap1)
{
    static_assert(!abc::details::is_swappable_impl<C>::use_std_swap_v);
    static_assert(abc::details::is_swappable_impl<C>::use_adl_swap_v);
    static_assert(abc::is_swappable_v<C>);
    static_assert(abc::is_swappable_v<C &>);
    static_assert(abc::is_swappable_v<C &&>);
    EXPECT_TRUE(abc::is_swappable_v<C>);

    static_assert(!abc::details::is_nothrow_swappable_impl<C>::use_nothrow_std_swap_v);
    static_assert(abc::details::is_nothrow_swappable_impl<C>::use_nothrow_adl_swap_v);
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
    auto
    operator=(D const &) -> D & = delete;
};

void
swap(D & /* other */, D &) noexcept
{
}

TEST(is_swappable, adl_swap2)
{
    static_assert(!abc::details::is_swappable_impl<D>::use_std_swap_v);
    static_assert(abc::details::is_swappable_impl<D>::use_adl_swap_v);
    static_assert(abc::is_swappable_v<D>);
    static_assert(abc::is_swappable_v<D &>);
    static_assert(abc::is_swappable_v<D &&>);
    EXPECT_TRUE(abc::is_swappable_v<D>);

    static_assert(!abc::details::is_nothrow_swappable_impl<D>::use_nothrow_std_swap_v);
    static_assert(abc::details::is_nothrow_swappable_impl<D>::use_nothrow_adl_swap_v);
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
    auto
    operator=(Derived &&) -> Derived & = delete;
};

void
swap(Base & /* other */, Base &) noexcept
{
}

TEST(is_swappable, adl_swap3)
{
    static_assert(abc::details::is_swappable_impl<Derived>::use_adl_swap_v);
    static_assert(!abc::details::is_swappable_impl<Derived>::use_std_swap_v);
    static_assert(abc::is_swappable_v<Derived>);
    static_assert(abc::is_swappable_v<Derived &>);
    static_assert(abc::is_swappable_v<Derived &&>);
    EXPECT_TRUE(abc::is_swappable_v<Derived>);

    static_assert(abc::details::is_nothrow_swappable_impl<Derived>::use_nothrow_adl_swap_v);
    static_assert(!abc::details::is_nothrow_swappable_impl<Derived>::use_nothrow_std_swap_v);
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
    auto
    operator=(F &&) -> F & = delete;

    F(F const &) = delete;
    auto
    operator=(F const &) -> F & = delete;

    friend auto
    swap(F & /* other */, F &) -> void
    {
    }
};

TEST(is_swappable, adl_swap4)
{
    static_assert(abc::details::is_swappable_impl<F>::use_adl_swap_v);
    static_assert(!abc::details::is_swappable_impl<F>::use_std_swap_v);
    static_assert(abc::is_swappable_v<F>);
    static_assert(abc::is_swappable_v<F &>);
    static_assert(abc::is_swappable_v<F &&>);
    EXPECT_TRUE(abc::is_swappable_v<F>);

    static_assert(!abc::details::is_nothrow_swappable_impl<F>::use_nothrow_adl_swap_v);
    static_assert(!abc::details::is_nothrow_swappable_impl<F>::use_nothrow_std_swap_v);
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

void
swap(G &, G &)
{
}

TEST(is_swappable, adl_swap5)
{
    static_assert(abc::details::is_swappable_impl<G>::use_adl_swap_v);
    static_assert(!abc::details::is_swappable_impl<G>::use_std_swap_v);
    static_assert(abc::is_swappable_v<G>);
    static_assert(abc::is_swappable_v<G &>);
    static_assert(abc::is_swappable_v<G &&>);
    EXPECT_TRUE(abc::is_swappable_v<G>);

    static_assert(!abc::details::is_nothrow_swappable_impl<G>::use_nothrow_adl_swap_v);
    static_assert(!abc::details::is_nothrow_swappable_impl<G>::use_nothrow_std_swap_v);
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
    A(A const &) = delete;
    A(A &&) = delete;
    A &
    operator=(A const &) = delete;
    A &
    operator=(A &&) = delete;
};

struct B
{
    B(B &)
    {
    }
};

struct C
{
    C(C &&) = delete;

    auto
    operator=(C &&) noexcept -> C &
    {
        return *this;
    }
};

struct D
{
    D(D &&) noexcept
    {
    }

    auto
    operator=(D &&) = delete;
};

struct E
{
    E(E const &) = delete;
    auto
    operator=(E const &) -> E & = delete;

    void
    swap(E & /* other */) noexcept
    {
    }
};

struct G
{
    G(G &&) = delete;
    auto
    operator=(G &&) -> G & = delete;
};

struct ADLOnlySwappableBase
{
    ADLOnlySwappableBase(ADLOnlySwappableBase &&) = delete;
    auto
    operator=(ADLOnlySwappableBase &&) -> ADLOnlySwappableBase & = delete;

    ADLOnlySwappableBase(G &&)
    {
    }

    auto
    operator=(G &&) -> ADLOnlySwappableBase &
    {
        return *this;
    }

    ADLOnlySwappableBase(G &)
    {
    }

    auto
    operator=(G &) -> ADLOnlySwappableBase &
    {
        return *this;
    }

    friend void
    swap(ADLOnlySwappableBase & /* other */, ADLOnlySwappableBase &) noexcept
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

#define SWAPPABLE_FUNDAMENTAL_TYPE_TEST_CASE(T)                                                                                                                                    \
    ENSURE_SWAPPABLE(T);                                                                                                                                                           \
    ENSURE_SWAPPABLE(T &);                                                                                                                                                         \
    ENSURE_SWAPPABLE(T &&);                                                                                                                                                        \
    ENSURE_SWAPPABLE(T volatile);                                                                                                                                                  \
    ENSURE_SWAPPABLE(T volatile &);                                                                                                                                                \
    ENSURE_SWAPPABLE(T volatile &&);                                                                                                                                               \
    ENSURE_NOTHROW_SWAPPABLE(T);                                                                                                                                                   \
    ENSURE_NOTHROW_SWAPPABLE(T &);                                                                                                                                                 \
    ENSURE_NOTHROW_SWAPPABLE(T &&);                                                                                                                                                \
    ENSURE_NOTHROW_SWAPPABLE(T volatile);                                                                                                                                          \
    ENSURE_NOTHROW_SWAPPABLE(T volatile &);                                                                                                                                        \
    ENSURE_NOTHROW_SWAPPABLE(T volatile &&);                                                                                                                                       \
    ENSURE_NOT_SWAPPABLE(T const);                                                                                                                                                 \
    ENSURE_NOT_SWAPPABLE(T const &);                                                                                                                                               \
    ENSURE_NOT_SWAPPABLE(T const &&);                                                                                                                                              \
    ENSURE_NOT_NOTHROW_SWAPPABLE(T const);                                                                                                                                         \
    ENSURE_NOT_NOTHROW_SWAPPABLE(T const &);                                                                                                                                       \
    ENSURE_NOT_NOTHROW_SWAPPABLE(T const &&)

#define SWAPPABLE_STD_TEST_CASE(T)                                                                                                                                                 \
    SWAPPABLE_AS_STD(T);                                                                                                                                                           \
    SWAPPABLE_AS_STD(T &);                                                                                                                                                         \
    SWAPPABLE_AS_STD(T &&);                                                                                                                                                        \
    SWAPPABLE_AS_STD(T volatile);                                                                                                                                                  \
    SWAPPABLE_AS_STD(T volatile &);                                                                                                                                                \
    SWAPPABLE_AS_STD(T volatile &&);                                                                                                                                               \
    SWAPPABLE_AS_STD(T const);                                                                                                                                                     \
    SWAPPABLE_AS_STD(T const &);                                                                                                                                                   \
    SWAPPABLE_AS_STD(T const &&)

TEST(is_swappable, fundamental)
{
    SWAPPABLE_FUNDAMENTAL_TYPE_TEST_CASE(std::int8_t);
    SWAPPABLE_FUNDAMENTAL_TYPE_TEST_CASE(std::uint8_t);
    SWAPPABLE_FUNDAMENTAL_TYPE_TEST_CASE(std::int16_t);
    SWAPPABLE_FUNDAMENTAL_TYPE_TEST_CASE(std::uint16_t);
    SWAPPABLE_FUNDAMENTAL_TYPE_TEST_CASE(std::int32_t);
    SWAPPABLE_FUNDAMENTAL_TYPE_TEST_CASE(std::uint32_t);
    SWAPPABLE_FUNDAMENTAL_TYPE_TEST_CASE(std::int64_t);
    SWAPPABLE_FUNDAMENTAL_TYPE_TEST_CASE(std::uint64_t);
#if defined(ABC_CXX17)
    SWAPPABLE_STD_TEST_CASE(std::int8_t);
    SWAPPABLE_STD_TEST_CASE(std::uint8_t);
    SWAPPABLE_STD_TEST_CASE(std::int16_t);
    SWAPPABLE_STD_TEST_CASE(std::uint16_t);
    SWAPPABLE_STD_TEST_CASE(std::int32_t);
    SWAPPABLE_STD_TEST_CASE(std::uint32_t);
    SWAPPABLE_STD_TEST_CASE(std::int64_t);
    SWAPPABLE_STD_TEST_CASE(std::uint64_t);
#endif
}

#define SWAPPABLE_STL_TYPE_TEST_CASE(T)                                                                                                                                            \
    ENSURE_SWAPPABLE(T);                                                                                                                                                           \
    ENSURE_SWAPPABLE(T &);                                                                                                                                                         \
    ENSURE_SWAPPABLE(T &&);                                                                                                                                                        \
    ENSURE_NOT_SWAPPABLE(T volatile);                                                                                                                                              \
    ENSURE_NOT_SWAPPABLE(T volatile &);                                                                                                                                            \
    ENSURE_NOT_SWAPPABLE(T volatile &&);                                                                                                                                           \
    ENSURE_NOTHROW_SWAPPABLE(T);                                                                                                                                                   \
    ENSURE_NOTHROW_SWAPPABLE(T &);                                                                                                                                                 \
    ENSURE_NOTHROW_SWAPPABLE(T &&);                                                                                                                                                \
    ENSURE_NOT_NOTHROW_SWAPPABLE(T volatile);                                                                                                                                      \
    ENSURE_NOT_NOTHROW_SWAPPABLE(T volatile &);                                                                                                                                    \
    ENSURE_NOT_NOTHROW_SWAPPABLE(T volatile &&);                                                                                                                                   \
    ENSURE_NOT_SWAPPABLE(T const);                                                                                                                                                 \
    ENSURE_NOT_SWAPPABLE(T const &);                                                                                                                                               \
    ENSURE_NOT_SWAPPABLE(T const &&);                                                                                                                                              \
    ENSURE_NOT_NOTHROW_SWAPPABLE(T const);                                                                                                                                         \
    ENSURE_NOT_NOTHROW_SWAPPABLE(T const &);                                                                                                                                       \
    ENSURE_NOT_NOTHROW_SWAPPABLE(T const &&)

#include <array>
#include <list>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

TEST(is_swappable, stl_type)
{
    using array_t = std::array<int, 2>;
    using map_t = std::map<int, int>;
    using unordered_map_t = std::unordered_map<int, int>;

#if defined(ABC_CXX17)
    SWAPPABLE_STD_TEST_CASE(std::string);
    SWAPPABLE_STD_TEST_CASE(std::wstring);
    SWAPPABLE_STD_TEST_CASE(std::vector<int>);
    SWAPPABLE_STD_TEST_CASE(array_t);
    SWAPPABLE_STD_TEST_CASE(map_t);
    SWAPPABLE_STD_TEST_CASE(unordered_map_t);
    SWAPPABLE_STD_TEST_CASE(std::set<int>);
    SWAPPABLE_STD_TEST_CASE(std::unordered_set<int>);
    SWAPPABLE_STD_TEST_CASE(std::set<std::string>);
    SWAPPABLE_STD_TEST_CASE(std::unordered_set<std::string>);
    // bug: std::string_view fails here.
    // SWAPPABLE_STD_TEST_CASE(std::string_view);
    // SWAPPABLE_STD_TEST_CASE(std::wstring_view);
    SWAPPABLE_STD_TEST_CASE(decltype("xx"));
    SWAPPABLE_STD_TEST_CASE(std::list<std::string_view>);
    std::string s1[5];
    SWAPPABLE_STD_TEST_CASE(decltype(s1));
#endif
}

#define SWAPPABLE_W_VS_STD_SWAPPABLE(T)                                                                                                                                            \
    static_assert(abc::is_swappable_with_v<T, T> == std::is_swappable_with_v<T, T>, "abc::is_swappable_with_v<" #T ", " #T "> != std::is_swappable_with_v<" #T ", " #T ">");       \
    static_assert(abc::is_nothrow_swappable_with_v<T, T> == std::is_nothrow_swappable_with_v<T, T>,                                                                                \
                  "abc::is_nothrow_swappable_with_v<" #T ", " #T "> != std::is_nothrow_swappable_with_v<" #T ", " #T ">");                                                         \
    static_assert(abc::is_swappable_with_v<T &, T &> == std::is_swappable_with_v<T &, T &>,                                                                                        \
                  "abc::is_swappable_with_v<" #T " &, " #T " &> != std::is_swappable_with_v<" #T " &, " #T " &>");                                                                 \
    static_assert(abc::is_nothrow_swappable_with_v<T &, T &> == std::is_nothrow_swappable_with_v<T &, T &>,                                                                        \
                  "abc::is_nothrow_swappable_with_v<" #T " &, " #T " &> != std::is_nothrow_swappable_with_v<" #T " &, " #T " &>");                                                 \
    static_assert(abc::is_swappable_with_v<T &&, T &&> == std::is_swappable_with_v<T &&, T &&>,                                                                                    \
                  "abc::is_swappable_with_v<" #T " &&, " #T " &&> != std::is_swappable_with_v<" #T " &&, " #T " &&>");                                                             \
    static_assert(abc::is_nothrow_swappable_with_v<T &&, T &&> == std::is_nothrow_swappable_with_v<T &&, T &&>,                                                                    \
                  "abc::is_nothrow_swappable_with_v<" #T " &&, " #T " &&> != std::is_nothrow_swappable_with_v<" #T " &&, " #T " &&>");                                             \
    static_assert(abc::is_swappable_with_v<T const, T const> == std::is_swappable_with_v<T const, T const>,                                                                        \
                  "abc::is_swappable_with_v<" #T " const, " #T " const> != std::is_swappable_with_v<" #T " const, " #T " const>");                                                 \
    static_assert(abc::is_nothrow_swappable_with_v<T const, T const> == std::is_nothrow_swappable_with_v<T const, T const>,                                                        \
                  "abc::is_nothrow_swappable_with_v<" #T " const, " #T " const> != std::is_nothrow_swappable_with_v<" #T " const, " #T " const>");                                 \
    static_assert(abc::is_swappable_with_v<T const &, T const &> == std::is_swappable_with_v<T const &, T const &>,                                                                \
                  "abc::is_swappable_with_v<" #T " const &, " #T " const &> != std::is_swappable_with_v<" #T " const &, " #T " const &>");                                         \
    static_assert(abc::is_nothrow_swappable_with_v<T const &, T const &> == std::is_nothrow_swappable_with_v<T const &, T const &>,                                                \
                  "abc::is_nothrow_swappable_with_v<" #T " const &, " #T " const &> != std::is_nothrow_swappable_with_v<" #T " const &, " #T " const &>");                         \
    static_assert(abc::is_swappable_with_v<T const &&, T const &&> == std::is_swappable_with_v<T const &&, T const &&>,                                                            \
                  "abc::is_swappable_with_v<" #T " const &&, " #T " const &&> != std::is_swappable_with_v<" #T " const &&, " #T " const &&>");                                     \
    static_assert(abc::is_nothrow_swappable_with_v<T const &&, T const &&> == std::is_nothrow_swappable_with_v<T const &&, T const &&>,                                            \
                  "abc::is_nothrow_swappable_with_v<" #T " const &&, " #T " const &&> != std::is_nothrow_swappable_with_v<" #T " const &&, " #T " const &&>");                     \
    static_assert(abc::is_swappable_with_v<T volatile, T volatile> == std::is_swappable_with_v<T volatile, T volatile>,                                                            \
                  "abc::is_swappable_with_v<" #T " volatile, " #T " volatile> != std::is_swappable_with_v<" #T " volatile, " #T " volatile>");                                     \
    static_assert(abc::is_nothrow_swappable_with_v<T volatile, T volatile> == std::is_nothrow_swappable_with_v<T volatile, T volatile>,                                            \
                  "abc::is_nothrow_swappable_with_v<" #T " volatile, " #T " volatile> != std::is_nothrow_swappable_with_v<" #T " volatile, " #T " volatile>");                     \
    static_assert(abc::is_swappable_with_v<T volatile &, T volatile &> == std::is_swappable_with_v<T volatile &, T volatile &>,                                                    \
                  "abc::is_swappable_with_v<" #T " volatile &, " #T " volatile &> != std::is_swappable_with_v<" #T " volatile &, " #T " volatile &>");                             \
    static_assert(abc::is_nothrow_swappable_with_v<T volatile &, T volatile &> == std::is_nothrow_swappable_with_v<T volatile &, T volatile &>,                                    \
                  "abc::is_nothrow_swappable_with_v<" #T " volatile &, " #T " volatile &> != std::is_nothrow_swappable_with_v<" #T " volatile &, " #T " volatile &>");             \
    static_assert(abc::is_swappable_with_v<T volatile &&, T volatile &&> == std::is_swappable_with_v<T volatile &&, T volatile &&>,                                                \
                  "abc::is_swappable_with_v<" #T " volatile &&, " #T " volatile &&> != std::is_swappable_with_v<" #T " volatile &&, " #T " volatile &&>");                         \
    static_assert(abc::is_nothrow_swappable_with_v<T volatile &&, T volatile &&> == std::is_nothrow_swappable_with_v<T volatile &&, T volatile &&>,                                \
                  "abc::is_nothrow_swappable_with_v<" #T " volatile &&, " #T " volatile &&> != std::is_nothrow_swappable_with_v<" #T " volatile &&, " #T " volatile &&>");         \
    static_assert(abc::is_swappable_with_v<T const volatile, T const volatile> == std::is_swappable_with_v<T const volatile, T const volatile>,                                    \
                  "abc::is_swappable_with_v<" #T " const volatile, " #T " const volatile> != std::is_swappable_with_v<" #T " const volatile, " #T " const volatile>");             \
    static_assert(abc::is_nothrow_swappable_with_v<T const volatile, T const volatile> == std::is_nothrow_swappable_with_v<T const volatile, T const volatile>,                    \
                  "abc::is_nothrow_swappable_with_v<" #T " const volatile, " #T " const volatile> != std::is_nothrow_swappable_with_v<" #T " const volatile, " #T                  \
                  " const volatile>");                                                                                                                                             \
    static_assert(abc::is_swappable_with_v<T const volatile &, T const volatile &> == std::is_swappable_with_v<T const volatile &, T const volatile &>,                            \
                  "abc::is_swappable_with_v<" #T " const volatile &, " #T " const volatile &> != std::is_swappable_with_v<" #T " const volatile &, " #T " const volatile &>");     \
    static_assert(abc::is_nothrow_swappable_with_v<T const volatile &, T const volatile &> == std::is_nothrow_swappable_with_v<T const volatile &, T const volatile &>,            \
                  "abc::is_nothrow_swappable_with_v<" #T " const volatile &, " #T " const volatile &> != std::is_nothrow_swappable_with_v<" #T " const volatile &, " #T            \
                  " const volatile &>");                                                                                                                                           \
    static_assert(abc::is_swappable_with_v<T const volatile &&, T const volatile &&> == std::is_swappable_with_v<T const volatile &&, T const volatile &&>,                        \
                  "abc::is_swappable_with_v<" #T " const volatile &&, " #T " const volatile &&> != std::is_swappable_with_v<" #T " const volatile &&, " #T " const volatile &&>"); \
    static_assert(abc::is_nothrow_swappable_with_v<T const volatile &&, T const volatile &&> == std::is_nothrow_swappable_with_v<T const volatile &&, T const volatile &&>,        \
                  "abc::is_nothrow_swappable_with_v<" #T " const volatile &&, " #T " const volatile &&> != std::is_nothrow_swappable_with_v<" #T " const volatile &&, " #T         \
                  " const volatile &&>")

TEST(is_swappable_with, fundamental)
{
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::int8_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::uint8_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::int16_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::uint16_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::int32_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::uint32_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::int64_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::uint64_t);
}

TEST(is_swappable_with, stl_type)
{
    using array_t = std::array<int, 2>;
    using map_t = std::map<int, int>;
    using unordered_map_t = std::unordered_map<int, int>;

    SWAPPABLE_W_VS_STD_SWAPPABLE(std::string);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::wstring);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::vector<int>);
    SWAPPABLE_W_VS_STD_SWAPPABLE(array_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(map_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(unordered_map_t);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::set<int>);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::unordered_set<int>);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::set<std::string>);
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::unordered_set<std::string>);
    // bug: std::string_view fails here.
    // SWAPPABLE_W_VS_STD_SWAPPABLE(std::string_view);
    // SWAPPABLE_W_VS_STD_SWAPPABLE(std::wstring_view);
    SWAPPABLE_W_VS_STD_SWAPPABLE(decltype("xx"));
    SWAPPABLE_W_VS_STD_SWAPPABLE(std::list<std::string_view>);
    std::string s1[5];
    SWAPPABLE_W_VS_STD_SWAPPABLE(decltype(s1));
}
