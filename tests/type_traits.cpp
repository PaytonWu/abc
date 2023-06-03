// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/type_traits.h"

#include <gtest/gtest.h>

#include <type_traits>

struct A {
    int _;
};
struct B {
    int _;
    A __;
};
struct C {
    int _;
    B __;
};

TEST(type_traits, conditional) {
    using namespace abc;

    static_assert(std::is_same_v<conditional_t<true, int, double>, int>);
    static_assert(std::is_same_v<conditional_t<false, int, double>, double>);
}

TEST(type_traits, type_tuple) {
    using namespace abc;

    static_assert(std::is_same_v<type_tuple<int, double>::append<char, bool>, type_tuple<int, double, char, bool>>);
    static_assert(std::is_same_v<type_tuple<int, double>::prepend<char, bool>, type_tuple<char, bool, int, double>>);
    static_assert(std::is_same_v<type_tuple<int, double>::to<std::tuple>, std::tuple<int, double>>);
    static_assert(std::is_same_v<type_tuple<int, double>::map<std::add_const>, type_tuple<int const, double const>>);
}

TEST(type_traits, type_filter) {
    using namespace abc;

    static_assert(std::is_same_v<type_filter_t<type_tuple<int, double, char>, std::is_integral>, type_tuple<int, char>>);
}

template <typename Acc, typename T>
using type_size_acc = std::integral_constant<size_t, Acc::value + sizeof(T)>;

template <abc::type_list TypeList, typename T>
using add_pointer_acc = TypeList::template append<std::add_pointer_t<T>>;

template <abc::type_list TypeList, typename T>
using is_integral_acc = abc::conditional_t<std::is_integral_v<T>, typename TypeList::template append<T> ,TypeList>;

TEST(type_traits, type_fold) {
    using namespace abc;

    static_assert(type_fold_t<type_tuple<int, double, char>, std::integral_constant<size_t, 0>, type_size_acc>::value == 13);
    static_assert(std::is_same_v<type_fold_t<type_tuple<int, double, char>, type_tuple<>, add_pointer_acc>, type_tuple<int*, double*, char*>>);
    static_assert(std::is_same_v<type_fold_t<type_tuple<int, double, char>, type_tuple<>, is_integral_acc>, type_tuple<int, char>>);
}

TEST(type_traits, type_concat) {
    using namespace abc;

    static_assert(std::is_same_v<type_concat_t<type_tuple<int, double>, type_tuple<char, bool>>, type_tuple<int, double, char, bool>>);
}

TEST(type_traits, type_contains) {
    using namespace abc;

    static_assert(type_contains_v<type_tuple<int, double, char>, int>);
    static_assert(!type_contains_v<type_tuple<int, double, char>, bool>);
}

TEST(type_traits, type_unique) {
    using namespace abc;

    static_assert(std::is_same_v<type_unique_t<type_tuple<int, double, char, int, double>>, type_tuple<int, double, char>>);
}

TEST(type_traits, type_partition) {
    using namespace abc;

    static_assert(std::is_same_v<type_partition_t<type_tuple<int, double, char, int, double>, std::is_integral>, std::pair<type_tuple<int, char, int>, type_tuple<double, double>>>);
}

template <typename T, typename U>
using type_less_cmp = std::bool_constant<(sizeof(T) < sizeof(U))>;

template <typename T, typename U>
using type_greater_cmp = std::bool_constant<(sizeof(T) > sizeof(U))>;

TEST(type_traits, type_sort) {
    using namespace abc;

    static_assert(std::is_same_v<type_sort_t<type_tuple<int, double, char, int, double>, type_less_cmp>, type_tuple<char, int, int, double, double>>);
    static_assert(std::is_same_v<type_sort_t<type_tuple<int, double, char, int, double>, type_greater_cmp>, type_tuple<double, double, int, int, char>>);

    static_assert(std::is_same_v<type_sort_t<type_tuple<int>, type_less_cmp>, type_tuple<int>>);
    static_assert(std::is_same_v<type_sort_t<type_tuple<char>, type_greater_cmp>, type_tuple<char>>);

    static_assert(std::is_same_v<type_sort_t<type_tuple<>, type_less_cmp>, type_tuple<>>);
    static_assert(std::is_same_v<type_sort_t<type_tuple<>, type_greater_cmp>, type_tuple<>>);

    static_assert(std::is_same_v<type_sort_t<type_tuple<int, struct C, char, int, double>, type_less_cmp>, type_tuple<char, int, int, double, struct C>>);
    static_assert(std::is_same_v<type_sort_t<type_tuple<int, double, char, int, struct C>, type_greater_cmp>, type_tuple<struct C, double, int, int, char>>);
}

TEST(type_traits, type_reverse) {
    using namespace abc;

    static_assert(std::is_same_v<type_reverse_t<type_tuple<int, double, char>>, type_tuple<char, double, int>>);

}