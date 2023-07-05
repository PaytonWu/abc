// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_RESULT)
#define ABC_RESULT

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
class [[nodiscard]] err;

template <typename T, typename E>
class [[nodiscard]] result;

namespace details {

template <typename E>
constexpr bool is_err_v = false;

template <typename E>
constexpr bool is_err_v<abc::err<E>> = true;

template <typename E>
concept can_be_err = std::is_object_v<E> && (!std::is_array_v<E>) && (!is_err_v<E>) && (!std::is_const_v<E>) && (!std::is_volatile_v<E>);

template <typename T>
constexpr bool is_result_v = false;

template <typename T, typename E>
constexpr bool is_result_v<abc::result<T, E>> = true;

template <typename Fn, typename T>
using fn_map_t = std::remove_cvref_t<std::invoke_result_t<Fn &&, T &&>>;

template <typename T>
struct exception_safe_guard {
    static_assert(std::is_nothrow_move_constructible_v<T>);

    constexpr explicit
    exception_safe_guard(T & o) : original_address_{ std::addressof(o) }, tmp_{ std::move(o) } {
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

template <typename T, typename U, typename ... Args>
constexpr void reinit_result(T * new_value, U * old_value, Args && ... args) {
    if constexpr (std::is_nothrow_constructible_v<T, Args...>) {
        std::destroy_at(old_value);
        std::construct_at(new_value, std::forward<Args>(args)...);
    } else if constexpr (std::is_nothrow_move_constructible_v<T>) {
        T tmp{ std::forward<Args>(args)... };
        std::destroy_at(old_value);
        std::construct_at(new_value, std::move(tmp));
    } else {
        exception_safe_guard<U> guard{ std::move(*old_value) };
        std::construct_at(new_value, std::forward<Args>(args)...);
        [[maybe_unused]] auto _ = guard.release();
    }
}

}

}

namespace abc {

template <typename E>
class [[nodiscard]] err {
    static_assert(details::can_be_err<E>);

private:
    E err_;

public:
    constexpr err() noexcept = default;
    constexpr err(err const & other) = default;
    constexpr err(err && other) = default;
    constexpr auto operator=(err const & other) -> err & = default;
    constexpr auto operator=(err && other) -> err & = default;
    ~err() = default;

    constexpr err(E const & e) noexcept(std::is_nothrow_copy_constructible_v<E>) : err_{ e } {}
    constexpr err(E && e) noexcept(std::is_nothrow_move_constructible_v<E>) : err_{ std::move(e) } {}

    template <typename G = E> requires (!std::is_same_v<std::remove_cvref_t<G>, err>) && (!std::is_same_v<std::remove_cvref_t<G>, std::in_place_t>) && std::is_constructible_v<E, G>
    constexpr explicit err(G && e) noexcept(std::is_nothrow_constructible_v<E, G>)
        : err_{ std::forward<G>(e) } {
    }

    template <typename ... Args>
    requires std::is_constructible_v<E, Args...>
    constexpr explicit err(std::in_place_t, Args && ... args) noexcept(std::is_nothrow_constructible_v<E, Args...>)
        : err_{ std::forward<Args>(args)... } {
    }

    template <typename U, typename ... Args> requires std::is_constructible_v<E, std::initializer_list<U>, Args...>
    constexpr explicit err(std::in_place_t,
                           std::initializer_list<U> il,
                           Args && ... args) noexcept(std::is_nothrow_constructible_v<E, std::initializer_list<U>, Args...>)
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

    constexpr void swap(err & other) noexcept(std::is_nothrow_swappable_v<E>) {
        using std::swap;
        swap(err_, other.err_);
    }

    template <typename G>
    friend constexpr auto operator==(err const & lhs, err<G> const & rhs) noexcept -> bool {
        return lhs.err_ == rhs.error();
    }

    friend constexpr auto
    swap(err & lhs, err & rhs) noexcept(noexcept(lhs.swap(rhs))) -> void requires std::is_swappable_v<E> {
        lhs.swap(rhs);
    }
};

template <typename E>
err(E) -> err<E>;

template <typename T, typename E>
class result {
private:
    union {
        T val_;
        E err_;
    };
    bool has_value_;

