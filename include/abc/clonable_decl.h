// Copyright(c) 2024 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_INCLUDE_ABC_CLONABLE_DECL)
#define ABC_INCLUDE_ABC_CLONABLE_DECL

#pragma once

#include <type_traits>

namespace abc
{

template <typename T>
class clonable
{
protected:
    clonable() = default;

    clonable(clonable const &) = default;
    clonable(clonable &&) = default;

    auto
    operator=(clonable const &) -> clonable & = default;

    auto
    operator=(clonable &&) -> clonable & = default;

    ~clonable() = default;

public:
    [[nodiscard]] virtual auto
    clone() const -> T = 0;
};

} // namespace abc

#endif // ABC_INCLUDE_ABC_CLONABLE_DECL
