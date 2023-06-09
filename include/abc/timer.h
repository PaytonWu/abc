// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_TIMER)
#define ABC_TIMER

#pragma once

#include "abc/details/config.h"

#include <asio/steady_timer.hpp>

#include <expected>
#include <memory>
#include <mutex>

namespace abc {

class timer {
public:
    using timeout_callback_t = std::function<void(std::error_code const &)>;

private:
    mutable std::mutex timer_mutex_;
    asio::steady_timer timer_;

public:
    timer(timer const &) = delete;
    auto operator=(timer const &) -> timer & = delete;
    timer(timer && other) = delete;
    auto operator=(timer && other) -> timer & = delete;
    ~timer() noexcept;

    explicit timer(asio::io_context * io_object, std::chrono::milliseconds const & ms_in_future, timeout_callback_t callback);

    auto expired() const noexcept -> bool;
    auto wait() -> std::expected<void, std::error_code>;
};

}

#endif //ABC_TIMER
