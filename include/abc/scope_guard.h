// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_SCOPE_GUARD
#define ABC_INCLUDE_ABC_SCOPE_GUARD

#pragma once

#include "scope_guard_decl.h"

namespace abc
{

template <typename Callable>
auto
make_scope_guard(Callable && action) -> ScopeGuard<Callable>
{
    return ScopeGuard<Callable>(std::forward<Callable>(action));
}

template <typename Callable>
ScopeGuard<Callable>::ScopeGuard(Callable action) noexcept : action_{ std::move(action) }
                                                           , active_{ true }
{
}

template <typename Callable>
ScopeGuard<Callable>::ScopeGuard(ScopeGuard<Callable> && other) noexcept : action_{ std::move(other.action_) }
                                                                         , active_{ other.active_ }
{
    other.release();
}

template <typename Callable>
ScopeGuard<Callable>::~ScopeGuard() noexcept
{
    if (active_)
    {
        bool exiting_exception = std::uncaught_exceptions() > exception_count_;
        try
        {
            action_(exiting_exception);
        }
        catch (std::exception const &)
        {
            // Ignore exceptions thrown by the callable
        }
        catch (...)
        {
            // Ignore exceptions thrown by the callable
        }
    }
}

template <typename Callable>
auto
ScopeGuard<Callable>::release() noexcept -> void
{
    active_ = false;
}

} // namespace abc

#endif
