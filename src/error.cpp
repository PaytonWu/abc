// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/error.h>

#include <fmt/core.h>

#include <cassert>

namespace abc::error::details {

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

std::error_code const & xabc_error::code() const noexcept {
    return ec_;
}

std::error_code make_error_code(xabc_enum_errc errc) noexcept {
    return std::error_code{ static_cast<int>(errc), abc_category() };
}

std::error_condition make_error_condition(xabc_enum_errc errc) noexcept {
    return std::error_condition{ static_cast<int>(errc), abc_category() };
}

static char const * errc_map(int const errc) noexcept {
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

class xabc_abc_category final : public std::error_category {
    [[nodiscard]] char const * name() const noexcept override {
        return "abc";
    }

    [[nodiscard]] std::string message(int const errc) const override {
        return std::string{ errc_map(errc) };
    }
};

template <typename ExceptionT>
void throw_exception(ExceptionT const & eh) {
    throw eh;
}

}

namespace abc::error {

std::error_category const & abc_category() noexcept {
    static details::xabc_abc_category abc_category;  // NOLINT(clang-diagnostic-exit-time-destructors)
    return abc_category;
}

void do_throw_error(std::error_code const & ec) {
    assert(ec);
    xerror_t const eh{ ec };
    details::throw_exception(eh);
}

void do_throw_error(std::error_code const & ec, std::string_view const extra_msg) {
    assert(ec);
    xerror_t const eh{ ec, extra_msg };
    throw_exception(eh);
}

}
