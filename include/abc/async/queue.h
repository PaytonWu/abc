// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_ASYNC_QUEUE
#define ABC_INCLUDE_ABC_ASYNC_QUEUE

#pragma once

#include "queue_decl.h"

#include <exec/task.hpp>

namespace abc::async
{

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
Queue<T, Capacity, Scheduler>::Queue(Scheduler scheduler) : buffer_{}
                                                          , head_{ 0 }
                                                          , tail_{ 0 }
                                                          , size_{ 0 }
                                                          , scheduler_{ scheduler }
{
    static_assert(Capacity > 0, "Queue capacity must be greater than 0");
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
std::size_t
Queue<T, Capacity, Scheduler>::next_index(std::size_t index) const noexcept
{
    return (index + 1) % Capacity;
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
bool
Queue<T, Capacity, Scheduler>::empty() const noexcept
{
    return this->size() == 0;
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
bool
Queue<T, Capacity, Scheduler>::full() const noexcept
{
    return this->size() == Capacity;
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
std::size_t
Queue<T, Capacity, Scheduler>::size() const noexcept
{
    return size_.load(std::memory_order_acquire);
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
bool
Queue<T, Capacity, Scheduler>::try_enqueue(T item)
{
    std::unique_lock<std::mutex> lock(mutex_);

    if (size_.load(std::memory_order_relaxed) == Capacity)
    {
        return false;
    }

    std::size_t tail = tail_.load(std::memory_order_relaxed);
    buffer_[tail] = std::move(item);
    tail_.store(next_index(tail), std::memory_order_relaxed);
    size_.fetch_add(1, std::memory_order_release);

    dequeue_cv_.notify_one();
    return true;
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
std::optional<T>
Queue<T, Capacity, Scheduler>::try_dequeue()
{
    std::unique_lock<std::mutex> lock(mutex_);

    if (size_.load(std::memory_order_relaxed) == 0)
    {
        return std::nullopt;
    }

    std::size_t head = head_.load(std::memory_order_relaxed);
    T item = std::move(buffer_[head]);
    head_.store(next_index(head), std::memory_order_relaxed);
    size_.fetch_sub(1, std::memory_order_release);

    enqueue_cv_.notify_one();
    return item;
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
auto
Queue<T, Capacity, Scheduler>::enqueue(T item) -> stdexec::sender auto
{
    return stdexec::let_value(stdexec::just(std::move(item)), [this](T item) -> exec::task<void> {
        while (true)
        {
            // Check if space is available
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (size_.load(std::memory_order_relaxed) < Capacity)
                {
                    // Space available, enqueue the item
                    std::size_t tail = tail_.load(std::memory_order_relaxed);
                    buffer_[tail] = std::move(item);
                    tail_.store(next_index(tail), std::memory_order_relaxed);
                    size_.fetch_add(1, std::memory_order_release);
                    dequeue_cv_.notify_one();
                    co_return;
                }
            }

            // No space available, yield and retry
            co_await stdexec::schedule(scheduler_);
        }
    });
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
auto
Queue<T, Capacity, Scheduler>::dequeue() -> stdexec::sender auto
{
    return [this]() -> exec::task<T> {
        while (true)
        {
            // Check if item is available
            {
                std::unique_lock<std::mutex> lock(mutex_);
                if (size_.load(std::memory_order_relaxed) > 0)
                {
                    // Item available, dequeue it
                    std::size_t head = head_.load(std::memory_order_relaxed);
                    T item = std::move(buffer_[head]);
                    head_.store(next_index(head), std::memory_order_relaxed);
                    size_.fetch_sub(1, std::memory_order_release);
                    enqueue_cv_.notify_one();
                    co_return std::move(item);
                }
            }

            // No item available, yield and retry
            co_await stdexec::schedule(scheduler_);
        }
    }();
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
constexpr auto
Queue<T, Capacity, Scheduler>::capacity() const noexcept -> std::size_t
{
    return Capacity;
}

} // namespace abc::async

#endif // ABC_INCLUDE_ABC_ASYNC_QUEUE
