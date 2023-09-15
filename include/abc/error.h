// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_ERROR_ERROR)
#define ABC_ERROR_ERROR

#pragma once

#include "abc/details/config.h"

#include <cassert>
#include <cstdint>
#include <string_view>
#include <system_error>

namespace abc {

class abc_error final : public std::exception {
private:
    std::string msg_{};
    std::error_code ec_{};

public:
    abc_error(abc_error const &) = default;
    auto operator=(abc_error const &) -> abc_error & = default;
    abc_error(abc_error &&) = default;
    auto operator=(abc_error &&) -> abc_error & = default;
    ~abc_error() override = default;

    explicit abc_error(std::error_code const & ec);
    explicit abc_error(std::error_code const & ec, std::string_view msg);
    explicit abc_error(int ec, std::error_category const & category);
    explicit abc_error(int ec, std::error_category const & category, std::string_view msg);

    [[nodiscard]] auto code() const noexcept -> std::error_code const &;

    [[nodiscard]] auto what() const noexcept -> char const * override;
};

enum class errc {
    success,
    timer_driver_not_run,

    empty_buffer,
    xxhash_error,
    invalid_hex_char,
    invalid_hex_string,
    invalid_byte_numbering,
    invalid_bit_numbering,
    bad_expected_access,
    fix_bytes_invalid_argument,
};

auto make_error_code(errc ec) noexcept -> std::error_code;
auto make_error_condition(errc ec) noexcept -> std::error_condition;

auto abc_category() noexcept -> std::error_category const &;

void throw_error(std::error_code const & ec);
void throw_error(std::error_code const & ec, std::string_view extra_msg);

}

namespace abc::details {

template <typename ExceptionT>
[[noreturn]] void
throw_exception(ExceptionT eh) {
    throw std::move(eh);
}

[[noreturn]] inline void
do_throw_error(std::error_code const & ec) {
    assert(ec);
    abc_error eh{ ec };
    throw_exception(std::move(eh));
}

[[noreturn]] inline void
do_throw_error(std::error_code const & ec, std::string_view const extra_msg) {
    assert(ec);
    abc_error eh{ ec, extra_msg };
    throw_exception(std::move(eh));
}

}

namespace std {

template <>
struct is_error_code_enum<abc::errc> : std::true_type {
};

template <>
struct is_error_condition_enum<abc::errc> : std::true_type {
};

}

#include "abc/utility.h"

#include <fmt/core.h>

namespace abc {

inline
abc_error::abc_error(std::error_code const & ec)
    : std::exception{}, msg_{ ec.message() }, ec_{ ec } {
}

inline
abc_error::abc_error(std::error_code const & ec, std::string_view const msg)
    : std::exception{}, msg_{ fmt::format("{}:{}", msg, ec.message()) }, ec_{ ec } {
}

inline
abc_error::abc_error(int const ec, std::error_category const & category)
    : std::exception{}, msg_{ std::error_code{ec, category}.message() }, ec_{ ec, category } {
}

inline
abc_error::abc_error(int const ec, std::error_category const & category, std::string_view const msg)
    : std::exception{}, msg_{ fmt::format("{}:{}", msg, std::error_code{ec, category}.message()) }, ec_{ ec, category } {
}

inline
auto
abc_error::code() const noexcept -> std::error_code const & {
    return ec_;
}

inline
auto
abc_error::what() const noexcept -> char const * {
    return msg_.c_str();
}

inline
auto
make_error_code(errc const ec) noexcept -> std::error_code {
    return std::error_code{ static_cast<int>(ec), abc_category() };
}

inline
auto
make_error_condition(errc const ec) noexcept -> std::error_condition {
    return std::error_condition{ static_cast<int>(ec), abc_category() };
}

inline
auto
abc_category() noexcept -> std::error_category const & {
    static struct : std::error_category {
        [[nodiscard]] auto name() const noexcept -> char const * override {
            return "abc";
        }

        [[nodiscard]] auto message(int const ec) const -> std::string override {
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

                case errc::bad_expected_access:
                    return "bad expected access";

                case errc::fix_bytes_invalid_argument:
                    return "invalid argument for constructing fix_bytes";
            }

            abc::unreachable();
        }
    } category;
    return category;
}

inline
void
throw_error(std::error_code const & ec) {
    if (ec) { details::do_throw_error(ec); }
}

inline
void
throw_error(std::error_code const & ec, std::string_view extra_msg) {
    if (ec) { details::do_throw_error(ec, extra_msg); }
}

}

#endif
