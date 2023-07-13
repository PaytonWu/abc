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

#endif
