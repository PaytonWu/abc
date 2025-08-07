// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_SCOPE_GUARD_DECL
#define ABC_INCLUDE_ABC_SCOPE_GUARD_DECL

#pragma once

#include "scope_guard_fwd_decl.h"

#include <exception>

namespace abc
{

template <typename Callable>
class ScopeGuard
{
private:
    Callable action_;                                   // Callable to execute
    bool active_;                                       // Activation flag
    int exception_count_{ std::uncaught_exceptions() }; // Track current exceptions

public:
    // Constructor: captures the callable action
    explicit ScopeGuard(Callable action) noexcept;

    // Move constructor
    ScopeGuard(ScopeGuard<Callable> && other) noexcept;

    // Destructor: executes action if still active
    ~ScopeGuard() noexcept;

    // Prevent copying
    ScopeGuard(ScopeGuard const &) = delete;
    auto operator=(ScopeGuard const &) -> ScopeGuard & = delete;

    // Deactivate the guard
    auto release() noexcept -> void;
};

} // namespace abc

#endif
