// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_EXPECTED)
#define ABC_EXPECTED

#pragma once

#include "abc/error.h"
#include "abc/utility.h"

#include <cassert>
#include <expected>
#include <functional>
#include <memory>
#include <optional>
#include <type_traits>

namespace abc {

template <typename E>
class [[nodiscard]] unexpected;

template <typename T>
class [[nodiscard]] ok;

template <typename T, typename E>
class [[nodiscard]] expected;

struct unexpect_t {
    constexpr unexpect_t() = default;
};
inline constexpr unexpect_t unexpect{};

namespace details {

template <typename E>
constexpr bool is_unexpected_v = false;

template <typename E>
constexpr bool is_unexpected_v<abc::unexpected<E>> = true;

template <typename E>
concept can_be_unexpected = std::is_object_v<E> && (!std::is_array_v<E>) && (!is_unexpected_v<E>) && (!std::is_const_v<E>) && (!std::is_volatile_v<E>);

//template <typename T>
//concept can_be_ok = std::is_object_v<T> && (!std::is_array_v<T>) && (!std::is_const_v<T>) && (!std::is_volatile_v<T>);

template <typename T>
constexpr bool is_expected_v = false;

template <typename T, typename E>
constexpr bool is_expected_v<abc::expected<T, E>> = true;

template <typename Fn, typename ... Args>
using transformed_type = std::remove_cv_t<std::invoke_result_t<Fn &&, Args &&...>>;

template <typename Fn, typename ... Args>
using transformed_decay_type = std::decay_t<std::invoke_result_t<Fn &&, Args && ...>>;

template <typename Fn, typename ... Args>
using expected_t = std::remove_cvref_t<std::invoke_result_t<Fn &&, Args &&...>>;

struct in_place_val_fn_t {
    constexpr in_place_val_fn_t() = default;
};
constexpr inline in_place_val_fn_t in_place_val_fn{};

struct in_place_err_fn_t {
    constexpr in_place_err_fn_t() = default;
};
constexpr inline in_place_err_fn_t in_place_err_fn{};

template <typename T>
struct exception_safe_guard {
    static_assert(std::is_nothrow_move_constructible_v<T>);

    constexpr explicit
    exception_safe_guard(T & o) : original_address_{std::addressof(o)}, tmp_{std::move(o)} {
        std::destroy_at(original_address_);
    }

    constexpr
    ~exception_safe_guard() {
        if (original_address_) [[unlikely]] {
            std::construct_at(original_address_, std::move(tmp_));
        }
    }

    exception_safe_guard(exception_safe_guard const &) = delete;

    exception_safe_guard & operator=(exception_safe_guard const &) = delete;

    constexpr
    T &&
    release() {
        original_address_ = nullptr;
        return std::move(tmp_);
    }

private:
    T * original_address_;
    T tmp_;
};

template <typename T, typename U, typename V>
constexpr void reinit_expected(T * new_value, U * old_value, V && v) {
    if constexpr (std::is_nothrow_constructible_v<T, V>) {
        std::destroy_at(old_value);
        std::construct_at(new_value, std::forward<V>(v));
    } else if constexpr (std::is_nothrow_move_constructible_v<T>) {
        T tmp{std::forward<V>(v)};
        std::destroy_at(old_value);
        std::construct_at(new_value, std::move(tmp));
    } else {
        exception_safe_guard<U> guard{*old_value};
        std::construct_at(new_value, std::forward<V>(v));
        [[maybe_unused]] auto _ = guard.release();
    }
}

template <typename T, typename E, bool = std::is_trivially_destructible_v<T>, bool = std::is_trivially_destructible_v<E>>
struct expected_storage {
    union {
        T ok_;
        E err_;
    };
    bool has_value_;

    constexpr
    expected_storage() noexcept(std::is_nothrow_default_constructible_v<T>) : ok_{}, has_value_{true} {}

