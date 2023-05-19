// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_TIMER_DRIVER)
#define ABC_TIMER_DRIVER

#pragma once

#include "abc/timer.h"

#include <asio/io_context.hpp>

#include <atomic>
#include <mutex>
#include <vector>
#include <memory>

namespace abc {

class timer_driver : std::enable_shared_from_this<timer_driver> {
private:
    std::atomic<bool> running_{false};
    mutable std::mutex timers_mutex_;
    std::vector<std::unique_ptr<timer>> timers_{};

    std::chrono::milliseconds reap_interval_;
    asio::io_context * io_context_;

public:
    timer_driver(timer_driver const &) = delete;
    auto operator=(timer_driver const &) -> timer_driver & = delete;
    timer_driver(timer_driver &&) = delete;
    auto operator=(timer_driver &&) -> timer_driver & = delete;
    ~timer_driver() = default;

    explicit timer_driver(asio::io_context * io_context, std::chrono::milliseconds reap_interval_ms = std::chrono::milliseconds{100});

    auto running() const noexcept -> bool;
    void running(bool r);

    void start();
    void stop();

    void schedule(std::chrono::milliseconds const & ms_in_future, timer::timeout_callback_t callback);

private:
    void do_reap();
};

}

#endif //ABC_TIMER_DRIVER
