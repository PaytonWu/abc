// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_SCOPE_GUARD_DECL
#define ABC_INCLUDE_ABC_SCOPE_GUARD_DECL

#pragma once

#include "scope_guard_fwd_decl.h"

#include <climits>
#include <concepts>
#include <exception>
#include <type_traits>

namespace abc
{

template <typename T, typename U>
concept not_same_as = !std::same_as<T, U>;

template <typename T>
concept not_lvalue_ref = !std::is_lvalue_reference_v<T>;

template <typename EF>
class [[nodiscard]] ScopeExit
{
public:
    template <typename EFP>
        requires not_same_as<std::remove_cvref_t<EFP>, ScopeExit> && std::constructible_from<EF, EFP>
    [[nodiscard]] explicit ScopeExit(EFP && exit_function) noexcept(std::is_nothrow_constructible_v<EF, EFP &>);

    template <typename EFP>
        requires not_same_as<std::remove_cvref_t<EFP>, ScopeExit> && std::constructible_from<EF, EFP> && not_lvalue_ref<EFP> && std::is_nothrow_constructible_v<EF, EFP>
    explicit ScopeExit(EFP && exit_function) noexcept;

    ScopeExit(ScopeExit && other) noexcept
        requires std::is_nothrow_move_constructible_v<EF>;

    ScopeExit(ScopeExit && other) noexcept(std::is_nothrow_copy_constructible_v<EF>)
        requires(!std::is_nothrow_move_constructible_v<EF>) && std::is_copy_constructible_v<EF>;

    ScopeExit(ScopeExit const &) = delete;
    ScopeExit & operator=(ScopeExit const &) = delete;
    ScopeExit & operator=(ScopeExit &&) = delete;

    ~ScopeExit() noexcept;

    void release() noexcept;

private:
    [[no_unique_address]] EF exit_function_;
    bool execute_on_destruction_ = true;
};

template <typename EF>
class [[nodiscard]] ScopeFail
{
public:
    template <typename EFP>
        requires not_same_as<std::remove_cvref_t<EFP>, ScopeFail> && std::constructible_from<EF, EFP>
    explicit ScopeFail(EFP && exit_function) noexcept(std::is_nothrow_constructible_v<EF, EFP &>);

    template <typename EFP>
        requires not_same_as<std::remove_cvref_t<EFP>, ScopeFail> && std::constructible_from<EF, EFP> && not_lvalue_ref<EFP> && std::is_nothrow_constructible_v<EF, EFP>
    explicit ScopeFail(EFP && exit_function) noexcept;

    ScopeFail(ScopeFail && other) noexcept
        requires std::is_nothrow_move_constructible_v<EF>;

    ScopeFail(ScopeFail && other) noexcept(std::is_nothrow_copy_constructible_v<EF>)
        requires(!std::is_nothrow_move_constructible_v<EF>) && std::is_copy_constructible_v<EF>;

    ScopeFail(ScopeFail const &) = delete;
    ScopeFail & operator=(ScopeFail const &) = delete;
    ScopeFail & operator=(ScopeFail &&) = delete;

    ~ScopeFail() noexcept;

    void release() noexcept;

private:
    [[no_unique_address]] EF exit_function_;
    int uncaught_init_ = std::uncaught_exceptions();
};

template <typename EF>
class [[nodiscard]] ScopeSuccess
{
public:
    template <typename EFP>
        requires not_same_as<std::remove_cvref_t<EFP>, ScopeSuccess> && std::constructible_from<EF, EFP>
    explicit ScopeSuccess(EFP && exit_function) noexcept(std::is_nothrow_constructible_v<EF, EFP &>);

    template <typename EFP>
        requires not_same_as<std::remove_cvref_t<EFP>, ScopeSuccess> && std::constructible_from<EF, EFP> && not_lvalue_ref<EFP> && std::is_nothrow_constructible_v<EF, EFP>
    explicit ScopeSuccess(EFP && exit_function) noexcept;

    ScopeSuccess(ScopeSuccess && other) noexcept
        requires std::is_nothrow_move_constructible_v<EF>;

    ScopeSuccess(ScopeSuccess && other) noexcept(std::is_nothrow_copy_constructible_v<EF>)
        requires(!std::is_nothrow_move_constructible_v<EF>) && std::is_copy_constructible_v<EF>;

    ScopeSuccess(ScopeSuccess const &) = delete;
    ScopeSuccess & operator=(ScopeSuccess const &) = delete;
    ScopeSuccess & operator=(ScopeSuccess &&) = delete;

    ~ScopeSuccess() noexcept(noexcept(this->exit_function_()));

    void release() noexcept;

private:
    [[no_unique_address]] EF exit_function_;
    int uncaught_init_ = std::uncaught_exceptions();
};

} // namespace abc

#endif
