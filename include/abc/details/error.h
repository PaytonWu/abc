// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_ERROR)
#define ABC_DETAILS_ERROR

#pragma once

#include "abc/details/config.h"

#include <stdexcept>
#include <system_error>

namespace abc::details {

class xabc_error final : public std::runtime_error {
private:
    std::error_code ec_{};

public:
    xabc_error(xabc_error const &) = default;
    auto operator=(xabc_error const &) -> xabc_error & = default;
    xabc_error(xabc_error &&) = default;
    auto operator=(xabc_error &&) -> xabc_error & = default;
    ~xabc_error() override = default;

    explicit xabc_error(std::error_code const & ec);
    explicit xabc_error(std::error_code const & ec, std::string_view msg);
    explicit xabc_error(int ec, std::error_category const & category);
    explicit xabc_error(int ec, std::error_category const & category, std::string_view msg);

    [[nodiscard]] auto code() const noexcept -> std::error_code const &;
};

enum class xabc_enum_errc {
    success,
    timer_driver_not_run,

    empty_buffer,
    xxhash_error,
    invalid_hex_char,
    invalid_hex_string,
};

auto make_error_code(xabc_enum_errc errc) noexcept -> std::error_code;
auto make_error_condition(xabc_enum_errc errc) noexcept -> std::error_condition;

void do_throw_error(std::error_code const & ec);
void do_throw_error(std::error_code const & ec, std::string_view extra_msg);

} // namespace abc::error::details

#endif
