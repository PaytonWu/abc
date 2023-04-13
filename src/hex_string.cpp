// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/hex_string.h"

#include "abc/bytes.h"
#include "abc/error.h"
#include "abc/hex_utility.h"

#include <fmt/format.h>

#include <cassert>

namespace abc::details {

//static auto to_hex_string(std::string_view const data, std::error_code & ec) -> xcase_insensitive_string_t<char> {
//    assert(!ec);
//
//    if (string_has_hex_prefix(data)) {
//        if (auto const maybe_hex_string = data.substr(2); hex_string_without_prefix(maybe_hex_string)) {
//            xcase_insensitive_string_t<char> r;
//            r.reserve(maybe_hex_string.size() + 1);
//
//            if (maybe_hex_string.size() & 1u) {
//                r.append(1, '0');
//            }
//            return r.append(std::begin(maybe_hex_string), std::end(maybe_hex_string));
//        }
//
//        ec = xerrc_t::invalid_hex_string;
//        return {};
//    }
//
//    if (hex_string_without_prefix(data)) {
//        xcase_insensitive_string_t<char> r;
//        r.reserve(data.size() + 1);
//
//        if (data.size() & 1u) {
//            r.append(1, '0');
//        }
//        return r.append(std::begin(data), std::end(data));
//    }
//
//    return to_hex_string(std::begin(data), std::end(data), "");
//}

//static auto to_hex_string(std::string_view const data) -> xcase_insensitive_string_t<char> {
//    std::error_code ec;
//    auto ret = to_hex_string(data, ec);
//    throw_error(ec);
//    return ret;
//}

}

namespace abc {



}
