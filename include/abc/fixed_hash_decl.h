// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_FIXED_HASH_DECL
#define ABC_INCLUDE_ABC_FIXED_HASH_DECL

#pragma once

#include "fixed_bytes.h"

namespace abc {

using h256_t = abc::fixed_bytes<32, abc::byte_numbering::msb0>;

}

#endif //ABC_INCLUDE_ABC_FIXED_HASH_DECL
