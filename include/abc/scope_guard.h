// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_SCOPE_GUARD
#define ABC_INCLUDE_ABC_SCOPE_GUARD

#pragma once

#include "scope_guard_decl.h"

namespace abc
{

// =============================================================================
// ScopeExit Implementation
// =============================================================================

template <typename EF>
template <typename EFP>
    requires not_same_as<std::remove_cvref_t<EFP>, ScopeExit<EF>> && std::constructible_from<EF, EFP>
ScopeExit<EF>::ScopeExit(EFP && exit_function) noexcept(std::is_nothrow_constructible_v<EF, EFP &>)
try
    : exit_function_(exit_function)
{
}
catch (...)
{
    exit_function();
}

template <typename EF>
template <typename EFP>
    requires not_same_as<std::remove_cvref_t<EFP>, ScopeExit<EF>> && std::constructible_from<EF, EFP> && not_lvalue_ref<EFP> && std::is_nothrow_constructible_v<EF, EFP>
ScopeExit<EF>::ScopeExit(EFP && exit_function) noexcept : exit_function_(std::forward<EFP>(exit_function))
{
}

template <typename EF>
ScopeExit<EF>::ScopeExit(ScopeExit && other) noexcept
    requires std::is_nothrow_move_constructible_v<EF>
    : exit_function_(std::forward<EF>(other.exit_function_))
{
    other.release();
}

template <typename EF>
ScopeExit<EF>::ScopeExit(ScopeExit && other) noexcept(std::is_nothrow_copy_constructible_v<EF>)
    requires(!std::is_nothrow_move_constructible_v<EF>) && std::is_copy_constructible_v<EF>
    : exit_function_(other.exit_function_)
{
    other.release();
}

template <typename EF>
ScopeExit<EF>::~ScopeExit() noexcept
{
    if (execute_on_destruction_)
    {
        exit_function_();
    }
}

template <typename EF>
void
ScopeExit<EF>::release() noexcept
{
    execute_on_destruction_ = false;
}

// =============================================================================
// ScopeFail Implementation
// =============================================================================

template <typename EF>
template <typename EFP>
    requires not_same_as<std::remove_cvref_t<EFP>, ScopeFail<EF>> && std::constructible_from<EF, EFP>
ScopeFail<EF>::ScopeFail(EFP && exit_function) noexcept(std::is_nothrow_constructible_v<EF, EFP &>)
try
    : exit_function_(exit_function)
{
}
catch (...)
{
    exit_function();
}

template <typename EF>
template <typename EFP>
    requires not_same_as<std::remove_cvref_t<EFP>, ScopeFail<EF>> && std::constructible_from<EF, EFP> && not_lvalue_ref<EFP> && std::is_nothrow_constructible_v<EF, EFP>
ScopeFail<EF>::ScopeFail(EFP && exit_function) noexcept : exit_function_(std::forward<EFP>(exit_function))
{
}

template <typename EF>
ScopeFail<EF>::ScopeFail(ScopeFail && other) noexcept
    requires std::is_nothrow_move_constructible_v<EF>
    : exit_function_(std::forward<EF>(other.exit_function_))
{
    other.release();
}

template <typename EF>
ScopeFail<EF>::ScopeFail(ScopeFail && other) noexcept(std::is_nothrow_copy_constructible_v<EF>)
    requires(!std::is_nothrow_move_constructible_v<EF>) && std::is_copy_constructible_v<EF>
    : exit_function_(other.exit_function_)
{
    other.release();
}

template <typename EF>
ScopeFail<EF>::~ScopeFail() noexcept
{
    if (std::uncaught_exceptions() > uncaught_init_)
    {
        exit_function_();
    }
}

template <typename EF>
void
ScopeFail<EF>::release() noexcept
{
    uncaught_init_ = INT_MAX;
}

// =============================================================================
// ScopeSuccess Implementation
// =============================================================================

template <typename EF>
template <typename EFP>
    requires not_same_as<std::remove_cvref_t<EFP>, ScopeSuccess<EF>> && std::constructible_from<EF, EFP>
ScopeSuccess<EF>::ScopeSuccess(EFP && exit_function) noexcept(std::is_nothrow_constructible_v<EF, EFP &>) : exit_function_(exit_function)
{
}

template <typename EF>
template <typename EFP>
    requires not_same_as<std::remove_cvref_t<EFP>, ScopeSuccess<EF>> && std::constructible_from<EF, EFP> && not_lvalue_ref<EFP> && std::is_nothrow_constructible_v<EF, EFP>
ScopeSuccess<EF>::ScopeSuccess(EFP && exit_function) noexcept : exit_function_(std::forward<EFP>(exit_function))
{
}

template <typename EF>
ScopeSuccess<EF>::ScopeSuccess(ScopeSuccess && other) noexcept
    requires std::is_nothrow_move_constructible_v<EF>
    : exit_function_(std::forward<EF>(other.exit_function_))
{
    other.release();
}

template <typename EF>
ScopeSuccess<EF>::ScopeSuccess(ScopeSuccess && other) noexcept(std::is_nothrow_copy_constructible_v<EF>)
    requires(!std::is_nothrow_move_constructible_v<EF>) && std::is_copy_constructible_v<EF>
    : exit_function_(other.exit_function_)
{
    other.release();
}

template <typename EF>
ScopeSuccess<EF>::~ScopeSuccess() noexcept(noexcept(this->exit_function_()))
{
    if (std::uncaught_exceptions() <= uncaught_init_)
    {
        exit_function_();
    }
}

template <typename EF>
void
ScopeSuccess<EF>::release() noexcept
{
    uncaught_init_ = -INT_MAX;
}

// =============================================================================
// Factory functions for creating scope guards
// =============================================================================

template <typename EF>
auto
make_scope_exit(EF && exit_function) -> ScopeExit<EF>
{
    return ScopeExit<EF>(std::forward<EF>(exit_function));
}

template <typename EF>
auto
make_scope_fail(EF && exit_function) -> ScopeFail<EF>
{
    return ScopeFail<EF>(std::forward<EF>(exit_function));
}

template <typename EF>
auto
make_scope_success(EF && exit_function) -> ScopeSuccess<EF>
{
    return ScopeSuccess<EF>(std::forward<EF>(exit_function));
}

} // namespace abc

#endif
