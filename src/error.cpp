// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/error.h"

#include <fmt/core.h>

#include <cassert>
#include <utility>

namespace abc {

abc_error::abc_error(std::error_code const & ec)
    : std::exception{}, msg_{ ec.message() }, ec_{ ec } {
}

abc_error::abc_error(std::error_code const & ec, std::string_view const msg)
    : std::exception{}, msg_{ fmt::format("{} {}", msg, ec.message()) }, ec_{ ec } {
}

abc_error::abc_error(int const ec, std::error_category const & category)
    : std::exception{}, msg_{ std::error_code{ec, category}.message() }, ec_{ ec, category } {
}

abc_error::abc_error(int const ec, std::error_category const & category, std::string_view const msg)
    : std::exception{}, msg_{ fmt::format("{} {}", msg, std::error_code{ec, category}.message()) }, ec_{ ec, category } {
}

auto abc_error::code() const noexcept -> std::error_code const & {
    return ec_;
}

auto abc_error::what() const noexcept -> char const * {
    return msg_.c_str();
}

auto make_error_code(errc const ec) noexcept -> std::error_code {
    return std::error_code{ static_cast<int>(ec), abc_category() };
}

auto make_error_condition(errc const ec) noexcept -> std::error_condition {
    return std::error_condition{ static_cast<int>(ec), abc_category() };
}

static constexpr auto errc_map(int const ec) noexcept -> char const * {
    switch (static_cast<errc>(ec)) {
        case errc::success:
            return "success";

        case errc::timer_driver_not_run:
            return "timer driver not run";

        case errc::empty_buffer:
            return "empty buffer";

        case errc::xxhash_error:
            return "xxhash error";

        case errc::invalid_hex_char:
            return "invalid hex char";

        case errc::invalid_hex_string:
            return "invalid hex string";

        case errc::invalid_byte_numbering:
            return "invalid byte numbering";

        case errc::invalid_bit_numbering:
            return "invalid bit numbering";
    }

    std::unreachable();
}

auto abc_category() noexcept -> std::error_category const & {
    static struct : std::error_category {
        [[nodiscard]] auto name() const noexcept -> char const * override {
            return "abc";
        }

        [[nodiscard]] auto message(int const errc) const -> std::string override {
            return errc_map(errc);
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

namespace abc::details {

template <typename ExceptionT>
void throw_exception(ExceptionT eh) {
    throw std::move(eh);
}

void do_throw_error(std::error_code const & ec) {
    assert(ec);
    abc_error eh{ ec };
    throw_exception(std::move(eh));
}

void do_throw_error(std::error_code const & ec, std::string_view const extra_msg) {
    assert(ec);
    abc_error eh{ ec, extra_msg };
    throw_exception(std::move(eh));
}

}
