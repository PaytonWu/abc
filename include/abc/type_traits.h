// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_TYPE_TRAITS)
#define ABC_TYPE_TRAITS

#pragma once

#include "abc/details/config.h"

#include <type_traits>
#include <utility>

namespace abc {

template <bool Condition>
struct conditional {
    template <typename True, typename/* False */>
    using type = True;
};

template <>
struct conditional<false> {
    template <typename/* True */, typename False>
    using type = False;
};

template <bool Condition, typename True, typename False>
using conditional_t = typename conditional<Condition>::template type<True, False>;

template <typename ... Ts>
struct type_tuple {
    struct is_type_tuple {
    };
    using type = type_tuple;

    constexpr static auto size = sizeof...(Ts);

    template <typename ... Us>
    using append = type_tuple<Ts..., Us...>;

    template <typename ... Us>
    using prepend = type_tuple<Us..., Ts...>;

    template <template <typename ...> typename Target>
    using to = Target<Ts...>;

    template <template <typename> typename TypeMapper>
    using map = type_tuple<typename TypeMapper<Ts>::type...>;

    template <template <typename> typename TypePred>
    struct filter;
};

template <typename T>
concept type_list = requires {
    typename T::is_type_tuple;
    typename T::type;
};

template <type_list TypeList, template <typename> typename TypeMapper>
struct type_map;

template <template <typename> typename TypeMapper, typename ... Ts>
struct type_map<type_tuple<Ts...>, TypeMapper> {
    using type = type_tuple<typename TypeMapper<Ts>::type...>;
};

template <type_list TypeList, template <typename> typename TypePred>
struct type_filter;

template <type_list TypeList, template <typename> typename TypePred>
using type_filter_t = type_filter<TypeList, TypePred>::type;

template <template <typename> typename TypePred>
struct type_filter<type_tuple<>, TypePred> {
    using type = type_tuple<>;
};

template <template <typename> typename TypePred, typename Head, typename ... Remain>
struct type_filter<type_tuple<Head, Remain...>, TypePred> : type_filter<type_tuple<Remain...>, TypePred> {
    using type = conditional_t<
    TypePred<Head>::value,
    typename type_filter<type_tuple<Remain...>, TypePred>::type::template prepend<Head>,
    typename type_filter<type_tuple<Remain...>, TypePred>::type>;
};

template <type_list TypeList, typename I, template <typename, typename> typename Op>
struct type_fold;

template <type_list TypeList, typename I, template <typename, typename> typename Op>
using type_fold_t = type_fold<TypeList, I, Op>::type;

template <typename I, template <typename, typename> typename Op>
struct type_fold<type_tuple<>, I, Op> {
    using type = std::type_identity_t<I>;
};

template <typename I, template <typename, typename> typename Op, typename Head, typename ... Remain>
struct type_fold<type_tuple<Head, Remain...>, I, Op> : type_fold<type_tuple<Remain...>, typename Op<I, Head>::type, Op> {
};

template <type_list ... TypeLists>
struct type_concat;

template <type_list ... TypeLists>
using type_concat_t = type_concat<TypeLists...>::type;

template <>
struct type_concat<> {
    using type = type_tuple<>;
};

template <type_list TypeList>
struct type_concat<TypeList> {
    using type = TypeList;
};

template <type_list T1, type_list T2>
struct type_concat<T1, T2> {
private:
    template <type_list AccT, typename T>
    using Acc = typename AccT::template append<T>;
public:
    using type = type_fold_t<T2, T1, Acc>;
};

template <type_list T1, type_list T2, type_list ... Remains>
struct type_concat<T1, T2, Remains...> : type_concat<typename type_concat<T1, T2>::type, Remains...> {
};

template <type_list TypeList, typename T>
struct type_contains;

template <type_list TypeList, typename T>
constexpr inline bool type_contains_v = type_contains<TypeList, T>::value;

template <typename T>
struct type_contains<type_tuple<>, T> : std::false_type {
};

template <typename T, typename Head, typename ... Remains>
struct type_contains<type_tuple<Head, Remains...>, T> : std::conditional_t<std::is_same_v<T, Head>, std::true_type, type_contains<type_tuple<Remains...>, T>> {
};

template <type_list TypeList>
struct type_unique {
private:
    template <type_list TypeListAcc, typename T>
    using Acc = conditional_t<type_contains_v<TypeListAcc, T>, TypeListAcc, typename TypeListAcc::template append<T>>;
public:
    using type = type_fold_t<TypeList, type_tuple<>, Acc>;
};

template <type_list TypeList>
using type_unique_t = typename type_unique<TypeList>::type;

template <type_list TypeList, template <typename> typename TypePred>
struct type_partition {
private:
    template <typename T>
    using PredTrue = std::bool_constant<TypePred<T>::value>;

    template <typename T>
    using PredFalse = std::negation<PredTrue<T>>;

public:
    using type = std::pair<type_filter_t<TypeList, PredTrue>, type_filter_t<TypeList, PredFalse>>;
};

template <type_list TypeList, template <typename> typename TypePred>
using type_partition_t = typename type_partition<TypeList, TypePred>::type;

template <type_list TypeList, template <typename, typename> typename Op>
struct type_sort;

template <type_list TypeList, template <typename, typename> typename Op>
using type_sort_t = typename type_sort<TypeList, Op>::type;

template <template <typename, typename> typename Op>
struct type_sort<type_tuple<>, Op> {
    using type = type_tuple<>;
};

template <template <typename, typename> typename Op, typename Head>
struct type_sort<type_tuple<Head>, Op> {
    using type = type_tuple<Head>;
};

template <template <typename, typename> typename Op, typename Head, typename ... Remains>
struct type_sort<type_tuple<Head, Remains...>, Op> {
private:
    template <typename T>
    using cmp = std::bool_constant<Op<T, Head>::value>;

    using partitioned = type_partition_t<type_tuple<Remains...>, cmp>;

    using left = typename type_sort<typename partitioned::first_type, Op>::type;
    using right = typename type_sort<typename partitioned::second_type, Op>::type;

public:
    using type = type_concat_t<left, type_tuple<Head>, right>;
};

template <type_list TypeList>
struct type_reverse;

template <type_list TypeList>
using type_reverse_t = typename type_reverse<TypeList>::type;

template <>
struct type_reverse<type_tuple<>> {
    using type = type_tuple<>;
};

template <typename T>
struct type_reverse<type_tuple<T>> {
    using type = type_tuple<T>;
};

template <typename Head, typename ... Remains>
struct type_reverse<type_tuple<Head, Remains...>> {
    using type = type_concat_t<typename type_reverse<type_tuple<Remains...>>::type, type_tuple<Head>>;
};

}

#endif //ABC_TYPE_TRAITS
