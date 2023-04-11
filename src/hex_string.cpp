// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/hex_string.h"

#include "abc/bytes.h"
#include "abc/error.h"
#include "abc/hex_utility.h"

#include <fmt/format.h>

#include <cassert>

namespace abc::details {

static auto from_hex_char(char const ch) noexcept -> xbyte_t {
    if ('0' <= ch && ch <= '9') {
        return static_cast<xbyte_t>(ch - '0');
    }

    if ('a' <= ch && ch <= 'f') {
        return static_cast<xbyte_t>(ch - 'a' + 10);
    }

    if ('A' <= ch && ch <= 'F') {
        return static_cast<xbyte_t>(ch - 'A' + 10);
    }

    assert(false);
    return static_cast<xbyte_t>(-1);
}

template <typename IteratorT>
auto to_hex_string(IteratorT begin, IteratorT end, xcase_insensitive_string_t<char> const & prefix) -> xcase_insensitive_string_t<char> {
    using iterator_traits = std::iterator_traits<IteratorT>;
    static_assert(sizeof(typename iterator_traits::value_type) == 1);

    constexpr char hex_digits[] = "0123456789abcdef";

    size_t off = prefix.size();
    xcase_insensitive_string_t<char> hex(std::distance(begin, end) * 2 + off, '0');
    hex.replace(0, off, prefix);
    for (auto it = begin; it != end; ++it) {
        hex[off++] = hex_digits[(*it >> 4) & 0x0f];
        hex[off++] = hex_digits[*it & 0x0f];
    }
    return hex;
}

template <typename T>
auto to_hex_string(T const & data) -> xcase_insensitive_string_t<char> {
    return to_hex_string(std::begin(data), std::end(data), "");
}

template <typename T>
auto to_hex_string_prefixed(T const & data) -> xcase_insensitive_string_t<char> {
    return to_hex_string(std::begin(data), std::end(data), hex_prefix);
}

auto to_hex_string(std::string_view const data, std::error_code & ec) -> xcase_insensitive_string_t<char> {
    assert(!ec);

    if (string_has_hex_prefix(data)) {
        if (auto const maybe_hex_string = data.substr(2); hex_string_without_prefix(maybe_hex_string)) {
            xcase_insensitive_string_t<char> r;
            r.reserve(maybe_hex_string.size() + 1);

            if (maybe_hex_string.size() & 1u) {
                r.append(1, '0');
            }
            return r.append(std::begin(maybe_hex_string), std::end(maybe_hex_string));
        }

        ec = xerrc_t::invalid_hex_string;
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

auto to_hex_string(std::string_view const data) {
    std::error_code ec;
    auto ret = to_hex_string(data, ec);
    throw_error(ec);
    return ret;
}

xabc_hex_string::xabc_hex_string(std::span<xbyte_t const> const input) : value_{ to_hex_string(input) } {
    assert((value_.size() & 1u) == 0);
}

xabc_hex_string::xabc_hex_string(std::string_view const input) : value_{ to_hex_string(input) } {
    assert((value_.size() & 1u) == 0);
}

auto xabc_hex_string::empty() const noexcept -> bool {
    return value_.empty();
}

auto xabc_hex_string::swap(xabc_hex_string & other) noexcept -> void {
    value_.swap(other.value_);
}

auto xabc_hex_string::to_string(xhex_string_format_t const format) const -> std::string {
    if (empty()) {
        return {};
    }

    if (format == xhex_string_format_t::with_prefix) {
        return fmt::format("{}{}", hex_prefix, value_);
    }

    return fmt::format("{}", value_);
}

auto xabc_hex_string::value() const -> std::string {
    return { value_.begin(), value_.end() };
}

auto xabc_hex_string::size(xhex_string_format_t const format) const noexcept -> size_t {
    return value_.size() + (static_cast<size_t>(format) << 1);
}

auto xabc_hex_string::length(xhex_string_format_t const format) const noexcept -> size_t {
    return size(format);
}

auto xabc_hex_string::from(std::span<xbyte_t const> const input) -> xabc_hex_string {
    return xabc_hex_string{ input };
}

auto xabc_hex_string::from(std::string_view const input) -> xabc_hex_string {
    return xabc_hex_string{ input };
}

}

namespace abc {

template <>
auto to<xbytes_t, xhex_string_t>(xhex_string_t const & from) -> xbytes_t {
    size_t index = 0;
    xbytes_t ret;
    auto const & value = from.value();
    ret.reserve((value.size() - index + 1) / 2);

    if (value.size() % 2) {
        auto const h = details::from_hex_char(value[index++]);
        ret.push_back(h);
    }

    for (auto i = index; i < value.size(); i += 2) {
        auto const h = details::from_hex_char(value[i]);
        auto const l = details::from_hex_char(value[1 + i]);

        ret.push_back(static_cast<xbyte_t>(h * 16 + l));
    }

    return ret;
}

}
