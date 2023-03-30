// Copyright(c) 2022 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_SIMPLE_CONVERTER)
#define ABC_SIMPLE_CONVERTER

#pragma once

namespace abc {

template <typename ToT, typename FromT>
auto to(FromT const & from)->ToT;

}

#endif
