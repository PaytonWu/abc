// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_ASYNC_QUEUE_DECL
#define ABC_INCLUDE_ABC_ASYNC_QUEUE_DECL

#pragma once

#include "queue_fwd_decl.h"

#include <array>
#include <atomic>
#include <optional>

namespace abc::async
{

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
class Queue
{
private:
    // Each cell has a sequence number and data
    struct Cell
    {
        std::atomic<std::size_t> sequence;
        T data;
    };

    // Buffer of cells
    std::array<Cell, Capacity> buffer_;

    // Padded atomic indices to prevent false sharing
    static constexpr std::size_t cache_line_size_in_bytes = 64; // Assuming 64-byte cache line size
    alignas(cache_line_size_in_bytes) std::atomic<std::size_t> head_{ 0 };
    alignas(cache_line_size_in_bytes) std::atomic<std::size_t> tail_{ 0 };

    Scheduler scheduler_;

public:
    explicit Queue(Scheduler scheduler);

    // Non-copyable, non-movable
    Queue(Queue const &) = delete;
    auto operator=(Queue const &) -> Queue & = delete;
    Queue(Queue &&) = delete;
    auto operator=(Queue &&) -> Queue & = delete;

    // Async enqueue operation
    auto enqueue(T item) -> stdexec::sender auto;

    // Async dequeue operation
    auto dequeue() -> stdexec::sender auto;

    // Synchronous operations for convenience
    auto try_enqueue(T item) -> bool;
    auto try_dequeue() -> std::optional<T>;

    // Query operations
    auto empty() const noexcept -> bool;
    auto full() const noexcept -> bool;
    auto size() const noexcept -> std::size_t;

    constexpr auto capacity() const noexcept -> std::size_t;
};

} // namespace abc::async

#endif // ABC_INCLUDE_ABC_ASYNC_QUEUE_DECL
