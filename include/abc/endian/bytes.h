// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_ENDIAN_BYTES)
#define ABC_ENDIAN_BYTES

#pragma once

#include "abc/details/endian/bytes.h"

#include <array>
#include <span>
#include <utility>
#include <vector>

namespace abc::endian {

using xbytes_be_t = details::endian::xabc_bytes<std::endian::big>;
using xbytes_le_t = details::endian::xabc_bytes<std::endian::little>;

}

#endif //ABC_ENDIAN_BYTES
