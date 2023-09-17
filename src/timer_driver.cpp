// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/timer_driver.h>

#include <cassert>

namespace abc {

timer_driver::timer_driver(asio::io_context * io_context, std::chrono::milliseconds reap_interval) : reap_interval_{ reap_interval }, io_context_{ io_context } {
}

auto
timer_driver::running() const noexcept -> bool {
    return running_.load(std::memory_order::acquire);
}

void
timer_driver::running(bool const r) {
    running_.store(r, std::memory_order::release);
}

void
timer_driver::start() {
    assert(!running());
    running(true);
    do_reap();
}

void
timer_driver::stop() {
    assert(running());
    running(false);
}

void
timer_driver::schedule(std::chrono::milliseconds const & ms_in_future, timer::timeout_callback_t callback) {
    if (!running()) {
        return;
    }

    std::lock_guard<std::mutex> lock{timers_mutex_};
    timers_.emplace_back(std::make_unique<timer>(io_context_, ms_in_future, std::move(callback)));
}

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