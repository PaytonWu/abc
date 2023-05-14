// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_BIGINT)
#define ABC_BIGINT

#pragma once

#include "abc/bytes.h"

#include <boost/multiprecision/cpp_int.hpp>

namespace abc {

using xuint128_t = boost::multiprecision::uint128_t;
using xuint256_t = boost::multiprecision::uint256_t;

using xint128_t = boost::multiprecision::int128_t;
using xint256_t = boost::multiprecision::int256_t;

using xbigint_t = boost::multiprecision::number<boost::multiprecision::cpp_int_backend<>>;

}

#endif
