// Copyright(c) 2020 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_ERROR_ERROR)
#define ABC_ERROR_ERROR

#pragma once

#include "config.h"

#include <stdexcept>
#include <system_error>

namespace abc::error::details {

class xabc_abc_error : public std::runtime_error {
private:
    std::error_code ec_{};

public:
    xabc_abc_error(xabc_abc_error const &) = default;
    xabc_abc_error & operator=(xabc_abc_error const &) = default;
    xabc_abc_error(xabc_abc_error &&) = default;
    xabc_abc_error & operator=(xabc_abc_error &&) = default;
    ~xabc_abc_error() override = default;

    explicit xabc_abc_error(std::error_code const & ec);
    explicit xabc_abc_error(std::error_code const & ec, std::string_view const & msg);
    explicit xabc_abc_error(int ec, std::error_category const & category);
    explicit xabc_abc_error(int ec, std::error_category const & category, std::string_view const & msg);

    [[nodiscard]] std::error_code const & code() const noexcept;
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

}

namespace abc::error {

using xabc_error_t = details::xabc_abc_error;
using xerrc_t = details::xabc_enum_errc;

auto abc_category() noexcept -> std::error_category const &;

template <typename ExceptionT>
void throw_exception(ExceptionT const & eh) {
    throw eh;
}

void do_throw_error(std::error_code const & ec);
void do_throw_error(std::error_code const & ec, std::string_view const & extra_msg);

inline void throw_error(std::error_code const & ec) {
    if (ec) {
        do_throw_error(ec);
    }
}

inline void throw_error(std::error_code const & ec, std::string_view const & extra_msg) {
    if (ec) {
        do_throw_error(ec, extra_msg);
    }
}

}

namespace std {

template <>
struct is_error_code_enum<abc::error::xerrc_t> : std::true_type {
};

template <>
struct is_error_condition_enum<abc::error::xerrc_t> : std::true_type {
};

}

#endif
