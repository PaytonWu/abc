// Copyright(c) 2023 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#if !defined(ABC_DETAILS_TIMER)
#define ABC_DETAILS_TIMER

#pragma once

#include <asio/steady_timer.hpp>

#include <expected>
#include <memory>
#include <mutex>

namespace abc::details {

class xabc_timer {
public:
    using timeout_callback_t = std::function<void(std::error_code const &)>;

private:
    mutable std::mutex timer_mutex_;
    asio::steady_timer timer_;

public:
    xabc_timer(xabc_timer const &) = delete;
    auto operator=(xabc_timer const &) -> xabc_timer & = delete;
    xabc_timer(xabc_timer && other) = delete;
    auto operator=(xabc_timer && other) -> xabc_timer & = delete;
    ~xabc_timer() noexcept;

    explicit xabc_timer(asio::io_context * io_object, std::chrono::milliseconds const & ms_in_future, timeout_callback_t callback);

    auto expired() const noexcept -> bool;
    auto wait() -> std::expected<void, std::error_code>;
};

}   // abc::details

#endif //ABC_TIMER_H