    static_assert(!std::is_reference_v<T>);
    static_assert(!std::is_function_v<T>);
    static_assert(!std::is_same_v<std::remove_cv_t<T>, std::in_place_t>);
    static_assert(!details::is_err_v<std::remove_cvref_t<T>>);
    static_assert(details::can_be_err<E>);

    template <typename U, typename G>
    constexpr static bool constructed_from_result_v = std::disjunction_v<
        std::is_constructible<T, result<U, G> &>,
        std::is_constructible<T, result<U, G>>,
        std::is_constructible<T, result<U, G> const &>,
        std::is_constructible<T, result<U, G> const>,
        std::is_convertible<result<U, G> &, T>,
        std::is_convertible<result<U, G>, T>,
        std::is_convertible<result<U, G> const &, T>,
        std::is_convertible<result<U, G> const, T>,
        std::is_constructible<err<E>, result<U, G>>,
        std::is_constructible<err<E>, result<U, G> &>,
        std::is_constructible<err<E>, result<U, G> const>,
        std::is_constructible<err<E>, result<U, G> const &>
    >;

    template<typename U, typename G>
    constexpr static bool explicit_convert_v = std::disjunction_v<
        std::negation<std::is_convertible<U, T>>,
        std::negation<std::is_convertible<G, E>>
    >;

    template <typename U>
    constexpr auto
    assign_value(U && v) -> void {
        if (has_value_) {
            val_ = std::forward<U>(v);
        } else {
            details::reinit_result(std::addressof(val_), std::addressof(err_), std::forward<U>(v));
            has_value_ = true;
        }

        assert(this->has_value_);
    }

    template <typename G>
    constexpr auto
    assign_error(G && e) -> void {
        if (has_value_) {
            details::reinit_result(std::addressof(err_), std::addressof(val_), std::forward<G>(e));
            has_value_ = false;
        } else {
            err_ = std::forward<G>(e);
        }

        assert(!this->has_value_);
    }

