// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/hex_string.h"

#include "abc/error.h"
#include "abc/hex_utility.h"

#include <cassert>

namespace abc::details {

xabc_hex_string::xabc_hex_string(std::string_view const input) : value_{ to_hex_string(input) } {
    assert((value_.size() & 1u) == 0);
}

auto xabc_hex_string::from(std::string_view const input) -> xabc_hex_string {
    return xabc_hex_string{ input };
}

auto xabc_hex_string::to_string(format const fmt) const -> std::string {
    if (empty()) {
        return {};
    }

    std::string tmp;
    switch (fmt) {
    case lower_case_with_prefix:
        std::ranges::transform(value_, std::back_inserter(tmp), [](auto const ch) -> char { return static_cast<char>(std::tolower(ch)); });
        return fmt::format("{}{}", hex_prefix, tmp);
    case lower_case_without_prefix:
        std::ranges::transform(value_, std::back_inserter(tmp), [](auto const ch) -> char { return static_cast<char>(std::tolower(ch)); });
        return fmt::format("{}", tmp);
    case upper_case_with_prefix:
        std::ranges::transform(value_, std::back_inserter(tmp), [](auto const ch) -> char { return static_cast<char>(std::toupper(ch)); });
        return fmt::format("{}{}", hex_prefix_uppercase, tmp);
    case upper_case_without_prefix:
        std::ranges::transform(value_, std::back_inserter(tmp), [](auto const ch) -> char { return static_cast<char>(std::toupper(ch)); });
        return fmt::format("{}", tmp);
    case mixed_case_with_prefix:
        return fmt::format("{}{}", hex_prefix, value_);
    case mixed_case_without_prefix:
        return fmt::format("{}", value_);
    default:  // NOLINT(clang-diagnostic-covered-switch-default)
        assert(false);
        return fmt::format("{}", value_);
    }
}

auto to_hex_string(std::string_view const data, std::error_code & ec) -> xcase_insensitive_string_t<char> {
    assert(!ec);

    if (has_hex_prefix(data)) {
        if (auto const maybe_hex_string = data.substr(2); hex_string_without_prefix(maybe_hex_string)) {
            xcase_insensitive_string_t<char> r;
            r.reserve(maybe_hex_string.size() + 1);

            if (maybe_hex_string.size() & 1u) {
                r.append(1, '0');
            }
            return r.append(std::begin(maybe_hex_string), std::end(maybe_hex_string));
        }

        ec = make_error_code(std::errc::invalid_argument);
        return {};
    }

    if (hex_string_without_prefix(data)) {
        xcase_insensitive_string_t<char> r;
        r.reserve(data.size() + 1);

        if (data.size() & 1u) {
            r.append(1, '0');
        }
        return r.append(std::begin(data), std::end(data));
    }

    return to_hex_string(std::begin(data), std::end(data), "");
}

auto to_hex_string(std::string_view const input) -> xcase_insensitive_string_t<char> {
    std::error_code ec;
    auto ret = to_hex_string(input, ec);
    throw_error(ec);
    return ret;
}

}

namespace abc {



}
