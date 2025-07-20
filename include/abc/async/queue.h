// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#ifndef ABC_INCLUDE_ABC_ASYNC_QUEUE
#define ABC_INCLUDE_ABC_ASYNC_QUEUE

#pragma once

#include "queue_decl.h"

#include "abc/error.h"

namespace abc::async
{

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
Queue<T, Capacity, Scheduler>::Queue(Scheduler scheduler) : scheduler_{ scheduler }
{
    static_assert(Capacity > 0, "Queue capacity must be greater than 0");
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be a power of 2");

    // Initialize all sequence numbers
    for (size_t i = 0; i < Capacity; ++i)
    {
        buffer_[i].sequence.store(i, std::memory_order_relaxed);
    }
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
bool
Queue<T, Capacity, Scheduler>::empty() const noexcept
{
    return head_.load(std::memory_order_acquire) == tail_.load(std::memory_order_acquire);
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
bool
Queue<T, Capacity, Scheduler>::full() const noexcept
{
    std::size_t head = head_.load(std::memory_order_acquire);
    std::size_t tail = tail_.load(std::memory_order_acquire);
    return (tail - head) == Capacity;
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
std::size_t
Queue<T, Capacity, Scheduler>::size() const noexcept
{
    std::size_t head = head_.load(std::memory_order_acquire);
    std::size_t tail = tail_.load(std::memory_order_acquire);
    return tail - head;
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
bool
Queue<T, Capacity, Scheduler>::enqueue(T && item)
{
    std::size_t pos = tail_.load(std::memory_order_relaxed);

    for (;;)
    {
        // Get the cell at the current position
        Cell * cell = &buffer_[pos & (Capacity - 1)];
        std::size_t seq = cell->sequence.load(std::memory_order_acquire);
        intptr_t diff = (intptr_t)seq - (intptr_t)pos;

        if (diff == 0)
        {
            // Cell is available, try to claim it
            if (tail_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
            {
                // Store the data
                cell->data = std::move(item);
                // Make the data available to consumers
                cell->sequence.store(pos + 1, std::memory_order_release);
                unfinished_.fetch_add(1, std::memory_order_release);
                return true;
            }
        }
        else if (diff < 0)
        {
            // Queue is full
            return false;
        }
        else
        {
            // Another thread claimed the cell, get updated position
            pos = tail_.load(std::memory_order_relaxed);
        }
    }
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
bool
Queue<T, Capacity, Scheduler>::enqueue(T const & item)
{
    std::size_t pos = tail_.load(std::memory_order_relaxed);

    for (;;)
    {
        // Get the cell at the current position
        Cell * cell = &buffer_[pos & (Capacity - 1)];
        std::size_t seq = cell->sequence.load(std::memory_order_acquire);
        intptr_t diff = (intptr_t)seq - (intptr_t)pos;

        if (diff == 0)
        {
            // Cell is available, try to claim it
            if (tail_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
            {
                // Store the data
                cell->data = item;
                // Make the data available to consumers
                cell->sequence.store(pos + 1, std::memory_order_release);
                unfinished_.fetch_add(1, std::memory_order_release);
                return true;
            }
        }
        else if (diff < 0)
        {
            // Queue is full
            return false;
        }
        else
        {
            // Another thread claimed the cell, get updated position
            pos = tail_.load(std::memory_order_relaxed);
        }
    }
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
std::optional<T>
Queue<T, Capacity, Scheduler>::dequeue()
{
    std::size_t pos = head_.load(std::memory_order_relaxed);

    for (;;)
    {
        // Get the cell at the current position
        Cell * cell = &buffer_[pos & (Capacity - 1)];
        std::size_t seq = cell->sequence.load(std::memory_order_acquire);
        intptr_t diff = (intptr_t)seq - (intptr_t)(pos + 1);

        if (diff == 0)
        {
            // Cell has data, try to claim it
            if (head_.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
            {
                // Extract the data
                T result = std::move(cell->data);
                // Mark the cell as empty
                cell->sequence.store(pos + Capacity, std::memory_order_release);
                return result;
            }
        }
        else if (diff < 0)
        {
            // Queue is empty
            return std::nullopt;
        }
        else
        {
            // Another thread claimed the cell, get updated position
            pos = head_.load(std::memory_order_relaxed);
        }
    }
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
auto
Queue<T, Capacity, Scheduler>::async_enqueue(T item) -> exec::task<void>
{
    while (true)
    {
        if (enqueue(std::move(item)))
        {
            co_return;
        }

        // Queue is full, yield and retry
        co_await stdexec::schedule(scheduler_);
    }
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
auto
Queue<T, Capacity, Scheduler>::async_dequeue() -> exec::task<T>
{
    while (true)
    {
        auto result = dequeue();
        if (result)
        {
            co_return std::move(*result);
        }

        // Queue is empty, yield and retry
        co_await stdexec::schedule(scheduler_);
    }
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
constexpr auto
Queue<T, Capacity, Scheduler>::capacity() const noexcept -> std::size_t
{
    return Capacity;
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
auto
Queue<T, Capacity, Scheduler>::task_done() -> void
{
    if (unfinished_.load(std::memory_order_relaxed) <= 0)
    {
        throw_error(errc::task_done_called_too_many_times);
    }

    unfinished_.fetch_sub(1, std::memory_order_relaxed);
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
auto
Queue<T, Capacity, Scheduler>::wait_for(auto pred) -> exec::task<T>
{
    while (true)
    {
        auto result = dequeue();
        if (result)
        {
            if (auto value = std::move(*result); static_cast<bool>(pred(value)))
            {
                co_return value;
            }
        }

        co_await stdexec::schedule(scheduler_);
    }
}

template <typename T, std::size_t Capacity, stdexec::scheduler Scheduler>
auto
Queue<T, Capacity, Scheduler>::join() -> exec::task<void>
{
    if (unfinished_.load(std::memory_order_relaxed) > 0)
    {
        co_await stdexec::schedule(scheduler_);
    }

    co_return;
}

} // namespace abc::async

#endif // ABC_INCLUDE_ABC_ASYNC_QUEUE