    constexpr auto
    swap_value_error(result & rhs) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<T>>) -> void {
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
    using err_t = err<E>;

    template<typename U>
    using rebind = result<U, E>;

    constexpr
    result() noexcept(std::is_nothrow_constructible_v<T>) requires std::is_default_constructible_v<T> : val_{}, has_value_{ true } {}

    // Copy constructors

    // Eligible default constructor: https://en.cppreference.com/w/cpp/language/default_constructor#Eligible_default_constructor
    constexpr
    result(result const &) = default;

    constexpr
    result(result const & other) noexcept(std::conjunction_v<std::is_nothrow_copy_constructible<T>, std::is_nothrow_copy_constructible<E>>)
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
    result(result &&) = default;

    constexpr
    result(result && other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<E>>)
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
    template <typename U, typename G> requires std::is_constructible_v<T, U const &> && std::is_constructible_v<E, G const &> && (!constructed_from_result_v<U, G>)
    constexpr explicit(explicit_convert_v<U const &, G const &>)
    result(result<U, G> const & other) // noexcept(std::conjunction_v<std::is_nothrow_constructible<T, U const &>, std::is_nothrow_constructible<E, G const &>>)
        : has_value_{ other.has_value_ } {
        if (has_value_) {
            std::construct_at(std::addressof(val_), other.val_);
        } else {
            std::construct_at(std::addressof(err_), other.err_);
        }
    }

    template <typename U, typename G> requires std::is_constructible_v<T, U> && std::is_constructible_v<E, G> && (!constructed_from_result_v<U, G>)
    constexpr explicit(explicit_convert_v<U, G>)
    result(result<U, G> && other) // noexcept(std::conjunction_v<std::is_nothrow_constructible<T, U>, std::is_nothrow_constructible<E, G>>)
        : has_value_{ other.has_value_ } {
        if (has_value_) {
            std::construct_at(std::addressof(val_), std::move(other).val_);
        } else {
            std::construct_at(std::addressof(err_), std::move(other).err_);
        }
    }

    template<typename U = T> requires (!std::is_same_v<std::remove_cvref_t<U>, result>) &&
                                      (!std::is_same_v<std::remove_cvref_t<U>, std::in_place_t>) &&
                                      (!details::is_err_v<std::remove_cvref_t<U>>) &&
                                      std::is_constructible_v<T, U>
    constexpr explicit(!std::is_convertible_v<U, T>) result(U && v) // noexcept(std::is_nothrow_constructible_v<T, U>)
        : val_{ std::forward<U>(v) }, has_value_{ true } {
    }

    template<typename G = E> requires std::is_constructible_v<E, G const &>
    constexpr explicit(!std::is_convertible_v<G const &, E>)
    result(err<G> const & e) noexcept(std::is_nothrow_constructible_v<E, G const &>)
        : err_{ e.error() }, has_value_{ false } {
    }

    template<typename G = E> requires std::is_constructible_v<E, G>
    constexpr explicit(!std::is_convertible_v<G, E>)
    result(err<G> && e) noexcept(std::is_nothrow_constructible_v<E, G>)
        : err_{ std::move(e).error() }, has_value_{ false } {
    }

    // Destructor
    // Prospective destructor: https://en.cppreference.com/w/cpp/language/destructor#Prospective_destructor
    constexpr
    ~result() = default;

    constexpr
    ~result() requires (!std::is_trivially_destructible_v<T>) || (!std::is_trivially_destructible_v<E>) {
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
    operator=(result const & other) -> result & = delete;

    constexpr auto
    operator=(result const & other) -> result & requires std::is_copy_assignable_v<T> && std::is_copy_constructible_v<T> && std::is_copy_assignable_v<E> && std::is_copy_constructible_v<E> && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>) {
        if (other.has_value_) {
            this->assign_value(other.val_);
        } else {
            this->assign_error(other.err_);
        }

        return *this;
    }

    constexpr auto
    operator=(result && other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_assignable<T>, std::is_nothrow_move_constructible<E>, std::is_nothrow_move_assignable<E>>) -> result &
                               requires std::is_move_constructible_v<T> && std::is_move_assignable_v<T> && std::is_move_constructible_v<E> && std::is_move_assignable_v<E> && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>) {
        if (other.has_value_) {
            this->assign_value(std::move(other.val_));
        } else {
            this->assign_error(std::move(other.err_));
        }

        return *this;
    }

