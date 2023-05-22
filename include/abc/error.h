// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_ERROR_ERROR)
#define ABC_ERROR_ERROR

#pragma once

#include "abc/details/error.h"

#include <cstdint>
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
    bad_result_access,
};

auto make_error_code(errc errc) noexcept -> std::error_code;
auto make_error_condition(errc errc) noexcept -> std::error_condition;

auto abc_category() noexcept -> std::error_category const &;

void throw_error(std::error_code const & ec);
void throw_error(std::error_code const & ec, std::string_view extra_msg);

class [[nodiscard]] error_domain {
protected:
    std::uintptr_t domain_id_;

public:
    auto domain_id() const noexcept -> std::uintptr_t {
        return domain_id_;
    }

private:
    friend auto operator==(error_domain const & lhs, error_domain const & rhs) noexcept -> bool {
        return lhs.domain_id_ == rhs.domain_id_;
    }

    friend auto operator<=>(error_domain const & lhs, error_domain const & rhs) noexcept -> std::strong_ordering {
        return lhs.domain_id_ <=> rhs.domain_id_;
    }
};

inline auto system_domain() noexcept -> error_domain {
    class system_domain : public error_domain {
    public:
        system_domain(system_domain const &) = default;
        auto operator=(system_domain const &) -> system_domain & = default;
        system_domain(system_domain &&) = default;
        auto operator=(system_domain &&) -> system_domain & = default;
        ~system_domain() = default;

        system_domain() noexcept {
            this->domain_id_ = reinterpret_cast<std::uintptr_t>(&std::system_category());
        }
    };

    static system_domain domain;

    return domain;
}

inline auto generic_domain() noexcept -> error_domain {
    class generic_domain : public error_domain {
    public:
        generic_domain(generic_domain const &) = default;
        auto operator=(generic_domain const &) -> generic_domain & = default;
        generic_domain(generic_domain &&) = default;
        auto operator=(generic_domain &&) -> generic_domain & = default;
        ~generic_domain() = default;

        generic_domain() noexcept {
            this->domain_id_ = reinterpret_cast<std::uintptr_t>(&std::generic_category());
        }
    };

    static generic_domain domain;
    return domain;
}

class [[nodiscard]] error {
private:
    static_assert(sizeof(std::uintptr_t) == sizeof(error_domain));

    error_domain domain_{ system_domain() };
    int value_{};

public:
    error() noexcept = default;
    error(error const &) noexcept = default;
    auto operator=(error const &) noexcept -> error & = default;
    error(error &&) noexcept = default;
    auto operator=(error &&) noexcept -> error & = default;
    ~error() noexcept = default;

    explicit error(int ec, error_domain const domain) noexcept : domain_{ domain }, value_{ ec } {
    }

    [[nodiscard]] explicit operator bool() const noexcept {
        return value_ != 0;
    }

    [[nodiscard]] auto domain() const noexcept -> error_domain {
        return domain_;
    }

    [[nodiscard]] auto value() const noexcept -> int {
        return value_;
    }
};

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
