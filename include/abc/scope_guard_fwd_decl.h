// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_SCOPE_GUARD_FWD_DECL
#define ABC_INCLUDE_ABC_SCOPE_GUARD_FWD_DECL

#pragma once

namespace abc
{

template <typename EF>
class ScopeExit;

template <typename EF>
class ScopeFail;

template <typename EF>
class ScopeSuccess;

// Factory functions
template <typename EF>
auto make_scope_exit(EF && exit_function) -> ScopeExit<EF>;

template <typename EF>
auto make_scope_fail(EF && exit_function) -> ScopeFail<EF>;

template <typename EF>
auto make_scope_success(EF && exit_function) -> ScopeSuccess<EF>;

// Deduction guides for automatic template argument deduction
template <typename EF>
ScopeExit(EF) -> ScopeExit<EF>;

template <typename EF>
ScopeFail(EF) -> ScopeFail<EF>;

template <typename EF>
ScopeSuccess(EF) -> ScopeSuccess<EF>;

} // namespace abc

#endif