    template <typename U = T> requires (!std::is_same_v<result, std::remove_cvref_t<U>>) && (!details::is_err_v<U>) && std::is_constructible_v<T, U> && (std::is_nothrow_constructible_v<T, U> || std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
    constexpr auto
    operator=(U && v) -> result & {
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
    operator=(err<G> const & e) -> result & {
        this->assign_error(e.error());
        return *this;
    }

    template <typename G> requires std::is_constructible_v<E, G> &&
                                   std::is_assignable_v<E &, G> &&
                                   (std::is_nothrow_constructible_v<E, G> || std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
    constexpr auto
    operator=(err<G> && e) -> result & {
        this->assign_error(std::move(e).error());
        return *this;
    }

    // Swap

    constexpr void
    swap(result & other) noexcept(std::conjunction_v<std::is_nothrow_move_constructible<T>, std::is_nothrow_move_constructible<E>, std::is_nothrow_swappable<T &>, std::is_nothrow_swappable<E &>>)
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
    swap(result & lhs, result & rhs) noexcept(noexcept(lhs.swap(rhs))) {
        lhs.swap(rhs);
    }

    // Equality operator

    template <typename T2, typename E2> requires (!std::is_void_v<T2>)
    friend constexpr auto
    operator==(result const & lhs, result<T2, E2> const & rhs) -> bool {
        if (lhs.has_value()) {
            return rhs.has_value() && (lhs.value() == rhs.value());
        } else {
            return !rhs.has_value() && (lhs.error() == rhs.error());
        }
    }

    template <typename T2, typename E2> requires std::is_void_v<T2>
    friend constexpr auto
    operator==(result const & lhs, result<T2, E2> const & rhs) -> bool {
        if (lhs.has_value()) {
            return rhs.has_value();
        } else {
            return !rhs.has_value()  && (lhs.error() == rhs.error());
        }
    }

    template <typename T2>
    friend constexpr auto
    operator==(result const & x, T2 const & val) -> bool {
        return x.has_value() && (x.value() == val);
    }

    template <typename E2>
    friend constexpr auto
    operator==(result const & x, err<E2> const & e) -> bool {
        return !x.has_value() && (x.error() == e.error());
    }

    // Observers

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

        throw_error(make_error_code(errc::bad_result_access));
        abc::unreachable();
    }

    [[nodiscard]]
    constexpr auto
    value() & -> T & {
        if (has_value()) [[likely]] {
            return val_;
        }

        throw_error(make_error_code(errc::bad_result_access));
        abc::unreachable();
    }

    [[nodiscard]]
    constexpr auto
    value() const && -> T const && {
        if (has_value()) [[likely]] {
            return std::move(val_);
        }

        throw_error(make_error_code(errc::bad_result_access));
        abc::unreachable();
    }

    [[nodiscard]]
    constexpr auto
    value() && -> T && {
        if (has_value()) [[likely]] {
            return std::move(val_);
        }

        throw_error(make_error_code(errc::bad_result_access));
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

    // Monadic operations

    // Rust APIs

    constexpr auto
    is_ok() const noexcept -> bool {
        return has_value();
    }

    template <typename Fn>
    constexpr auto
    is_ok_and(Fn && f) const -> bool {
        static_assert(std::is_invocable_r_v<bool, Fn, T>);

        return has_value() && std::invoke(std::forward<Fn>(f), val_);
    }

    constexpr auto
    is_err() const noexcept -> bool {
        return !has_value();
    }

    template <typename Fn> requires std::is_invocable_r_v<bool, Fn, E>
    constexpr auto
    is_err_and(Fn && f) const -> bool {
        return is_err() && std::invoke(std::forward<Fn>(f), err_);
    }

private:
    template <typename Self>
    constexpr static auto
    ok_impl(Self && self) -> std::optional<T> {
        static_assert(std::is_same_v<result, std::remove_cvref_t<Self>>);
        static_assert(std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>);

        if (self.is_ok()) {
            return std::make_optional<T>(std::forward<Self>(self).value());
        }

        return std::nullopt;
    }

#if defined(__cpp_explicit_this_parameter) && ((__cpp_explicit_this_parameter) >= 202110L)
public:
    constexpr auto
    ok(this result & self)  -> std::optional<T> requires std::is_copy_constructible_v<T> {
        return ok_impl(self);
    }

    constexpr auto
    ok(this result const & self) -> std::optional<T> requires std::is_copy_constructible_v<T> {
        return ok_impl(self);
    }

    constexpr auto
    ok(this result const && self) -> std::optional<T> requires std::is_move_constructible_v<T> {
        return ok_impl(std::move(self));
    }

    constexpr auto
    ok(this result && self) -> std::optional<T> requires std::is_move_constructible_v<T> {
        return ok_impl(std::move(self));
    }
#else
public:
    constexpr auto
    ok() & -> std::optional<T> requires std::is_copy_constructible_v<T> {
        return ok_impl(*this);
    }

    constexpr auto
    ok() const & -> std::optional<T> requires std::is_copy_constructible_v<T> {
        return ok_impl(*this);
    }

    constexpr auto
    ok() && -> std::optional<T> requires std::is_move_constructible_v<T> {
        return ok_impl(std::move(*this));
    }

    constexpr auto
    ok() const && -> std::optional<T> requires std::is_move_constructible_v<T> {
        return ok_impl(std::move(*this));
    }

#endif

#if defined(__cpp_explicit_this_parameter) && ((__cpp_explicit_this_parameter) >= 202110L)
#else

    constexpr auto
    err() & -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(err_);
        }

        return std::nullopt;
    }

    constexpr auto
    err() const & -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(err_);
        }

