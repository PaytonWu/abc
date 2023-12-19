// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_BYTES_VIEW_DECL
#define ABC_INCLUDE_ABC_BYTES_VIEW_DECL

#pragma once

#include "bytes_view_fwd_decl.h"

#include <range/v3/range/concepts.hpp>

namespace abc {

using bytes_be_view_t = bytes_view<byte_numbering::msb0>;
using bytes_le_view_t = bytes_view<byte_numbering::lsb0>;
using bytes_view_t = bytes_view<byte_numbering::none>;

}

namespace ranges {

template <abc::byte_numbering ByteNumbering>
inline constexpr bool enable_borrowed_range<abc::bytes_view<ByteNumbering>> = true;

template <abc::byte_numbering ByteNumbering>
inline constexpr bool enable_view<abc::bytes_view<ByteNumbering>> = true;

}

#endif //ABC_INCLUDE_ABC_BYTES_VIEW_DECL
