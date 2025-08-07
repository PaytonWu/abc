// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_SCOPE_GUARD_FWD_DECL
#define ABC_INCLUDE_ABC_SCOPE_GUARD_FWD_DECL

#pragma once

namespace abc
{

template <typename Callable>
class ScopeGuard;

template <typename Callable>
auto make_scope_guard(Callable && action) -> ScopeGuard<Callable>;

// Deduction guide for automatic template argument deduction
template <typename Callable>
ScopeGuard(Callable) -> ScopeGuard<Callable>;

} // namespace abc

#endif
