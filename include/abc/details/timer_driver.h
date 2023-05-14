//
// Created by payto on 5/14/2023.
//

#ifndef ABC_DETAILS_TIMER_DRIVER
#define ABC_DETAILS_TIMER_DRIVER

#pragma once

#include "abc/timer.h"

#include <asio/io_context.hpp>

#include <atomic>
#include <mutex>
#include <vector>
#include <memory>

namespace abc::details {

class xabc_timer_driver : std::enable_shared_from_this<xabc_timer_driver> {
private:
    std::atomic<bool> running_{false};
    mutable std::mutex timers_mutex_;
    std::vector<std::unique_ptr<xtimer_t>> timers_{};

    std::chrono::milliseconds reap_interval_;
    asio::io_context * io_context_;

public:
    xabc_timer_driver(xabc_timer_driver const &) = delete;
    auto operator=(xabc_timer_driver const &) -> xabc_timer_driver & = delete;
    xabc_timer_driver(xabc_timer_driver &&) = delete;
    auto operator=(xabc_timer_driver &&) -> xabc_timer_driver & = delete;
    ~xabc_timer_driver() = default;

    explicit xabc_timer_driver(asio::io_context * io_context, std::chrono::milliseconds reap_interval_ms = std::chrono::milliseconds{100});

    auto running() const noexcept -> bool;
    void running(bool r);

    void start();
    void stop();

    void schedule(std::chrono::milliseconds const & ms_in_future, xtimer_t::timeout_callback_t callback);

private:
    void do_reap();
};

}


#endif //ABC_DETAILS_TIMER_DRIVER
