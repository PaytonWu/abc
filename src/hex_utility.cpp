// Copyright(c) 2022 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/hex_utility.h>

#include <algorithm>

namespace abc {

auto hex_string_without_prefix(std::string_view const & string_slice) noexcept -> bool {
    return std::ranges::all_of(string_slice,
                               [](auto const ch) {
                                   return is_hex_char(ch);
                               });
}

auto hex_string_with_prefix(std::string_view const & string_slice) noexcept -> bool {
    if (!string_has_hex_prefix(string_slice)) {
        return false;
    }

    return hex_string_without_prefix(string_slice.substr(2));
}

auto hex_string(std::string_view const & string_slice) noexcept -> bool {
    return hex_string_with_prefix(string_slice) || hex_string_without_prefix(string_slice);
}

auto string_has_hex_prefix(std::string_view const & string_slice) noexcept -> bool {
    return string_slice.starts_with(hex_prefix) || string_slice.starts_with(hex_prefix_uppercase);
}

}
