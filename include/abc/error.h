// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_ERROR_ERROR)
#define ABC_ERROR_ERROR

#pragma once

#include "abc/details/error.h"

#include <system_error>

namespace abc {

class error final : public std::exception {
private:
    std::string msg_{};
    std::error_code ec_{};

public:
    error(error const &) = default;
    auto operator=(error const &) -> error & = default;
    error(error &&) = default;
    auto operator=(error &&) -> error & = default;
    ~error() override = default;

    explicit error(std::error_code const & ec);
    explicit error(std::error_code const & ec, std::string_view msg);
    explicit error(int ec, std::error_category const & category);
    explicit error(int ec, std::error_category const & category, std::string_view msg);

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
};

auto make_error_code(errc errc) noexcept -> std::error_code;
auto make_error_condition(errc errc) noexcept -> std::error_condition;

auto abc_category() noexcept -> std::error_category const &;

void throw_error(std::error_code const & ec);
void throw_error(std::error_code const & ec, std::string_view extra_msg);

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