    template <typename ... Args>
    requires std::is_constructible_v<T, Args...>
    constexpr
    expected_storage(std::in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : ok_{std::forward<Args>(args)...}, has_value_{true} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<T, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(std::in_place_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>, Args...>)
        : ok_{ilist, std::forward<Args>(args)...}, has_value_{true} {
    }

    template <typename ... Args>
    requires std::is_constructible_v<E, Args...>
    constexpr
    expected_storage(unexpect_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : err_{std::forward<Args>(args)...}, has_value_{false} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<E, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(unexpect_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args...>)
        : err_{ilist, std::forward<Args>(args)...}, has_value_{false} {
    }

    ~expected_storage() noexcept(std::is_nothrow_destructible_v<T> && std::is_nothrow_destructible_v<E>) {
        if (has_value_) {
            std::destroy_at(std::addressof(ok_));
        } else {
            std::destroy_at(std::addressof(err_));
        }
    }
};

template <typename T, typename E>
struct expected_storage<T, E, true, true> {
    union {
        T ok_;
        E err_;
    };
    bool has_value_;

    constexpr
    expected_storage() noexcept(std::is_nothrow_default_constructible_v<T>) : ok_{}, has_value_{true} {}

    template <typename ... Args>
    requires std::is_constructible_v<T, Args...>
    constexpr
    expected_storage(std::in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : ok_{std::forward<Args>(args)...}, has_value_{true} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<T, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(std::in_place_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>, Args...>)
        : ok_{ilist, std::forward<Args>(args)...}, has_value_{true} {
    }

    template <typename ... Args>
    requires std::is_constructible_v<E, Args...>
    constexpr
    expected_storage(unexpect_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : err_{std::forward<Args>(args)...}, has_value_{false} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<E, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(unexpect_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args...>)
        : err_{ilist, std::forward<Args>(args)...}, has_value_{false} {
    }

    ~expected_storage() = default;
};

template <typename T, typename E>
struct expected_storage<T, E, true, false> {
    union {
        T ok_;
        E err_;
    };
    bool has_value_;

    constexpr
    expected_storage() noexcept(std::is_nothrow_default_constructible_v<T>) : ok_{}, has_value_{true} {}

    template <typename ... Args>
    requires std::is_constructible_v<T, Args...>
    constexpr
    expected_storage(std::in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : ok_{std::forward<Args>(args)...}, has_value_{true} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<T, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(std::in_place_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>, Args...>)
        : ok_{ilist, std::forward<Args>(args)...}, has_value_{true} {
    }

    template <typename ... Args>
    requires std::is_constructible_v<E, Args...>
    constexpr
    expected_storage(unexpect_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : err_{std::forward<Args>(args)...}, has_value_{false} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<E, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(unexpect_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args...>)
        : err_{ilist, std::forward<Args>(args)...}, has_value_{false} {
    }

    ~expected_storage() noexcept(std::is_nothrow_destructible_v<T> && std::is_nothrow_destructible_v<E>) {
        if (!has_value_) {
            std::destroy_at(std::addressof(err_));
        }
    }
};

template <typename T, typename E>
struct expected_storage<T, E, false, true> {
    union {
        T ok_;
        E err_;
    };
    bool has_value_;

    constexpr
    expected_storage() noexcept(std::is_nothrow_default_constructible_v<T>) : ok_{}, has_value_{true} {}

    template <typename ... Args>
    requires std::is_constructible_v<T, Args...>
    constexpr
    expected_storage(std::in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, Args...>)
        : ok_{std::forward<Args>(args)...}, has_value_{true} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<T, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(std::in_place_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U>, Args...>)
        : ok_{ilist, std::forward<Args>(args)...}, has_value_{true} {
    }

    template <typename ... Args>
    requires std::is_constructible_v<E, Args...>
    constexpr
    expected_storage(unexpect_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : err_{std::forward<Args>(args)...}, has_value_{false} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<E, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(unexpect_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args...>)
        : err_{ilist, std::forward<Args>(args)...}, has_value_{false} {
    }

    ~expected_storage() noexcept(std::is_nothrow_destructible_v<T> && std::is_nothrow_destructible_v<E>) {
        if (has_value_) {
            std::destroy_at(std::addressof(ok_));
        }
    }
};

template <typename E>
struct expected_storage<void, E, false, true> {
    using void_placeholder_t = struct {
    };
    union {
        void_placeholder_t _;
        E err_;
    };
    bool has_value_;

    constexpr
    expected_storage() noexcept : has_value_{true} {}

    template <typename ... Args>
    requires std::is_constructible_v<E, Args...>
    constexpr
    expected_storage(unexpect_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : err_{std::forward<Args>(args)...}, has_value_{false} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<E, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(unexpect_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args...>)
        : err_{ilist, std::forward<Args>(args)...}, has_value_{false} {
    }

    ~expected_storage() = default;
};

template <typename E>
struct expected_storage<void, E, false, false> {
    using void_placeholder_t = struct {
    };
    union {
        void_placeholder_t _;
        E err_;
    };
    bool has_value_;

    constexpr
    expected_storage() noexcept : has_value_{true} {}

    template <typename ... Args>
    requires std::is_constructible_v<E, Args...>
    constexpr
    expected_storage(unexpect_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : err_{std::forward<Args>(args)...}, has_value_{false} {
    }

    template <typename U, typename ... Args>
    requires std::is_constructible_v<E, std::initializer_list<U>, Args...>
    constexpr
    expected_storage(unexpect_t, std::initializer_list<U> ilist, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args...>)
        : err_{ilist, std::forward<Args>(args)...}, has_value_{false} {
    }

    ~expected_storage() noexcept(std::is_nothrow_destructible_v<E>) {
        if (!has_value_) {
            std::destroy_at(std::addressof(err_));
        }
    }
};

template <typename T, typename E>
struct expected_storate_operation {
    expected_storage<T, E> storage_;
};

}

}

namespace abc {

template <typename E>
class [[nodiscard]] unexpected {
    static_assert(details::can_be_unexpected<E>);

private:
    E err_;

public:
    constexpr unexpected() noexcept = default;
    constexpr unexpected(unexpected const &) = default;
    constexpr unexpected(unexpected &&) = default;
    constexpr auto operator=(unexpected const &) -> unexpected & = default;
    constexpr auto operator=(unexpected &&) -> unexpected & = default;
    ~unexpected() = default;

    constexpr unexpected(E const & e) noexcept(std::is_nothrow_copy_constructible_v<E>) : err_{ e } {}
    constexpr unexpected(E && e) noexcept(std::is_nothrow_move_constructible_v<E>) : err_{ std::move(e) } {}

    template <typename G = E> requires (!std::is_same_v<std::remove_cvref_t<G>, unexpected>) && (!std::is_same_v<std::remove_cvref_t<G>, std::in_place_t>) && std::is_constructible_v<E, G>
    constexpr explicit unexpected(G && e) noexcept(std::is_nothrow_constructible_v<E, G>)
        : err_{ std::forward<G>(e) } {
    }

    template <typename ... Args>
    requires std::is_constructible_v<E, Args...>
    constexpr explicit unexpected(std::in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : err_{ std::forward<Args>(args)... } {
    }

    template <typename U, typename ... Args> requires std::is_constructible_v<E, std::initializer_list<U> &, Args...>
    constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> il, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args...>)
        : err_{ il, std::forward<Args>(args)... } {
    }

    [[nodiscard]] constexpr auto error() const & noexcept -> E const & {
        return err_;
    }

    [[nodiscard]] constexpr auto error() & noexcept -> E & {
        return err_;
    }

    [[nodiscard]] constexpr auto error() const && noexcept -> E const && {
        return std::move(err_);
    }

    [[nodiscard]] constexpr auto error() && noexcept -> E && {
        return std::move(err_);
    }

    constexpr void swap(unexpected & other) noexcept(std::is_nothrow_swappable_v<E>) {
        using std::swap;
        swap(err_, other.err_);
    }

    template <typename G>
    friend constexpr auto operator==(unexpected const & lhs, unexpected<G> const & rhs) noexcept -> bool {
        return lhs.err_ == rhs.error();
    }

    friend constexpr auto
    swap(unexpected & lhs, unexpected & rhs) noexcept(noexcept(lhs.swap(rhs))) -> void requires std::is_swappable_v<E> {
        lhs.swap(rhs);
    }
};

template <typename E>
unexpected(E) -> unexpected<E>;

template <class E>
constexpr unexpected<typename std::decay<E>::type>
make_unexpected(E && e) {
    return unexpected<typename std::decay<E>::type>(std::forward<E>(e));
}

template <typename T, typename E>
class expected {
private:
    union {
        T val_;
        E err_;
    };
    bool has_value_;

    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_function_v<T>);
    static_assert(!std::is_same_v<std::remove_cv_t<T>, std::in_place_t>);
    static_assert(!details::is_unexpected_v<std::remove_cv_t<T>>);
    static_assert(details::can_be_unexpected<E>);

    template <typename U, typename G>
    constexpr static bool constructed_from_expected_v = std::disjunction_v<
        std::is_constructible<T, expected<U, G> &>,
        std::is_constructible<T, expected<U, G>>,
        std::is_constructible<T, expected<U, G> const &>,
        std::is_constructible<T, expected<U, G> const>,
        std::is_convertible<expected<U, G> &, T>,
        std::is_convertible<expected<U, G>, T>,
        std::is_convertible<expected<U, G> const &, T>,
        std::is_convertible<expected<U, G> const, T>,
        std::is_constructible<unexpected<E>, expected<U, G>>,
        std::is_constructible<unexpected<E>, expected<U, G> &>,
        std::is_constructible<unexpected<E>, expected<U, G> const>,
        std::is_constructible<unexpected<E>, expected<U, G> const &>
    >;

    template<typename U, typename G>
    constexpr static bool explicit_convert_v = std::disjunction_v<
        std::negation<std::is_convertible<U, T>>,
        std::negation<std::is_convertible<G, E>>
    >;
    
    template <typename U>
    constexpr static bool is_expected_with_same_value_type_v = details::is_expected_v<U> && std::is_same_v<T, typename U::value_type>;
    
    template <typename U>
    constexpr static bool is_expected_with_same_error_type_v = details::is_expected_v<U> && std::is_same_v<E, typename U::error_type>;

    template <typename U>
    constexpr auto
    assign_value(U && v) -> void {
        if (has_value_) {
            val_ = std::forward<U>(v);
        } else {
            details::reinit_expected(std::addressof(val_), std::addressof(err_), std::forward<U>(v));
            has_value_ = true;
        }

        assert(this->has_value_);
    }

    template <typename G>
    constexpr auto
    assign_error(G && e) -> void {
        if (has_value_) {
            details::reinit_expected(std::addressof(err_), std::addressof(val_), std::forward<G>(e));
            has_value_ = false;
        } else {
            err_ = std::forward<G>(e);
        }

        assert(!this->has_value_);
    }

    constexpr auto
    swap_value_error(expected & rhs) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<T>>) -> void {
        assert(this->has_value_ && !rhs.has_value_);

        if constexpr (std::is_nothrow_move_constructible_v<E>) {
            details::exception_safe_guard<E> guard{ rhs.err_ };
            std::construct_at(std::addressof(rhs.val_), std::move(this->val_));
            rhs.has_value_ = true;

            std::destroy_at(std::addressof(this->val_));
            std::construct_at(std::addressof(this->err_), guard.release());
            this->has_value_ = false;
        } else {
            details::exception_safe_guard<T> guard{ this->val_ };
            std::construct_at(std::addressof(this->err_), std::move(rhs.err_));
            this->has_value_ = false;

            std::destroy_at(std::addressof(rhs.err_));
            std::construct_at(std::addressof(rhs.val_), guard.release());
            rhs.has_value_ = true;
        }
    }

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

//    template<typename U>
//    using rebind = expected<U, E>;

    // Conditionally Trivial Special Member Functions: http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p0848r3.html
    // https://devblogs.microsoft.com/cppblog/conditionally-trivial-special-member-functions/

    // Default constructors

    constexpr
    expected() noexcept(std::is_nothrow_default_constructible_v<T>) requires std::is_default_constructible_v<T> && std::is_trivially_default_constructible_v<T> && std::is_trivially_default_constructible_v<E> : val_{}, has_value_{ true } {
    }

    constexpr
    expected() noexcept(std::is_nothrow_default_constructible_v<T>) requires std::is_default_constructible_v<T> && (!std::is_trivially_default_constructible_v<T> || !std::is_trivially_default_constructible_v<E>) : val_{}, has_value_{ true } {
    }

    // Copy constructors

    constexpr
    expected(expected const &) = default;

    constexpr
    expected(expected const & other) noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<T>, std::is_nothrow_copy_constructible<E>>)
                                 requires std::is_copy_constructible_v<T> && std::is_copy_constructible_v<E> && (!std::is_trivially_copy_constructible_v<T> || !std::is_trivially_copy_constructible_v<E>)
        : has_value_{ other.has_value_ } {
        if (has_value_) {
            if constexpr (!std::is_trivially_copy_constructible_v<T>) {
                std::construct_at(std::addressof(val_), other.val_);
            } else {
                val_ = other.val_;
            }
        } else {
            if constexpr (!std::is_trivially_copy_constructible_v<E>) {
                std::construct_at(std::addressof(err_), other.err_);
            } else {
                err_ = other.err_;
            }
        }
    }

    // Move constructors

    // Eligible move constructor: https://en.cppreference.com/w/cpp/language/move_constructor#Eligible_move_constructor

    constexpr
    expected(expected &&) = default;

    constexpr
    expected(expected && other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<E>>)
                            requires std::is_move_constructible_v<T> && std::is_move_constructible_v<E> && (!std::is_trivially_move_constructible_v<T> || !std::is_trivially_move_constructible_v<E>)
        : has_value_{ other.has_value_ } {
        if (has_value_) {
            if constexpr (!std::is_trivially_move_constructible_v<T>) {
                std::construct_at(std::addressof(val_), std::move(other).val_);
            } else {
                val_ = std::move(other).val_;
            }
        } else {
            if constexpr (!std::is_trivially_move_constructible_v<E>) {
                std::construct_at(std::addressof(err_), std::move(other).err_);
            } else {
                err_ = std::move(other).err_;
            }
        }
    }

    // Converting constructors

    template <typename U, typename G> requires std::is_constructible_v<T, std::add_lvalue_reference_t<std::add_const_t<U>>> && std::is_constructible_v<E, std::add_lvalue_reference_t<std::add_const_t<G>>> && (!constructed_from_expected_v<U, G>)
    constexpr explicit(explicit_convert_v<U const &, G const &>)
    expected(expected<U, G> const & other) noexcept(std::conjunction_v<std::is_nothrow_constructible<T, std::add_lvalue_reference_t<std::add_const_t<U>>>, std::is_nothrow_constructible<E, std::add_lvalue_reference_t<std::add_const_t<G>>>>)
        : has_value_{ other.has_value_ } {
        if (has_value_) {
            std::construct_at(std::addressof(val_), other.val_);
        } else {
            std::construct_at(std::addressof(err_), other.err_);
        }
    }

    template <typename U, typename G> requires std::is_constructible_v<T, U> && std::is_constructible_v<E, G> && (!constructed_from_expected_v<U, G>)
    constexpr explicit(explicit_convert_v<U, G>)
    expected(expected<U, G> && other) // noexcept(std::conjunction_v<std::is_nothrow_constructible<T, U>, std::is_nothrow_constructible<E, G>>)
        : has_value_{ other.has_value_ } {
        if (has_value_) {
            std::construct_at(std::addressof(val_), std::move(other.val_));
        } else {
            std::construct_at(std::addressof(err_), std::move(other.err_));
        }
    }

    template<typename U = T> requires (!std::is_same_v<std::remove_cvref_t<U>, expected>) &&
                                      (!std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>) &&
                                      (!details::is_unexpected_v<std::remove_cvref_t<U>>) &&
                                      std::is_constructible_v<T, U>
    constexpr explicit(!std::is_convertible_v<U, T>) expected(U && v) // noexcept(std::is_nothrow_constructible_v<T, U>)
        : val_{ std::forward<U>(v) }, has_value_{ true } {
    }

    template<typename G = E> requires std::is_constructible_v<E, G const &>
    constexpr explicit(!std::is_convertible_v<G const &, E>)
    expected(unexpected<G> const & e) noexcept(std::is_nothrow_constructible_v<E, G const &>)
        : err_{ e.error() }, has_value_{ false } {
    }

    template<typename G = E> requires std::is_constructible_v<E, G>
    constexpr explicit(!std::is_convertible_v<G, E>)
    expected(unexpected<G> && e) noexcept(std::is_nothrow_constructible_v<E, G>)
        : err_{ std::move(e).error() }, has_value_{ false } {
    }

    template <typename ... Args> requires std::is_constructible_v<T, Args...>
    constexpr explicit
    expected(std::in_place_t, Args &&... args ) noexcept(std::is_nothrow_constructible_v<T, Args...>) : val_{ std::forward<Args>(args)... }, has_value_{ true } {
    }

    template <typename U, typename ... Args > requires std::is_constructible_v<T, std::initializer_list<U> &, Args...>
    constexpr explicit
    expected(std::in_place_t, std::initializer_list<U> il, Args &&... args) noexcept(std::is_nothrow_constructible_v<T, std::initializer_list<U> &, Args...>) : val_{il, std::forward<Args>(args)... }, has_value_{ true } {
    }

    template <typename ...>
    constexpr explicit
    expected(std::in_place_t) : expected() {
    }

    template <typename ... Args> requires std::is_constructible_v<E, Args...>
    constexpr explicit
    expected(unexpect_t, Args&&... args ) noexcept(std::is_nothrow_constructible_v<E, Args...>) : err_{ std::forward<Args>(args)... }, has_value_{ false } {
    }

    template <typename U, typename ... Args> requires std::is_constructible_v<E, std::initializer_list<U> &, Args...>
    constexpr explicit
    expected(unexpect_t, std::initializer_list<U> il, Args &&... args ) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U> &, Args...>) : err_{ il, std::forward<Args>(args)... }, has_value_{ false } {
    }

private:
    template<typename, typename> friend class expected;

    template <typename Fn>
    explicit
    expected(details::in_place_val_fn_t, Fn && f)
        : val_{ std::forward<Fn>(f)() }, has_value_{ true } {
    }

    template <typename Fn>
    explicit
    expected(details::in_place_err_fn_t, Fn && f)
        : err_{ std::forward<Fn>(f)() }, has_value_{ false } {
    }
public:

    // Destructor
    // Prospective destructor: https://en.cppreference.com/w/cpp/language/destructor#Prospective_destructor
    constexpr
    ~expected() = default;

    constexpr
    ~expected() requires (!std::is_trivially_destructible_v<T>) || (!std::is_trivially_destructible_v<E>) {
        if (has_value_) {
            if constexpr (!std::is_trivially_destructible_v<T>) {
                std::destroy_at(std::addressof(val_));
            }
        } else {
            if constexpr (!std::is_trivially_destructible_v<E>) {
                std::destroy_at(std::addressof(err_));
            }
        }
    }

    // Assignment operators

    // Eligible copy assignment operator: https://en.cppreference.com/w/cpp/language/copy_assignment#Eligible_copy_assignment_operator
    constexpr auto
    operator=(expected const & other) -> expected & = delete;

    constexpr auto
    operator=(expected const & other) -> expected & requires std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T> && std::is_copy_assignable_v<E> && std::is_copy_constructible_v<E> && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>) {
        if (other.has_value_) {
            this->assign_value(other.val_);
        } else {
            this->assign_error(other.err_);
        }

        return *this;
    }

    constexpr auto
    operator=(expected && other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_assignable<T>, std::is_nothrow_move_constructible<E>, std::is_nothrow_move_assignable<E>>) -> expected &
                               requires std::is_move_constructible_v<T> && std::is_move_assignable_v<T> && std::is_move_constructible_v<E> && std::is_move_assignable_v<E> && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>) {
        if (other.has_value_) {
            this->assign_value(std::move(other.val_));
        } else {
            this->assign_error(std::move(other.err_));
        }

        return *this;
    }

    template <typename U = T> requires (!std::is_same_v<expected, std::remove_cvref_t<U>>) && (!details::is_unexpected_v<U>) && std::is_constructible_v<T, U> && (std::is_nothrow_constructible_v<T, U> || std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
    constexpr auto
    operator=(U && v) -> expected & {
        if (this->has_value_) {
            val_ = std::forward<U>(v);
        } else {
            this->assign_value(std::forward<U>(v));
        }

        return *this;
    }

    template <typename G> requires std::is_constructible_v<E, G const &> &&
                                   std::is_assignable_v<E &, G const &> &&
                                   (std::is_nothrow_constructible_v<E, G const &> || std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
    constexpr auto
    operator=(unexpected<G> const & e) -> expected & {
        this->assign_error(e.error());
        return *this;
    }

    template <typename G> requires std::is_constructible_v<E, G> &&
                                   std::is_assignable_v<E &, G> &&
                                   (std::is_nothrow_constructible_v<E, G> || std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
    constexpr auto
    operator=(unexpected<G> && e) -> expected & {
        this->assign_error(std::move(e).error());
        return *this;
    }

    // Modifiers

    template<typename ... Args> requires std::is_nothrow_constructible_v<T, Args...>
    constexpr auto
    emplace(Args &&... args) noexcept -> T & {
        if (has_value_) {
            std::destroy_at(std::addressof(val_));
        } else {
            std::destroy_at(std::addressof(err_));
            has_value_ = true;
        }
        std::construct_at(std::addressof(val_), std::forward<Args>(args)...);

        return val_;
    }

    template<typename U, typename... Args> requires std::is_nothrow_constructible_v<T, std::initializer_list<U> &, Args...>
    constexpr auto
    emplace(std::initializer_list<U> il, Args &&... args) noexcept -> T & {
        if (has_value_) {
            std::destroy_at(std::addressof(val_));
        } else {
            std::destroy_at(std::addressof(err_));
            has_value_ = true;
        }

        std::construct_at(std::addressof(val_), il, std::forward<Args>(args)...);

        return val_;
    }

    template<typename U, typename... Args> requires (!std::is_nothrow_constructible_v<T, std::initializer_list<U> &, Args...>)
    constexpr auto
    emplace(std::initializer_list<U> il, Args &&... args) -> T & {
        if (has_value_) {
            details::exception_safe_guard<T> guard{ val_ };
            std::construct_at(std::addressof(val_), il, std::forward<Args>(args)...);
            [[maybe_unused]] auto _ = guard.release();
        } else {
            std::destroy_at(std::addressof(err_));
            std::construct_at(std::addressof(val_), il, std::forward<Args>(args)...);
            has_value_ = true;
        }

        return val_;
    }

    // Swap

    constexpr void
    swap(expected & other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<E>, std::is_nothrow_swappable<T &>, std::is_nothrow_swappable<E &>>)
                         requires std::is_swappable_v<T> && std::is_swappable_v<E> && std::is_move_constructible_v<T> && std::is_move_constructible_v<E> && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>) {
        if (this->has_value_) {
            if (other.has_value_) {
                std::swap(this->val_, other.val_);
            } else {
                this->swap_value_error(other);
            }
        } else {
            if (!other.has_value_) {
                std::swap(this->err_, other.err_);
            } else {
                other.swap_value_error(*this);
            }
        }
    }

    friend constexpr void
    swap(expected & lhs, expected & rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }

    // Equality operator

    template <typename T2, typename E2> requires (!std::is_void_v<T2>)
    friend constexpr auto
    operator==(expected const & lhs, expected<T2, E2> const & rhs) -> bool {
        if (lhs.has_value()) {
            return rhs.has_value() && (lhs.value() == rhs.value());
        } else {
            return !rhs.has_value() && (lhs.error() == rhs.error());
        }
    }

    template <typename T2, typename E2> requires std::is_void_v<T2>
    friend constexpr auto
    operator==(expected const & lhs, expected<T2, E2> const & rhs) -> bool {
        if (lhs.has_value()) {
            return rhs.has_value();
        } else {
            return !rhs.has_value()  && (lhs.error() == rhs.error());
        }
    }

    template <typename T2>
    friend constexpr auto
    operator==(expected const & x, T2 const & val) -> bool {
        return x.has_value() && (x.value() == val);
    }

    template <typename E2>
    friend constexpr auto
    operator==(expected const & x, unexpected<E2> const & e) -> bool {
        return !x.has_value() && (x.error() == e.error());
    }

    // Observers

    [[nodiscard]] constexpr explicit
    operator bool() const noexcept {
        return has_value_;
    }

    [[nodiscard]]
    constexpr auto
    has_value() const noexcept -> bool {
        return has_value_;
    }

    [[nodiscard]]
    constexpr auto
    value() const & -> T const & {
        if (has_value()) [[likely]] {
            return val_;
        }

        throw_error(make_error_code(errc::bad_expected_access));
        abc::unreachable();
    }

    [[nodiscard]]
    constexpr auto
    value() & -> T & {
        if (has_value()) [[likely]] {
            return val_;
        }

        throw_error(make_error_code(errc::bad_expected_access));
        abc::unreachable();
    }

    [[nodiscard]]
    constexpr auto
    value() const && -> T const && {
        if (has_value()) [[likely]] {
            return std::move(val_);
        }

        throw_error(make_error_code(errc::bad_expected_access));
        abc::unreachable();
    }

    [[nodiscard]]
    constexpr auto
    value() && -> T && {
        if (has_value()) [[likely]] {
            return std::move(val_);
        }

        throw_error(make_error_code(errc::bad_expected_access));
        abc::unreachable();
    }

    constexpr auto
    error() const & noexcept -> E const & {
        assert(!this->has_value_);
        return err_;
    }

    constexpr auto
    error() & noexcept -> E & {
        assert(!this->has_value_);
        return err_;
    }

    constexpr auto
    error() const && noexcept -> E const && {
        assert(!this->has_value_);
        return std::move(err_);
    }

    constexpr auto
    error() && noexcept -> E && {
        assert(!this->has_value_);
        return std::move(err_);
    }

    template <typename U>
    constexpr auto
    value_or(U && v) const & -> T {
        static_assert(std::is_copy_constructible_v<T>);
        static_assert(std::is_convertible_v<U, T>);

        if (has_value()) {
            return val_;
        }

        return static_cast<T>(std::forward<U>(v));
    }

    template <typename U>
    constexpr auto
    value_or(U && v) && -> T {
        static_assert(std::is_move_constructible_v<T>);
        static_assert(std::is_convertible_v<U, T>);

        if (has_value()) {
            return std::move(val_);
        }

        return static_cast<T>(std::forward<U>(v));
    }

    [[nodiscard]] constexpr const T *
    operator->() const & noexcept {
        assert(has_value());
        return std::addressof(val_);
    }

    [[nodiscard]] constexpr T *
    operator->() && noexcept {
        assert(has_value());
        return std::addressof(val_);
    }

    [[nodiscard]] constexpr auto
    operator*() const & noexcept -> T const & {
        assert(has_value());
        return val_;
    }

    [[nodiscard]] constexpr auto
    operator*() & noexcept -> T & {
        assert(has_value());
        return val_;
    }

    [[nodiscard]] constexpr auto
    operator*() const && noexcept -> T const && {
        assert(has_value());
        return std::move(val_);
    }

    [[nodiscard]] constexpr auto
    operator*() && noexcept -> T &&
    {
        assert(has_value());
        return std::move(val_);
    }

    // Monadic operations

    template <typename Fn> requires std::is_copy_constructible_v<E>
    [[nodiscard]] constexpr auto
    and_then(Fn && f) & -> abc::details::expected_t<Fn, T &> {
        using U = abc::details::expected_t<Fn, T &>;

        static_assert(is_expected_with_same_error_type_v<U>);
        static_assert(std::is_invocable_v<Fn, T &>);

        if (has_value()) {
            return std::invoke(std::forward<Fn>(f), value());
        }

        return U{ unexpect, error() };
    }

    template <typename Fn> requires std::is_copy_constructible_v<E>
    [[nodiscard]] constexpr auto
    and_then(Fn && f) const & -> abc::details::expected_t<Fn, T const &> {
        using U = abc::details::expected_t<Fn, T const &>;

        static_assert(is_expected_with_same_error_type_v<U>);
        static_assert(std::is_invocable_v<Fn, T const &>);

        if (has_value()) {
            return std::invoke(std::forward<Fn>(f), value());
        }

        return U{ unexpect, error() };
    }

    template <typename Fn> requires std::is_move_constructible_v<E>
    [[nodiscard]] constexpr auto
    and_then(Fn && f) && -> abc::details::expected_t<Fn, T &&> {
        using U = abc::details::expected_t<Fn, T &&>;

        static_assert(is_expected_with_same_error_type_v<U>);
        static_assert(std::is_invocable_v<Fn, T &&>);

        if (has_value()) {
            return std::invoke(std::forward<Fn>(f), std::move(value()));
        }

        return U{ unexpect, std::move(error()) };
    }

    template <typename Fn> requires std::is_copy_constructible_v<E>
    [[nodiscard]] constexpr auto
    and_then(Fn && f) const && -> details::expected_t<Fn, T const &&> {
        using U = abc::details::expected_t<Fn, T const &&>;

        static_assert(is_expected_with_same_error_type_v<U>);
        static_assert(std::is_invocable_v<Fn, T const &&>);

        if (has_value()) {
            return std::invoke(std::forward<Fn>(f), std::move(value()));
        }

        return U{ unexpect, std::move(error()) };
    }

private:
    template <typename Self, typename Fn>
    constexpr static auto
    transform_impl(Self && self, Fn && f) -> expected<details::transformed_decay_type<Fn, decltype(std::forward<Self>(self).value())>, E> {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);
        static_assert(std::is_invocable_v<Fn, decltype(std::forward<Self>(self).value())>);

        using U = details::transformed_type<Fn, decltype(std::forward<Self>(self).value())>;
        static_assert(!details::is_expected_v<U>);

        using result_type = expected<details::transformed_decay_type<Fn, decltype(std::forward<Self>(self).value())>, E>;

        if (self.has_value()) {
            return result_type{ details::in_place_val_fn, [&]() { return std::invoke(std::forward<Fn>(f), std::forward<Self>(self).value()); } };
        }

        return result_type{ unexpect, std::forward<Self>(self).error() };
    }

public:
    template <typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    transform(Fn && f) & -> expected<details::transformed_decay_type<Fn, T &>, E> {
        return transform_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    transform(Fn && f) const & -> expected<details::transformed_decay_type<Fn, T const &>, E> {
        return transform_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    transform(Fn && f) && -> expected<details::transformed_decay_type<Fn, T &&>, E> {
        return transform_impl(std::move(*this), std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    transform(Fn && f) const && -> expected<details::transformed_decay_type<Fn, T const &&>, E> {
        return transform_impl(std::move(*this), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename Fn>
    constexpr static auto
    or_else_impl(Self && self, Fn && f) -> details::expected_t<Fn, decltype(std::forward<Self>(self).error())> {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);
        static_assert(std::is_invocable_v<Fn, decltype(std::forward<Self>(self).error())>);

        using U = details::expected_t<Fn, decltype(std::forward<Self>(self).error())>;
        static_assert(is_expected_with_same_value_type_v<U>);

        if (!self.has_value()) {
            return std::invoke(std::forward<Fn>(f), std::forward<Self>(self).error());
        }

        return U{ std::in_place, std::forward<Self>(self).value() };
    }

public:
    template <typename Fn> requires std::is_copy_constructible_v<T>
    [[nodiscard]] constexpr auto
    or_else(Fn && f) & -> abc::details::expected_t<Fn, E &> {
        return or_else_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_copy_constructible_v<T>
    [[nodiscard]] constexpr auto
    or_else(Fn && f) const & -> abc::details::expected_t<Fn, E const &> {
        return or_else_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<T>
    [[nodiscard]] constexpr auto
    or_else(Fn && f) && -> abc::details::expected_t<Fn, E &&> {
        return or_else_impl(std::move(*this), std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_copy_constructible_v<T>
    [[nodiscard]] constexpr auto
    or_else(Fn && f) const && -> abc::details::expected_t<Fn, E const &&> {
        return or_else_impl(std::move(*this), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename Fn>
    constexpr static auto
    transform_error_impl(Self && self, Fn && f) -> expected<T, details::transformed_decay_type<Fn, decltype(std::forward<Self>(self).error())>> {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);
        static_assert(std::is_invocable_v<Fn, decltype(std::forward<Self>(self).error())>);

        using G = details::transformed_type<Fn, decltype(std::forward<Self>(self).error())>;
        static_assert(!details::is_expected_v<G>);
        static_assert(details::can_be_unexpected<G>);

        using result_type = expected<T, details::transformed_decay_type<Fn, decltype(std::forward<Self>(self).error())>>;

        if (self.has_value()) {
            return result_type{ std::in_place, std::forward<Self>(self).value() };
        }

        return result_type{ details::in_place_err_fn, [&]() { return std::invoke(std::forward<Fn>(f), std::forward<Self>(self).error()); } };
    }

public:

    template <typename Fn> requires std::is_copy_constructible_v<T>
    constexpr auto
    transform_error(Fn && f) & -> expected<T, details::transformed_decay_type<Fn, E &>> {
        return transform_error_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_copy_constructible_v<T>
    constexpr auto
    transform_error(Fn && f) const & -> expected<T, details::transformed_decay_type<Fn, E const &>> {
        return transform_error_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<T>
    constexpr auto
    transform_error(Fn && f) && -> expected<T, details::transformed_decay_type<Fn, E &&>> {
        return transform_error_impl(std::move(*this), std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<T>
    constexpr auto
    transform_error(Fn && f) const && -> expected<T, details::transformed_decay_type<Fn, E const &&>> {
        return transform_error_impl(std::move(*this), std::forward<Fn>(f));
    }

    // Rust APIs

    [[nodiscard]] constexpr auto
    is_ok() const noexcept -> bool {
        return has_value();
    }

    template <typename Fn>
    [[nodiscard]] constexpr auto
    is_ok_and(Fn && f) const -> bool {
        static_assert(std::is_invocable_r_v<bool, Fn, T const &>);

        return is_ok() && std::invoke(std::forward<Fn>(f), val_);
    }

    [[nodiscard]] constexpr auto
    is_err() const noexcept -> bool {
        return !has_value();
    }

    template <typename Fn>
    [[nodiscard]] constexpr auto
    is_err_and(Fn && f) const -> bool {
        static_assert(std::is_invocable_r_v<bool, Fn, E const &>);

        return is_err() && std::invoke(std::forward<Fn>(f), err_);
    }

private:
    template <typename Self>
    constexpr static auto
    ok_impl(Self && self) -> std::optional<T> {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);
        static_assert(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>);

        if (self.is_ok()) {
            return std::make_optional<T>(std::forward<Self>(self).value());
        }

        return std::nullopt;
    }

#if defined(__cpp_explicit_this_parameter) && ((__cpp_explicit_this_parameter) >= 202110L)
public:
    [[nodiscard]] constexpr auto
    ok(this expected & self)  -> std::optional<T> requires std::is_copy_constructible_v<T> {
        return ok_impl(self);
    }

    [[nodiscard]] constexpr auto
    ok(this expected const & self) -> std::optional<T> requires std::is_copy_constructible_v<T> {
        return ok_impl(self);
    }

    constexpr auto
    ok(this expected const && self) -> std::optional<T> requires std::is_move_constructible_v<T> {
        return ok_impl(std::move(self));
    }

    constexpr auto
    ok(this expected && self) -> std::optional<T> requires std::is_move_constructible_v<T> {
        return ok_impl(std::move(self));
    }
#else
public:
    [[nodiscard]] constexpr auto
    ok() & -> std::optional<T> requires std::is_copy_constructible_v<T> {
        return ok_impl(*this);
    }

    [[nodiscard]] constexpr auto
    ok() const & -> std::optional<T> requires std::is_copy_constructible_v<T> {
        return ok_impl(*this);
    }

    [[nodiscard]] constexpr auto
    ok() && -> std::optional<T> requires std::is_move_constructible_v<T> {
        return ok_impl(std::move(*this));
    }

    [[nodiscard]] constexpr auto
    ok() const && -> std::optional<T> requires std::is_move_constructible_v<T> {
        return ok_impl(std::move(*this));
    }

#endif

#if defined(__cpp_explicit_this_parameter) && ((__cpp_explicit_this_parameter) >= 202110L)
#else

    [[nodiscard]] constexpr auto
    err() & -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(err_);
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    err() const & -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(err_);
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    err() && -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(std::move(err_));
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    err() const && -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(std::move(err_));
        }

        return std::nullopt;
    }

#endif

    template <typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    map(Fn && f) & -> expected<details::transformed_decay_type<Fn, T &>, E> {
        return transform_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    map(Fn && f) const & -> expected<details::transformed_decay_type<Fn, T const &>, E> {
        return transform_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    map(Fn && f) && -> expected<details::transformed_decay_type<Fn, T &&>, E> {
        return transform_impl(std::move(*this), std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    map(Fn && f) const && -> expected<details::transformed_decay_type<Fn, T const &&>, E> {
        return transform_impl(std::move(*this), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename U, typename Fn> requires std::is_same_v<expected, std::remove_cvref_t<Self>>
    constexpr static auto
    map_or_impl(Self && self, U && default_value, Fn && f) -> U {
        if (self.is_ok()) {
            return std::invoke(std::forward<Fn>(f), std::forward<Self>(self).value());
        }

        return std::forward<U>(default_value);
    }

public:
    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T &>
    [[nodiscard]] constexpr auto
    map_or(U && default_value, Fn && f) & -> U {
        return map_or_impl(*this, std::forward<U>(default_value), std::forward<Fn>(f));
    }

    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T const &>
    [[nodiscard]] constexpr auto
    map_or(U && default_value, Fn && f) const & -> U {
        return map_or_impl(*this, std::forward<U>(default_value), std::forward<Fn>(f));
    }

    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T &&>
    [[nodiscard]] constexpr auto
    map_or(U && default_value, Fn && f) && -> U {
        return map_or_impl(std::move(*this), std::forward<U>(default_value), std::forward<Fn>(f));
    }

    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T const &&>
    [[nodiscard]] constexpr auto
    map_or(U && default_value, Fn && f) const && -> U {
        return map_or_impl(std::move(*this), std::forward<U>(default_value), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename U, typename ErrFn, typename ValFn>
    constexpr static auto
    map_or_else_impl(Self && self, ErrFn && err_fn, ValFn && val_fn) -> U {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);

        if (self.is_ok()) {
            return static_cast<U>(std::invoke(std::forward<ValFn>(val_fn), std::forward<Self>(self).value()));
        }

        return static_cast<U>(std::invoke(std::forward<ErrFn>(err_fn), std::forward<Self>(self).error()));
    }

public:
    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_v<ErrFn, E &> && std::is_invocable_v<ValFn, T &>
    [[nodiscard]] constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) & -> U {
        static_assert(std::is_convertible_v<std::invoke_result_t<ErrFn, E &>, U>);
        static_assert(std::is_convertible_v<std::invoke_result_t<ValFn, T &>, U>);

        return map_or_else_impl(*this, std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_v<ErrFn, E const &> && std::is_invocable_v<ValFn, T const &>
    [[nodiscard]] constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) const & -> U {
        static_assert(std::is_convertible_v<std::invoke_result_t<ErrFn, E const &>, U>);
        static_assert(std::is_convertible_v<std::invoke_result_t<ValFn, T const &>, U>);

        return map_or_else_impl(*this, std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_v<ErrFn, E &&> && std::is_invocable_v<ValFn, T &&>
    [[nodiscard]] constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) && -> U {
        static_assert(std::is_convertible_v<std::invoke_result_t<ErrFn, E &&>, U>);
        static_assert(std::is_convertible_v<std::invoke_result_t<ValFn, T &&>, U>);

        return map_or_else_impl(std::move(*this), std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_v<ErrFn, E const &&> && std::is_invocable_v<ValFn, T const &&>
    [[nodiscard]] constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) const && -> U {
        static_assert(std::is_convertible_v<std::invoke_result_t<ErrFn, E const &&>, U>);
        static_assert(std::is_convertible_v<std::invoke_result_t<ValFn, T const &&>, U>);

        return map_or_else_impl(std::move(*this), std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

public:
    template <typename Fn> requires std::is_copy_constructible_v<T>
    constexpr auto
    map_err(Fn && f) & -> expected<T, details::transformed_decay_type<Fn, E &>> {
        return transform_error_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_copy_constructible_v<T>
    constexpr auto
    map_err(Fn && f) const & -> expected<T, details::transformed_decay_type<Fn, E const &>> {
        return transform_error_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<T>
    constexpr auto
    map_err(Fn && f) && -> expected<T, details::transformed_decay_type<Fn, E &&>> {
        return transform_error_impl(std::move(*this), std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<T>
    constexpr auto
    map_err(Fn && f) const && -> expected<T, details::transformed_decay_type<Fn, E const &&>> {
        return transform_error_impl(std::move(*this), std::forward<Fn>(f));
    }

    [[nodiscard]] constexpr auto
    expect(std::string_view msg) & noexcept -> T & {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return value();
    }

    [[nodiscard]] constexpr auto
    expect(std::string_view msg) const & noexcept -> T const & {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return value();
    }

    [[nodiscard]] constexpr auto
    expect(std::string_view msg) && noexcept -> T && {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return std::move(value());;
    }

    [[nodiscard]] constexpr auto
    expect(std::string_view msg) const && noexcept -> T const && {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return std::move(value());
    }

    [[nodiscard]] constexpr auto
    unwrap() & noexcept -> T & {    // mark noexcept, force panic if error occurs.
        return expect("expected should have value when calling `unwrap()`");
    }

    [[nodiscard]] constexpr auto
    unwrap() const & noexcept -> T const & {    // mark noexcept, force panic if error occurs.
        return expect("expected should have value when calling `unwrap()`");
    }

    [[nodiscard]] constexpr auto
    unwrap() && noexcept -> T && {    // mark noexcept, force panic if error occurs.
        return std::move(expect("expected should have value when calling `unwrap()`"));
    }

    [[nodiscard]] constexpr auto
    unwrap() const && noexcept -> T const && {    // mark noexcept, force panic if error occurs.
        return std::move(expect("expected should have value when calling `unwrap()`"));
    }

    [[nodiscard]] constexpr auto
    unwrap_or_default() const & noexcept(std::is_nothrow_default_constructible_v<T> && std::is_nothrow_copy_constructible_v<T>) -> T {
        static_assert(std::is_default_constructible_v<T>);
        static_assert(std::is_copy_constructible_v<T>);

        if (is_err()) {
            return T{};
        }

        return value();
    }

    [[nodiscard]] constexpr auto
    unwrap_or_default() && noexcept(std::is_nothrow_default_constructible_v<T> && std::is_nothrow_move_constructible_v<T>) -> T {
        static_assert(std::is_default_constructible_v<T>);
        static_assert(std::is_move_constructible_v<T>);

        if (is_err()) {
            return T{};
        }

        return std::move(val_);
    }

    [[nodiscard]] constexpr auto
    unwrap_or_default() const && -> T requires std::is_default_constructible_v<T> && std::is_move_constructible_v<T> {
        if (is_err()) {
            return T{};
        }

        return std::move(val_);
    }

    [[nodiscard]] constexpr auto
    expect_err(std::string_view msg) & noexcept -> E & {    // mark noexcept, force panic if expected has value.
        if (is_ok()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return error();
    }

    [[nodiscard]] constexpr auto
    expect_err(std::string_view msg) const & noexcept -> E const & {    // mark noexcept, force panic if expected has value.
        if (is_ok()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return error();
    }

    [[nodiscard]] constexpr auto
    expect_err(std::string_view msg) && noexcept -> E && {    // mark noexcept, force panic if expected has value.
        if (is_ok()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return std::move(error());
    }

    [[nodiscard]] constexpr auto
    expect_err(std::string_view msg) const && noexcept -> E const && {  // mark noexcept, force panic if expected has value.
        if (is_ok()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return std::move(error());
    }

    [[nodiscard]] constexpr auto
    unwrap_err() & noexcept -> E & {    // mark noexcept, force panic if expected has value.
        return expect_err(value().to_string());
    }

    [[nodiscard]] constexpr auto
    unwrap_err() const & noexcept -> E const & {    // mark noexcept, force panic if expected has value.
        return expect_err(value().to_string());
    }

    [[nodiscard]] constexpr auto
    unwrap_err() && noexcept -> E && {
        return expect_err(value().to_string());
    }

    [[nodiscard]] constexpr auto
    unwrap_err() const && noexcept -> E const && {
        return expect_err(value().to_string());
    }

    template <typename U> requires std::is_copy_constructible_v<T> && std::is_convertible_v<U, T>
    [[nodiscard]] constexpr auto
    unwrap_or(U && default_value) const & -> T {
        return value_or(std::forward<U>(default_value));
    }

    template <typename U> requires std::is_move_constructible_v<T> && std::is_constructible_v<U, T>
    [[nodiscard]] constexpr auto
    unwrap_or(U && default_value) && -> T {
        return value_or(std::forward<U>(default_value));
    }

    template <typename Fn>
    [[nodiscard]] constexpr auto
    unwrap_or_else(Fn && f) const & -> T requires std::is_copy_constructible_v<T> {
        using U = details::transformed_type<Fn, E const &>;

        static_assert(!details::is_expected_v<U>);
        static_assert(std::is_invocable_v<Fn, E const &>);
        static_assert(std::is_convertible_v<U, T>);

        if (is_ok()) {
            return value();
        }

        return std::invoke(std::forward<Fn>(f), error());
    }

    template <typename Fn>
    [[nodiscard]] constexpr auto
    unwrap_or_else(Fn && f) && -> T requires std::is_move_constructible_v<T> {
        using U = details::transformed_type<Fn, E &&>;

        static_assert(!details::is_expected_v<U>);
        static_assert(std::is_invocable_v<Fn, E &&>);
        static_assert(std::is_convertible_v<U, T>);

        if (is_ok()) {
            return std::move(value());
        }

        return std::invoke(std::forward<Fn>(f), std::move(error()));
    }
};

template <typename T, typename E> requires std::is_void_v<T>
class expected<T, E> {
    struct void_placeholder {
        constexpr void_placeholder() noexcept = default;
    };

    union {
        void_placeholder void_{};
        E err_;
    };
    bool has_value_{true};

    static_assert(details::can_be_unexpected<E>);

    template <typename U, typename G>
    constexpr static bool constructed_from_expected_v = std::disjunction_v<
        std::is_constructible<unexpected<E>, expected<U, G>>,
        std::is_constructible<unexpected<E>, expected<U, G> &>,
        std::is_constructible<unexpected<E>, expected<U, G> const>,
        std::is_constructible<unexpected<E>, expected<U, G> const &>
    >;

    template <typename U>
    constexpr static bool is_expected_with_same_value_type_v = details::is_expected_v<U> && std::is_same_v<T, typename U::value_type>;

    template <typename U>
    constexpr static bool is_expected_with_same_error_type_v = details::is_expected_v<U> && std::is_same_v<E, typename U::error_type>;

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected <E>;

    // Constructor

    constexpr
    expected() = default;

    constexpr
    expected(expected const &) = default;

    constexpr
    expected(expected const & other) noexcept(std::is_nothrow_copy_constructible_v<E>) requires std::is_copy_constructible_v<E> && (!std::is_trivially_copy_constructible_v<E>)
        : void_{}, has_value_{ other.has_value_ } {
        if (!has_value_) {
            std::construct_at(std::addressof(err_), other.err_);
        }
    }

    expected(expected &&) = default;

    expected(expected && other) noexcept(std::is_nothrow_move_constructible_v<E>) requires std::is_move_constructible_v<E> && (!std::is_trivially_move_constructible_v<E>)
        : void_{}, has_value_{ other.has_value_ } {
        if (!has_value_) {
            std::construct_at(std::addressof(err_), std::move(other.err_));
        }
    }

    template <typename U, typename G> requires std::is_void_v<U> && std::is_constructible_v<E, G const &> && (!constructed_from_expected_v<U, G>)
    constexpr explicit(!std::is_convertible_v<G const &, E>)
    expected(expected<U, G> const & other) noexcept(std::is_nothrow_constructible_v<E, G const &>)
        : void_{}, has_value_{ other.has_value_ } {
        if (!has_value_) {
            std::construct_at(std::addressof(err_), other.err_);
        }
    }

    template <typename U, typename G> requires std::is_void_v<U> && std::is_constructible_v<E, G> && (!constructed_from_expected_v<U, G>)
    constexpr explicit(!std::is_convertible_v<G, E>)
    expected(expected<U, G> && other) noexcept(std::is_nothrow_constructible_v<E, G>)
        : void_{}, has_value_{ other.has_value_ } {
        if (!has_value_) {
            std::construct_at(std::addressof(err_), std::move(other.err_));
        }
    }

    template <typename G> requires std::is_constructible_v<E, G const &>
    constexpr explicit(!std::is_convertible_v<G const &, E>)
    expected(unexpected <G> const & e) : err_{ e.error() }, has_value_{ false } {
    }

    template <typename G> requires std::is_constructible_v<E, G>
    constexpr explicit(!std::is_convertible_v<G, E>)
    expected(unexpected <G> && e) : err_{ std::move(e).error() }, has_value_{ false } {
    }

    constexpr explicit
    expected(std::in_place_t) noexcept
        : expected{} {
    }

    template<typename ... Args> requires std::is_constructible_v<E, Args...>
    constexpr explicit
    expected(abc::unexpect_t, Args &&... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
    : err_{ std::forward<Args>(args)... }, has_value_{ false } {
    }

    template<typename U, typename ... Args> requires std::is_constructible_v<E, std::initializer_list<U> &, Args...>
    constexpr explicit
    expected(abc::unexpect_t, std::initializer_list<U> il, Args &&... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U> &, Args...>)
        : err_{ il, std::forward<Args>(args)... }, has_value_{ false } {
    }

private:
    template<typename, typename> friend class expected;

    template <typename Fn>
    explicit
    expected(details::in_place_val_fn_t, Fn && f)
        : void_{}, has_value_{ true } {
        std::forward<Fn>(f)();
    }

    template <typename Fn>
    explicit
    expected(details::in_place_err_fn_t, Fn && f)
        : err_{ std::forward<Fn>(f)() }, has_value_{ false } {
    }
public:

    // Destructor

    constexpr ~expected() = default;

    constexpr ~expected() requires (!std::is_trivially_destructible_v<E>) {
        if (!has_value_)
            std::destroy_at(__builtin_addressof(err_));
    }

    // assignment

    auto operator=(expected const &) -> expected & = delete;

    constexpr auto
    operator=(expected const & rhs) noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<E>, std::is_nothrow_copy_assignable<E>>) -> expected &
                                    requires std::is_copy_constructible_v<E> && std::is_copy_assignable_v<E>
    {
        if (rhs.has_value_) {
            emplace();
        } else {
            this->assign_err(rhs.err_);
        }
        return *this;
    }

    constexpr auto
    operator=(expected && rhs) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<E>, std::is_nothrow_move_assignable<E>>) -> expected &
                               requires std::is_move_constructible_v<E> && std::is_move_assignable_v<E>
    {
        if (rhs.has_value_) {
            emplace();
        } else {
            this->assign_err(std::move(rhs.err_));
        }
        return *this;
    }

    template<typename G> requires std::is_constructible_v<E, G const &> && std::is_assignable_v<E &, G const &>
    constexpr auto
    operator=(unexpected<G> const & rhs) -> expected & {
        this->assign_err(rhs.error());
        return *this;
    }

    template<typename G> requires std::is_constructible_v<E, G> && std::is_assignable_v<E &, G>
    constexpr auto
    operator=(unexpected<G> && rhs) -> expected & {
        this->assign_err(std::move(rhs).error());
        return *this;
    }

    // modifiers

    constexpr auto
    emplace() noexcept -> void {
        if (!has_value_) {
            std::destroy_at(std::addressof(err_));
            has_value_ = true;
        }
    }

    // swap

    constexpr void
    swap(expected & rhs) noexcept(std::conjunction_v<std::is_nothrow_swappable<E &>, std::is_nothrow_move_constructible<E>>)
                         requires std::is_swappable_v<E> && std::is_move_constructible_v<E> {
        if (has_value_) {
            if (!rhs.has_value_) {
                std::construct_at(std::addressof(err_),std::move(rhs.err_)); // might throw
                std::destroy_at(std::addressof(rhs.err_));
                has_value_ = false;
                rhs.has_value_ = true;
            }
        } else {
            if (rhs.has_value_) {
                std::construct_at(std::addressof(rhs.err_), std::move(err_)); // might throw
                std::destroy_at(std::addressof(err_));
                has_value_ = true;
                rhs.has_value_ = false;
            } else {
                using std::swap;
                swap(err_, rhs.err_);
            }
        }
    }

    friend constexpr void
    swap(expected & lhs, expected & rhs) noexcept(noexcept(lhs.swap(rhs)))
                                         requires requires{ lhs.swap(rhs); } {
        lhs.swap(rhs);
    }

    // observers

    [[nodiscard]] constexpr explicit
    operator bool() const noexcept {
        return has_value_;
    }

    constexpr auto
    has_value() const noexcept -> bool {
        return has_value_;
    }

    constexpr void
    operator*() const noexcept {
        assert(has_value_);
    }

    constexpr void
    value() const & {
        if (has_value_) [[likely]] {
            return;
        }

        throw_error(make_error_code(errc::bad_expected_access));
        abc::unreachable();
    }

    constexpr void
    value() && {
        if (has_value_) [[likely]] {
            return;
        }

        throw_error(make_error_code(errc::bad_expected_access));
        abc::unreachable();
    }

    constexpr auto
    error() const & noexcept -> E const & {
        assert(!has_value_);
        return err_;
    }

    constexpr auto
    error() & noexcept -> E & {
        assert(!has_value_);
        return err_;
    }

    constexpr auto
    error() const && noexcept -> E const && {
        assert(!has_value_);
        return std::move(err_);
    }

    constexpr auto
    error() && noexcept -> E && {
        assert(!has_value_);
        return std::move(err_);
    }

    // monadic operations

private:
    template <typename Self, typename Fn>
    constexpr static auto
    and_then_impl(Self && self, Fn && f) -> details::expected_t<Fn> {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);
        static_assert(std::is_invocable_v<Fn>);

        using U = details::expected_t<Fn>;
        static_assert(is_expected_with_same_error_type_v<U>);

        if (self.has_value()) {
            return std::invoke(std::forward<Fn>(f));
        }

        return U{ unexpect, std::forward<Self>(self).error() };
    }

public:
    template<typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    and_then(Fn && f) & -> details::expected_t<Fn> {
        return and_then_impl(*this, std::forward<Fn>(f));
    }

    template<typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    and_then(Fn && f) const & -> details::expected_t<Fn> {
        return and_then_impl(*this, std::forward<Fn>(f));
    }

    template<typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    and_then(Fn && f) && -> details::expected_t<Fn> {
        return and_then_impl(std::move(*this), std::forward<Fn>(f));
    }

    template<typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    and_then(Fn && f) const && -> details::expected_t<Fn> {
        return and_then_impl(std::move(*this), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename Fn>
    constexpr static auto
    or_else_impl(Self && self, Fn && f) -> details::expected_t<Fn, decltype(std::forward<Self>(self).error())> {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);
        static_assert(std::is_invocable_v<Fn, decltype(std::forward<Self>(self).error())>);

        using U = details::expected_t<Fn, decltype(std::forward<Self>(self).error())>;
        static_assert(is_expected_with_same_value_type_v<U>);

        if (!self.has_value()) {
            return std::invoke(std::forward<Fn>(f), std::forward<Self>(self).error());
        }

        return U{ std::in_place, std::forward<Self>(self).value() };
    }

public:
    template<typename Fn>
    constexpr auto
    or_else(Fn && f) & -> details::expected_t<Fn, E &> {
        return or_else_impl(*this, std::forward<Fn>(f));
    }

    template<typename Fn>
    constexpr auto
    or_else(Fn && f) const & -> details::expected_t<Fn, E const &> {
        return or_else_impl(*this, std::forward<Fn>(f));
    }

    template<typename Fn>
    constexpr auto
    or_else(Fn && f) && -> details::expected_t<Fn, E &&> {
        return or_else_impl(std::move(*this), std::forward<Fn>(f));
    }

    template<typename Fn>
    constexpr auto
    or_else(Fn && f) const && -> details::expected_t<Fn, E const &&> {
        return or_else_impl(std::move(*this), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename Fn>
    constexpr static auto
    transform_impl(Self && self, Fn && f) -> expected<details::transformed_decay_type<Fn>, E> {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);
        static_assert(std::is_invocable_v<Fn>);

        using U = details::transformed_type<Fn>;
        static_assert(!details::is_expected_v<U>);

        using result_type = expected<details::transformed_decay_type<Fn>, E>;

        if (self.has_value()) {
            return result_type{ details::in_place_val_fn, [&]() { return std::invoke(std::forward<Fn>(f)); } };
        }

        return result_type{ unexpect, std::forward<Self>(self).error() };
    }

public:
    template<typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    transform(Fn && f) & -> expected<details::transformed_decay_type<Fn>, E> {
        return transform_impl(*this, std::forward<Fn>(f));
    }

    template<typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    transform(Fn && f) const & -> expected<details::transformed_decay_type<Fn>, E> {
        return transform_impl(*this, std::forward<Fn>(f));
    }

    template<typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    transform(Fn && f) && -> expected<details::transformed_decay_type<Fn>, E> {
        return transform_impl(std::move(*this), std::forward<Fn>(f));
    }

    template<typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    transform(Fn && f) const && -> expected<details::transformed_decay_type<Fn>, E> {
        return transform_impl(std::move(*this), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename Fn>
    constexpr static auto
    transform_error_impl(Self && self, Fn && f) -> expected<T, details::transformed_decay_type<Fn, decltype(std::forward<Self>(self).error())>> {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);
        static_assert(std::is_invocable_v<Fn, decltype(std::forward<Self>(self).error())>);

        using U = details::transformed_type<Fn, decltype(std::forward<Self>(self).error())>;
        static_assert(!details::is_expected_v<U>);

        using result_type = expected<T, details::transformed_decay_type<Fn, decltype(std::forward<Self>(self).error())>>;

        if (!self.has_value()) {
            return result_type{ unexpect, std::invoke(std::forward<Fn>(f), std::forward<Self>(self).error()) };
        }

        return result_type{ std::in_place };
    }

public:
    template<typename Fn>
    constexpr auto
    transform_error(Fn && f) & -> expected<T, details::transformed_decay_type<Fn, E &>> {
        return transform_error_impl(*this, std::forward<Fn>(f));
    }

    template<typename Fn>
    constexpr auto
    transform_error(Fn && f) const & -> expected<T, details::transformed_decay_type<Fn, E const &>> {
        return transform_error_impl(*this, std::forward<Fn>(f));
    }

    template<typename Fn>
    constexpr auto
    transform_error(Fn && f) && -> expected<T, details::transformed_decay_type<Fn, E &&>> {
        return transform_error_impl(std::move(*this), std::forward<Fn>(f));
    }

    template<typename Fn>
    constexpr auto
    transform_error(Fn && f) const && -> expected<T, details::transformed_decay_type<Fn, E const &&>> {
        return transform_error_impl(std::move(*this), std::forward<Fn>(f));
    }

    // equality operators

    template<typename U, typename G> requires std::is_void_v<U>
    [[nodiscard]] friend constexpr auto
    operator==(expected const & lhs, expected<U, G> const & rhs) noexcept(noexcept(lhs.has_value_ == rhs.has_value_)) -> bool {
        if (lhs.has_value_) {
            return rhs.has_value_;
        } else {
            return !rhs.has_value_ && lhs.err_ == rhs.err_;
        }
    }

    template<typename G>
    [[nodiscard]] friend constexpr auto
    operator==(expected const & lhs, unexpected<G> const & rhs) noexcept(noexcept(lhs.err_ == rhs.error())) -> bool {
        return !lhs.has_value_ && lhs.err_ == rhs.error();
    }

    // Rust APIs

    [[nodiscard]] constexpr auto
    is_ok() const noexcept -> bool {
        return has_value();
    }

    template <typename Fn>
    [[nodiscard]] constexpr auto
    is_ok_and(Fn && f) const -> bool {
        static_assert(std::is_invocable_v<Fn>);

        return is_ok() && std::invoke(std::forward<Fn>(f));
    }

    [[nodiscard]] constexpr auto
    is_err() const noexcept -> bool {
        return !has_value();
    }

    template <typename Fn>
    [[nodiscard]] constexpr auto
    is_err_and(Fn && f) const -> bool {
        static_assert(std::is_invocable_v<Fn, E const &>);
        static_assert(std::is_convertible_v<std::invoke_result_t<Fn, E const &>, bool>);

        return is_err() && std::invoke(std::forward<Fn>(f), err_);
    }

private:
    template <typename Self>
    constexpr static auto
    ok_impl(Self && self) -> std::optional<T> {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);

        if (self.is_ok()) {
            return std::make_optional<T>();
        }

        return std::nullopt;
    }

public:
    [[nodiscard]] constexpr auto
    ok() const -> std::optional<T> {
        if (is_ok()) {
            return std::make_optional<T>();
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    err() & -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(err_);
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    err() const & -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(err_);
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    err() && -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(std::move(err_));
        }

        return std::nullopt;
    }

    [[nodiscard]] constexpr auto
    err() const && -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(std::move(err_));
        }

        return std::nullopt;
    }

    template <typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    map(Fn && f) & -> expected<details::transformed_decay_type<Fn>, E> {
        return transform_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_copy_constructible_v<E>
    constexpr auto
    map(Fn && f) const & -> expected<details::transformed_decay_type<Fn>, E> {
        return transform_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    map(Fn && f) && -> expected<details::transformed_decay_type<Fn>, E> {
        return transform_impl(std::move(*this), std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<E>
    constexpr auto
    map(Fn && f) const && -> expected<details::transformed_decay_type<Fn>, E> {
        return transform_impl(std::move(*this), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename U, typename Fn>
    constexpr static auto
    map_or_impl(Self && self, U && default_value, Fn && f) -> U {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);
        static_assert(std::is_invocable_v<Fn>);

        if (self.is_ok()) {
            return std::invoke(std::forward<Fn>(f));
        }

        return std::forward<U>(default_value);
    }

public:
    template <typename U, typename Fn>
    [[nodiscard]] constexpr auto
    map_or(U && default_value, Fn && f) & -> U {
        return map_or_impl(*this, std::forward<U>(default_value), std::forward<Fn>(f));
    }

    template <typename U, typename Fn>
    [[nodiscard]] constexpr auto
    map_or(U && default_value, Fn && f) const & -> U {
        return map_or_impl(*this, std::forward<U>(default_value), std::forward<Fn>(f));
    }

    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T &&>
    [[nodiscard]] constexpr auto
    map_or(U && default_value, Fn && f) && -> U {
        return map_or_impl(std::move(*this), std::forward<U>(default_value), std::forward<Fn>(f));
    }

    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T const &&>
    [[nodiscard]] constexpr auto
    map_or(U && default_value, Fn && f) const && -> U {
        return map_or_impl(std::move(*this), std::forward<U>(default_value), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename U, typename ErrFn, typename ValFn>
    constexpr static auto
    map_or_else_impl(Self && self, ErrFn && err_fn, ValFn && val_fn) -> U {
        static_assert(std::is_same_v<expected, std::remove_cvref_t<Self>>);

        if (self.is_ok()) {
            return static_cast<U>(std::invoke(std::forward<ValFn>(val_fn)));
        }

        return static_cast<U>(std::invoke(std::forward<ErrFn>(err_fn), std::forward<Self>(self).error()));
    }

public:
    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_v<ErrFn, E &> && std::is_invocable_v<ValFn, T &>
    [[nodiscard]] constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) & -> U {
        static_assert(std::is_convertible_v<std::invoke_result_t<ErrFn, E &>, U>);
        static_assert(std::is_convertible_v<std::invoke_result_t<ValFn, T &>, U>);

        return map_or_else_impl(*this, std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_v<ErrFn, E const &> && std::is_invocable_v<ValFn, T const &>
    [[nodiscard]] constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) const & -> U {
        static_assert(std::is_convertible_v<std::invoke_result_t<ErrFn, E const &>, U>);
        static_assert(std::is_convertible_v<std::invoke_result_t<ValFn, T const &>, U>);

        return map_or_else_impl(*this, std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_v<ErrFn, E &&> && std::is_invocable_v<ValFn, T &&>
    [[nodiscard]] constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) && -> U {
        static_assert(std::is_convertible_v<std::invoke_result_t<ErrFn, E &&>, U>);
        static_assert(std::is_convertible_v<std::invoke_result_t<ValFn, T &&>, U>);

        return map_or_else_impl(std::move(*this), std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_v<ErrFn, E const &&> && std::is_invocable_v<ValFn, T const &&>
    [[nodiscard]] constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) const && -> U {
        static_assert(std::is_convertible_v<std::invoke_result_t<ErrFn, E const &&>, U>);
        static_assert(std::is_convertible_v<std::invoke_result_t<ValFn, T const &&>, U>);

        return map_or_else_impl(std::move(*this), std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

public:
    template <typename Fn> requires std::is_copy_constructible_v<T>
    constexpr auto
    map_err(Fn && f) & -> expected<T, details::transformed_decay_type<Fn, E &>> {
        return transform_error_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_copy_constructible_v<T>
    constexpr auto
    map_err(Fn && f) const & -> expected<T, details::transformed_decay_type<Fn, E const &>> {
        return transform_error_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<T>
    constexpr auto
    map_err(Fn && f) && -> expected<T, details::transformed_decay_type<Fn, E &&>> {
        return transform_error_impl(std::move(*this), std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_move_constructible_v<T>
    constexpr auto
    map_err(Fn && f) const && -> expected<T, details::transformed_decay_type<Fn, E const &&>> {
        return transform_error_impl(std::move(*this), std::forward<Fn>(f));
    }

    [[nodiscard]] constexpr auto
    expect(std::string_view msg) & noexcept -> T {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return value();
    }

    [[nodiscard]] constexpr auto
    expect(std::string_view msg) const & noexcept -> T {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return value();
    }

    [[nodiscard]] constexpr auto
    expect(std::string_view msg) && noexcept -> T {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return value();
    }

    [[nodiscard]] constexpr auto
    expect(std::string_view msg) const && noexcept -> T {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return value();
    }

    [[nodiscard]] constexpr auto
    unwrap() const & noexcept -> T {    // mark noexcept, force panic if error occurs.
        return expect("expected should have value when calling `unwrap()`");
    }

    [[nodiscard]] constexpr auto
    unwrap() && noexcept -> T {    // mark noexcept, force panic if error occurs.
        return std::move(expect("expected should have value when calling `unwrap()`"));
    }

    [[nodiscard]] constexpr auto
    unwrap_or_default() const & noexcept -> T {
        static_assert(std::is_default_constructible_v<T>);
        static_assert(std::is_copy_constructible_v<T>);

        if (is_err()) {
            return T{};
        }

        return value();
    }

    [[nodiscard]] constexpr auto
    unwrap_or_default() && noexcept -> T {
        static_assert(std::is_default_constructible_v<T>);
        static_assert(std::is_move_constructible_v<T>);

        if (is_err()) {
            return T{};
        }

        return value();
    }

    [[nodiscard]] constexpr auto
    expect_err(std::string_view msg) & noexcept -> E & {    // mark noexcept, force panic if expected has value.
        if (is_ok()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return error();
    }

    [[nodiscard]] constexpr auto
    expect_err(std::string_view msg) const & noexcept -> E const & {    // mark noexcept, force panic if expected has value.
        if (is_ok()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return error();
    }

    [[nodiscard]] constexpr auto
    expect_err(std::string_view msg) && noexcept -> E && {    // mark noexcept, force panic if expected has value.
        if (is_ok()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return std::move(error());
    }

    [[nodiscard]] constexpr auto
    expect_err(std::string_view msg) const && noexcept -> E const && {  // mark noexcept, force panic if expected has value.
        if (is_ok()) {
            abc::throw_error(errc::bad_expected_access, msg);
        }

        return std::move(error());
    }

    [[nodiscard]] constexpr auto
    unwrap_err() & noexcept -> E & {    // mark noexcept, force panic if expected has value.
        return expect_err(value().to_string());
    }

    [[nodiscard]] constexpr auto
    unwrap_err() const & noexcept -> E const & {    // mark noexcept, force panic if expected has value.
        return expect_err(value().to_string());
    }

    [[nodiscard]] constexpr auto
    unwrap_err() && noexcept -> E && {
        return expect_err(value().to_string());
    }

    [[nodiscard]] constexpr auto
    unwrap_err() const && noexcept -> E const && {
        return expect_err(value().to_string());
    }

    template <typename U = T> requires std::is_copy_constructible_v<T> && std::is_convertible_v<U, T>
    [[nodiscard]] constexpr auto
    unwrap_or(U && default_value) const & -> T {
        return value_or(std::forward<U>(default_value));
    }

    template <typename U> requires std::is_move_constructible_v<T> && std::is_constructible_v<U, T>
    [[nodiscard]] constexpr auto
    unwrap_or(U && default_value) && -> T {
        return value_or(std::forward<U>(default_value));
    }

    template <typename Fn>
    [[nodiscard]] constexpr auto
    unwrap_or_else(Fn && f) const & -> T requires std::is_copy_constructible_v<T> {
        using U = details::transformed_type<Fn, E const &>;

        static_assert(!details::is_expected_v<U>);
        static_assert(std::is_invocable_v<Fn, E const &>);
        static_assert(std::is_convertible_v<U, T>);

        if (is_ok()) {
            return value();
        }

        return std::invoke(std::forward<Fn>(f), error());
    }

    template <typename Fn>
    [[nodiscard]] constexpr auto
    unwrap_or_else(Fn && f) && -> T requires std::is_move_constructible_v<T> {
        using U = details::transformed_type<Fn, E &&>;

        static_assert(!details::is_expected_v<U>);
        static_assert(std::is_invocable_v<Fn, E &&>);
        static_assert(std::is_convertible_v<U, T>);

        if (is_ok()) {
            return std::move(value());
        }

        return std::invoke(std::forward<Fn>(f), std::move(error()));
    }

private:
    template<typename V>
    constexpr auto
    assign_err(V && v) -> void {
        if (has_value_) {
            std::construct_at(std::addressof(err_), std::forward<V>(v));
            has_value_ = false;
        }
        else {
            err_ = std::forward<V>(v);
        }
    }
};



}   // namespace abc

#endif //ABC_EXPECTED
