// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/error.h>
#include <abc/utility.h>

#include <fmt/core.h>

namespace abc::details
{

[[noreturn]] static void
do_throw_error(std::error_code const & ec)
{
    assert(ec);
    abc_error eh{ ec };
    throw_exception(std::move(eh));
}

[[noreturn]] static void
do_throw_error(std::error_code const & ec, std::string_view const extra_msg)
{
    assert(ec);
    abc_error eh{ ec, extra_msg };
    throw_exception(std::move(eh));
}

}

namespace abc
{

abc_error::abc_error(std::error_code const & ec)
    : std::exception{}, msg_{ ec.message() }, ec_{ ec }
{
}

abc_error::abc_error(std::error_code const & ec, std::string_view const msg)
    : std::exception{}, msg_{ fmt::format("{}:{}", msg, ec.message()) }, ec_{ ec }
{
}

abc_error::abc_error(int const ec, std::error_category const & category)
    : std::exception{}, msg_{ std::error_code{ ec, category }.message() }, ec_{ ec, category }
{
}

abc_error::abc_error(int const ec, std::error_category const & category, std::string_view const msg)
    : std::exception{}, msg_{ fmt::format("{}:{}", msg, std::error_code{ ec, category }.message()) }, ec_{ ec, category }
{
}

auto
abc_error::code() const noexcept -> std::error_code const &
{
    return ec_;
}

auto
abc_error::what() const noexcept -> char const *
{
    return msg_.c_str();
}

auto
make_error_code(errc const ec) noexcept -> std::error_code
{
    return std::error_code{ static_cast<int>(ec), abc_category() };
}

auto
make_error_condition(errc const ec) noexcept -> std::error_condition
{
    return std::error_condition{ static_cast<int>(ec), abc_category() };
}

auto
abc_category() noexcept -> std::error_category const &
{
    static struct
        : std::error_category
    {
        [[nodiscard]] auto
        name() const noexcept -> char const * override
        {
            return "abc";
        }

        [[nodiscard]] auto
        message(int const ec) const -> std::string override
        {
            switch (static_cast<errc>(ec))
            {
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

                case errc::span_built_from_rvalue:
                    return "span built from rvalue";

                case errc::not_supported_byte_numbering:
                    return "not supported byte numbering";

                case errc::not_supported_byte_order:
                    return "not supported byte order";

                case errc::view_built_from_rvalue:
                    return "view built from rvalue";

                case errc::bad_optional_access:
                    return "bad optional access";

                default:
                    assert(false);
                    return "unknown error";
            }
        }
    } category;
    return category;
}

void
throw_error(std::error_code const & ec)
{
    if (ec)
    {
        details::do_throw_error(ec);
    }
}

void
throw_error(std::error_code const & ec, std::string_view extra_msg)
{
    if (ec)
    {
        details::do_throw_error(ec, extra_msg);
    }
}

}