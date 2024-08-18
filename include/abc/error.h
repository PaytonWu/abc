// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_INCLUDE_ABC_ERROR)
#define ABC_INCLUDE_ABC_ERROR

#pragma once

#include "abc/details/config.h"

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
    span_built_from_rvalue,
    not_supported_byte_numbering,
    not_supported_byte_order,
    view_built_from_rvalue,
    bad_optional_access,
};

auto make_error_code(errc ec) noexcept -> std::error_code;

auto abc_category() noexcept -> std::error_category const &;

template <typename ExceptionT>
[[noreturn]] void
throw_exception(ExceptionT const & eh)
{
    throw eh;
}

template <typename ExceptionT, typename ... ArgsT>
[[noreturn]] void
throw_exception(ArgsT && ... args)
{
    throw ExceptionT{std::forward<ArgsT>(args)...};
}

void throw_error(std::error_code const & ec);
void throw_error(std::error_code const & ec, std::string_view extra_msg);

}

namespace std {

template <>
struct is_error_code_enum<abc::errc> : std::true_type {
};

}

#endif
