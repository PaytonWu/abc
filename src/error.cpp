// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/error.h"

#include <fmt/core.h>

#include <cassert>

namespace abc::details {

xabc_error::xabc_error(std::error_code const & ec)
    : std::runtime_error{ ec.message() }, ec_{ ec } {
}

xabc_error::xabc_error(std::error_code const & ec, std::string_view const msg)
    : std::runtime_error{ fmt::format("{} {}", msg, ec.message()) }, ec_{ ec } {
}

xabc_error::xabc_error(int const ec, std::error_category const & category)
    : runtime_error{ std::error_code{ec, category}.message() }, ec_{ ec, category } {
}

xabc_error::xabc_error(int const ec, std::error_category const & category, std::string_view const msg)
    : runtime_error{ fmt::format("{} {}", msg, std::error_code{ec, category}.message()) }, ec_{ ec, category } {
}

auto xabc_error::code() const noexcept -> std::error_code const & {
    return ec_;
}

auto make_error_code(xabc_enum_errc errc) noexcept -> std::error_code {
    return std::error_code{ static_cast<int>(errc), abc_category() };
}

auto make_error_condition(xabc_enum_errc errc) noexcept -> std::error_condition {
    return std::error_condition{ static_cast<int>(errc), abc_category() };
}

static constexpr auto errc_map(int const errc) noexcept -> char const * {
    switch (static_cast<xabc_enum_errc>(errc)) {
    case xabc_enum_errc::success:
        return "success";

    case xabc_enum_errc::timer_driver_not_run:
        return "timer driver not run";

    case xabc_enum_errc::empty_buffer:
        return "empty buffer";

    case xabc_enum_errc::invalid_hex_char:
        return "invalid hex char";

    case xabc_enum_errc::invalid_hex_string:
        return "invalid hex string";

    case xabc_enum_errc::xxhash_error:
        return "xxhash error";

    default:  // NOLINT(clang-diagnostic-covered-switch-default)
        return "unknown error";
    }
}

template <typename ExceptionT>
void throw_exception(ExceptionT const & eh) {
    throw eh;
}

void do_throw_error(std::error_code const & ec) {
    assert(ec);
    xabc_error const eh{ ec };
    throw_exception(eh);
}

void do_throw_error(std::error_code const & ec, std::string_view const extra_msg) {
    assert(ec);
    xabc_error const eh{ ec, extra_msg };
    throw_exception(eh);
}

}

namespace abc {

auto abc_category() noexcept -> std::error_category const & {
    static struct : std::error_category {
        [[nodiscard]] auto name() const noexcept -> char const * override {
            return "abc";
        }

        [[nodiscard]] auto message(int const errc) const -> std::string override {
            return details::errc_map(errc);
        }
    } category;
    return category;
}

void throw_error(std::error_code const & ec) {
    if (ec) { details::do_throw_error(ec); }
}
void throw_error(std::error_code const & ec, std::string_view extra_msg) {
    if (ec) { details::do_throw_error(ec, extra_msg); }
}

}
