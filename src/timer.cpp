// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include "abc/details/timer.h"

namespace abc::details {

xabc_timer::xabc_timer(asio::io_context * io_context, std::chrono::milliseconds const & ms_in_future, timeout_callback_t callback) : timer_{ io_context->get_executor(), ms_in_future } {
    timer_.async_wait(std::move(callback));
}

auto xabc_timer::expired() const noexcept -> bool {
    std::lock_guard<std::mutex> lock{ timer_mutex_ };
    return timer_.expiry() < asio::steady_timer::clock_type::now();
}

auto xabc_timer::wait() -> std::expected<void, std::error_code> {
    std::lock_guard<std::mutex> lock{ timer_mutex_ };
    std::error_code ec;
    timer_.wait(ec);
    if (ec) {
        return std::unexpected{ ec };
    }

    return std::expected<void, std::error_code>{};
}

}