        return std::nullopt;
    }

    constexpr auto
    err() && -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(std::move(err_));
        }

        return std::nullopt;
    }

    constexpr auto
    err() const && -> std::optional<E> {
        if (is_err()) {
            return std::make_optional<E>(std::move(err_));
        }

        return std::nullopt;
    }

#endif

private:
    template <typename Self, typename Fn>

    constexpr static auto
    map_impl(Self && self, Fn && f) -> result<details::fn_map_t<Fn, decltype(std::forward<Self>(self).value())>, E> {
        static_assert(std::is_same_v<result, std::remove_cvref_t<Self>>);
        static_assert((std::is_copy_constructible_v<E> || std::is_move_constructible_v<E>));
        static_assert(std::is_invocable_v<Fn, decltype(std::forward<Self>(self).value())>);

        using U = details::fn_map_t<Fn, decltype(std::forward<Self>(self).value())>;

        if (self.is_ok()) {
            return result<U, E>(std::invoke(std::forward<Fn>(f), std::forward<Self>(self).value()));
        }

        return result<U, E>(abc::err{std::forward<Self>(self).error()});
    }

public:
    template <typename Fn> requires std::is_invocable_v<Fn, T &> && std::is_copy_constructible_v<E>
    constexpr auto
    map(Fn && f) & -> result<details::fn_map_t<Fn, T &>, E> {
        return map_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_invocable_v<Fn, T> && std::is_copy_constructible_v<E>
    constexpr auto
    map(Fn && f) const & -> result<details::fn_map_t<Fn, T const &>, E> {
        return map_impl(*this, std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_invocable_v<Fn, T> && std::is_move_constructible_v<E>
    constexpr auto
    map(Fn && f) && -> result<details::fn_map_t<Fn, T &&>, E> {
        return map_impl(std::move(*this), std::forward<Fn>(f));
    }

    template <typename Fn> requires std::is_invocable_v<Fn, T> && std::is_move_constructible_v<E>
    constexpr auto
    map(Fn && f) const && -> result<details::fn_map_t<Fn, T const &&>, E> {
        return map_impl(std::move(*this), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename U, typename Fn> requires std::is_same_v<result, std::remove_cvref_t<Self>> && std::is_invocable_r_v<U, Fn, T>
    constexpr static auto
    map_or_impl(Self && self, U && default_value, Fn && f) -> U {
        if (self.is_ok()) {
            return std::invoke(std::forward<Fn>(f), std::forward<Self>(self).value());
        }

        return std::forward<U>(default_value);
    }

public:
    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T>
    constexpr auto
    map_or(U && default_value, Fn && f) & -> U {
        return map_or_impl(*this, std::forward<U>(default_value), std::forward<Fn>(f));
    }

    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T>
    constexpr auto
    map_or(U && default_value, Fn && f) const & -> U {
        return map_or_impl(*this, std::forward<U>(default_value), std::forward<Fn>(f));
    }

    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T>
    constexpr auto
    map_or(U && default_value, Fn && f) && -> U {
        return map_or_impl(std::move(*this), std::forward<U>(default_value), std::forward<Fn>(f));
    }

    template <typename U, typename Fn> requires std::is_invocable_r_v<U, Fn, T>
    constexpr auto
    map_or(U && default_value, Fn && f) const && -> U {
        return map_or_impl(std::move(*this), std::forward<U>(default_value), std::forward<Fn>(f));
    }

private:
    template <typename Self, typename U, typename ErrFn, typename ValFn> requires std::is_same_v<result, std::remove_cvref_t<Self>> && std::is_invocable_r_v<U, ErrFn, E> && std::is_invocable_r_v<U, ValFn, T>
    constexpr static auto
    map_or_else_impl(Self && self, ErrFn && err_fn, ValFn && val_fn) -> U {
        if (self.is_ok()) {
            return std::invoke(std::forward<ValFn>(val_fn), std::forward<Self>(self).value());
        }

        return std::invoke(std::forward<ErrFn>(err_fn), std::forward<Self>(self).error());
    }

public:
    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_r_v<U, ErrFn, E> && std::is_invocable_r_v<U, ValFn, T>
    constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) & -> U {
        return map_or_else_impl(*this, std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_r_v<U, ErrFn, E> && std::is_invocable_r_v<U, ValFn, T>
    constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) const & -> U {
        return map_or_else_impl(*this, std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_r_v<U, ErrFn, E> && std::is_invocable_r_v<U, ValFn, T>
    constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) && -> U {
        return map_or_else_impl(std::move(*this), std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

    template <typename U, typename ErrFn, typename ValFn> requires std::is_invocable_r_v<U, ErrFn, E> && std::is_invocable_r_v<U, ValFn, T>
    constexpr auto
    map_or_else(ErrFn && err_fn, ValFn && val_fn) const && -> U {
        return map_or_else_impl(std::move(*this), std::forward<ErrFn>(err_fn), std::forward<ValFn>(val_fn));
    }

private:
    template <typename Self, typename G, typename Fn> requires std::is_same_v<result, std::remove_cvref_t<Self>> && std::is_invocable_r_v<G, Fn, E> && (std::is_move_constructible_v<T> || std::is_copy_constructible_v<T>)
    constexpr static auto
    map_err_impl(Self && self, Fn && f) -> result<T, G> {
        if (self.is_ok()) {
            return result<T, G>{ std::forward<Self>(self).value() };
        }

        return result<T, G>{ abc::err<G>{ std::invoke(std::forward<Fn>(f), std::forward<Self>(self).error()) } };
    }

public:
    template <typename G, typename Fn> requires std::is_invocable_r_v<G, Fn, E> && std::is_copy_constructible_v<T>
    constexpr auto
    map_err(Fn && f) & -> result<T, G> {
        return map_err_impl(*this, std::forward<Fn>(f));
    }

    template <typename G, typename Fn> requires std::is_invocable_r_v<G, Fn, E> && std::is_copy_constructible_v<T>
    constexpr auto
    map_err(Fn && f) const & -> result<T, G> {
        return map_err_impl(*this, std::forward<Fn>(f));
    }

    template <typename G, typename Fn> requires std::is_invocable_r_v<G, Fn, E> && std::is_move_constructible_v<T>
    constexpr auto
    map_err(Fn && f) && -> result<T, G> {
        return map_err_impl(std::move(*this), std::forward<Fn>(f));
    }

    template <typename G, typename Fn> requires std::is_invocable_r_v<G, Fn, E> && std::is_move_constructible_v<T>
    constexpr auto
    map_err(Fn && f) const && -> result<T, G> {
        return map_err_impl(std::move(*this), std::forward<Fn>(f));
    }

    constexpr auto
    expect(std::string_view msg) & noexcept -> T & {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_result_access, msg);
        }

        return value();
    }

    constexpr auto
    expect(std::string_view msg) const & noexcept -> T const & {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_result_access, msg);
        }

        return value();
    }

    constexpr auto
    expect(std::string_view msg) && noexcept -> T && {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_result_access, msg);
        }

        return std::move(val_);;
    }

    constexpr auto
    expect(std::string_view msg) const && noexcept -> T const && {    // mark noexcept, force panic if error occurs.
        if (is_err()) {
            abc::throw_error(errc::bad_result_access, msg);
        }

        return std::move(val_);;
    }

    constexpr auto
    unwrap() & noexcept -> T & {    // mark noexcept, force panic if error occurs.
        return expect("result should have value when calling `unwrap()`");
    }

    constexpr auto
    unwrap() const & noexcept -> T const & {    // mark noexcept, force panic if error occurs.
        return expect("result should have value when calling `unwrap()`");
    }

    constexpr auto
    unwrap() && noexcept -> T && {    // mark noexcept, force panic if error occurs.
        return std::move(expect("result should have value when calling `unwrap()`"));
    }

    constexpr auto
    unwrap() const && noexcept -> T const && {    // mark noexcept, force panic if error occurs.
        return std::move(expect("result should have value when calling `unwrap()`"));
    }

    constexpr auto
    unwrap_or_default() & -> T requires std::is_default_constructible_v<T> && std::is_copy_constructible_v<T> {
        if (is_err()) {
            return T{};
        }

        return value();
    }

    constexpr auto
    unwrap_or_default() const & -> T requires std::is_default_constructible_v<T> && std::is_copy_constructible_v<T> {
        if (is_err()) {
            return T{};
        }

        return value();
    }

    constexpr auto
    unwrap_or_default() && -> T requires std::is_default_constructible_v<T> && std::is_move_constructible_v<T> {
        if (is_err()) {
            return T{};
        }

        return std::move(val_);
    }

    constexpr auto
    unwrap_or_default() const && -> T requires std::is_default_constructible_v<T> && std::is_move_constructible_v<T> {
        if (is_err()) {
            return T{};
        }

        return std::move(val_);
    }
};

template <typename T, typename E> requires std::is_void_v<T>
class result<T, E> {
    struct void_placeholder {
    };

    union {
        void_placeholder void_;
        E err_;
    };
    bool has_value_;

    static_assert(details::can_be_err<E>);

    template <typename U, typename G>
    constexpr static bool
        constructed_from_result_v = std::disjunction_v<
        std::is_constructible<err < E>, result<U, G>>,
        std::is_constructible<err < E>, result<U, G>
    &>,
    std::is_constructible<err < E>, result<U, G> const>,
    std::is_constructible<err < E>, result<U, G> const &>>;

public:
    using value_type = T;
    using error_type = E;
    using err_t = err <E>;

    // Constructor

    constexpr
    result() noexcept: void_{}, has_value_{ true } {
    }

    result(result const &) = default;

    constexpr
    result(result const & other) requires std::is_copy_constructible_v<E> && (!std::is_trivially_copy_constructible_v<E>)
        : void_{}, has_value_{ other.has_value_ } {
        if (!has_value_) {
            std::construct_at(std::addressof(err_), other.err_);
        }
    }

    result(result &&) = default;

    result(result && other) noexcept(std::is_nothrow_move_constructible_v<E>)requires std::is_move_constructible_v<E> && (!std::is_trivially_move_constructible_v<E>)
        : void_{}, has_value_{ other.has_value_ } {
        if (!has_value_) {
            std::construct_at(std::addressof(err_), std::move(other.err_));
        }
    }

    template <typename U, typename G>
    requires std::is_void_v<U> && std::is_constructible_v<E, G const &> && (!constructed_from_result_v<U, G>)
    constexpr
    explicit(!std::is_convertible_v<G const &, E>)
    result(result<U, G> const & other) noexcept(std::is_nothrow_constructible_v<E, G const &>)
        : void_{}, has_value_{ other.has_value_ } {
        if (!has_value_) {
            std::construct_at(std::addressof(err_), other.err_);
        }
    }

    template <typename U, typename G>
    requires std::is_void_v<U> && std::is_constructible_v<E, G> && (!constructed_from_result_v<U, G>)
    constexpr
    explicit(!std::is_convertible_v<G, E>)
    result(result<U, G> && other) noexcept(std::is_nothrow_constructible_v<E, G>)
        : void_{}, has_value_{ other.has_value_ } {
        if (!has_value_) {
            std::construct_at(std::addressof(err_), std::move(other.err_));
        }
    }

    template <typename G>
    requires std::is_constructible_v<E, G const &>
    constexpr
    explicit(!std::is_convertible_v<G const &, E>)
    result(err <G> const & e) : err_{ e.error() }, has_value_{ false } {
    }

    template <typename G>
    requires std::is_constructible_v<E, G>
    constexpr
    explicit(!std::is_convertible_v<G, E>)
    result(err <G> && e) : err_{ std::move(e).error() }, has_value_{ false } {
    }
};

}   // namespace abc

#endif //ABC_RESULT
