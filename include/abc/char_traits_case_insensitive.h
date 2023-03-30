// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_CASE_INSENSITIVE_STRING)
#define ABC_CASE_INSENSITIVE_STRING

#pragma once

#include <cctype>
#include <string>
#include <string_view>

namespace abc::details {

template <typename CharT>
struct xabc_case_insensitive : std::char_traits<CharT> {
private:
    [[nodiscard]] static auto to_lower(CharT const ch) -> int {
        return std::tolower(static_cast<uint8_t>(ch));
    }

public:
    [[nodiscard]] static auto eq(CharT const c1, CharT const c2) -> bool {
        return to_lower(c1) == to_lower(c2);
    }

    [[nodiscard]] static auto lt(CharT const c1, CharT const c2) -> bool {
        return to_lower(c1) < to_lower(c2);
    }

    [[nodiscard]] static auto compare(CharT const * s1, CharT const * s2, std::size_t n) -> int {
        while (n-- != 0) {
            if (to_lower(*s1) < to_lower(*s2)) {
                return -1;
            }

            if (to_lower(*s1) > to_lower(*s2)) {
                return 1;
            }

            ++s1; ++s2;
        }
        return 0;
    }

    [[nodiscard]] static auto find(CharT const * s, std::size_t n, CharT a) -> char const * {
        auto const lower(to_lower(a));
        while (n-- != 0) {
            if (to_lower(*s) == lower) {
                return s;
            }

            ++s;
        }

        return nullptr;
    }
};

}

namespace abc {

template <typename CharT>
using xcase_insentive_string_t = std::basic_string<CharT, details::xabc_case_insensitive<CharT>>;

template <typename CharT>
using xcase_insentive_string_view_t = std::basic_string_view<CharT, details::xabc_case_insensitive<CharT>>;

}

#endif
