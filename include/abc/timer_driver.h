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

#include <cassert>

namespace abc {

inline
timer_driver::timer_driver(asio::io_context * io_context, std::chrono::milliseconds reap_interval) : reap_interval_{ reap_interval }, io_context_{ io_context } {
}

inline
auto
timer_driver::running() const noexcept -> bool {
    return running_.load(std::memory_order::acquire);
}

inline
void
timer_driver::running(bool const r) {
    running_.store(r, std::memory_order::release);
}

inline
void
timer_driver::start() {
    assert(!running());
    running(true);
    do_reap();
}

inline
void
timer_driver::stop() {
    assert(running());
    running(false);
}

inline
void
timer_driver::schedule(std::chrono::milliseconds const & ms_in_future, timer::timeout_callback_t callback) {
    if (!running()) {
        return;
    }

    std::lock_guard<std::mutex> lock{timers_mutex_};
    timers_.emplace_back(std::make_unique<timer>(io_context_, ms_in_future, std::move(callback)));
}

inline
void
timer_driver::do_reap() {
    if (!running()) {
        return;
    }

    {
        std::lock_guard<std::mutex> lock{timers_mutex_};

        auto it = std::begin(timers_);
        while (it != std::end(timers_)) {
            if ((*it)->expired()) {
                (*it)->wait();
                it = timers_.erase(it);
            } else {
                ++it;
            }
        }
    }

    if (!running()) {
        return;
    }

    timers_.emplace_back(std::make_unique<timer>(io_context_, reap_interval_, [this, self = shared_from_this()](std::error_code const & ec) {
        if (ec && ec == asio::error::operation_aborted) {
            return;
        }

        if (!running()) {
            return;
        }

        do_reap();
    }));
}

}

#endif //ABC_TIMER_DRIVER
