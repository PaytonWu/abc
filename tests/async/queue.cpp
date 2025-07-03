// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

// generate test for async queue
#include <abc/async/queue.h>

#include <exec/inline_scheduler.hpp>
#include <exec/static_thread_pool.hpp>
#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>
#include <thread>

TEST(async_queue, sync_enqueue_dequeue)
{
    using namespace abc::async;

    // Create a queue with a capacity of 5
    Queue<int, 4, exec::inline_scheduler> queue(exec::inline_scheduler{});

    // Test enqueue and dequeue operations
    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.full());
    EXPECT_EQ(queue.size(), 0);

    // Enqueue items
    for (int i = 0; i < 4; ++i)
    {
        EXPECT_TRUE(queue.try_enqueue(i));
        EXPECT_EQ(queue.size(), i + 1);
        EXPECT_FALSE(queue.empty());
        if (i < 3)
        {
            EXPECT_FALSE(queue.full());
        }
        else
        {
            EXPECT_TRUE(queue.full());
        }
    }

    // Attempt to enqueue when full
    EXPECT_FALSE(queue.try_enqueue(4));
    EXPECT_EQ(queue.size(), 4);
    EXPECT_TRUE(queue.full());

    // Dequeue items
    for (int i = 0; i < 4; ++i)
    {
        auto item = queue.try_dequeue();
        ASSERT_TRUE(item.has_value());
        EXPECT_EQ(item.value(), i);
        EXPECT_EQ(queue.size(), 3 - i);
        if (i < 3)
        {
            EXPECT_FALSE(queue.empty());
            EXPECT_FALSE(queue.full());
        }
        else
        {
            EXPECT_TRUE(queue.empty());
            EXPECT_FALSE(queue.full());
        }
    }

    // Attempt to dequeue when empty
    auto item = queue.try_dequeue();
    EXPECT_FALSE(item.has_value());
}

TEST(async_queue, basic_async_enqueue_dequeue)
{
    using namespace abc::async;

    // Create a queue with a capacity of 5
    stdexec::scheduler auto scheduler = exec::inline_scheduler{};
    Queue<int, 4, exec::inline_scheduler> queue(scheduler);

    // Test async enqueue and dequeue operations
    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.full());
    EXPECT_EQ(queue.size(), 0);

    bool success1 = queue.try_enqueue(0);
    bool success2 = queue.try_enqueue(1);
    bool success3 = queue.try_enqueue(2);
    bool success4 = queue.try_enqueue(3);
    bool success5 = queue.try_enqueue(4); // This should fail
    bool success6 = queue.try_enqueue(5); // This should fail

    EXPECT_TRUE(success1 && success2 && success3 && success4);
    EXPECT_FALSE(success5 || success6); // Queue should be full

    // Create concurrent enqueue/dequeue operations
    auto producer = [&queue]() -> exec::task<void> {
        co_await queue.enqueue(6);
        co_await queue.enqueue(7);
    };

    auto consumer = [&queue, &scheduler]() -> exec::task<void> {
        // Add a small delay to let producer try to enqueue first
        co_await stdexec::schedule(scheduler);

        for (int i = 0; i < 4; ++i)
        {
            auto item = co_await queue.dequeue();
            EXPECT_EQ(item, i);
        }
    };

    // Run producer and consumer concurrently
    auto concurrent_work = stdexec::when_all(producer(), consumer());
    stdexec::sync_wait(std::move(concurrent_work));

    EXPECT_FALSE(queue.empty());
    EXPECT_FALSE(queue.full());
    EXPECT_EQ(queue.size(), 2); // Only one item (5) should remain in

    // Clean up remaining items
    auto const sz = queue.size();
    for (auto i = 0uz; i < sz; ++i)
    {
        auto item = queue.try_dequeue();
        ASSERT_TRUE(item.has_value());
        EXPECT_EQ(item.value(), 6 + i);
    }
}

TEST(async_queue, concurrent_producers_consumers)
{
    using namespace abc::async;

    constexpr std::size_t capacity = 1024;
    constexpr std::size_t num_producers = 4;
    constexpr std::size_t num_consumers = 4;
    constexpr std::size_t items_per_producer = 1000;

    exec::static_thread_pool thread_pool{ 4 };
    stdexec::scheduler auto scheduler = thread_pool.get_scheduler();
    Queue<int, capacity, decltype(scheduler)> queue(scheduler);
    std::atomic<std::size_t> produced_count{ 0 };
    std::atomic<std::size_t> consumed_count{ 0 };

    // Producers
    std::vector<std::thread> producers;
    for (std::size_t i = 0; i < num_producers; ++i)
    {
        producers.emplace_back([&queue, &produced_count, items_per_producer, i]() {
            for (std::size_t j = 0; j < items_per_producer; ++j)
            {
                while (!queue.try_enqueue(static_cast<int>(i * items_per_producer + j)))
                {
                    // Retry until successful
                }
                ++produced_count;
            }
        });
    }

    // Consumers
    std::vector<std::thread> consumers;
    for (std::size_t i = 0; i < num_consumers; ++i)
    {
        consumers.emplace_back([&queue, &consumed_count]() {
            while (true)
            {
                auto item = queue.try_dequeue();
                if (item.has_value())
                {
                    ++consumed_count;
                }
                else if (consumed_count.load() == num_producers * items_per_producer)
                {
                    break; // Exit when all items are consumed
                }
            }
        });
    }

    // Join threads
    for (auto & producer : producers)
    {
        producer.join();
    }
    for (auto & consumer : consumers)
    {
        consumer.join();
    }

    EXPECT_EQ(produced_count.load(), num_producers * items_per_producer);
    EXPECT_EQ(consumed_count.load(), num_producers * items_per_producer);
    EXPECT_TRUE(queue.empty());
}

TEST(async_queue, boundary_conditions)
{
    using namespace abc::async;

    constexpr std::size_t capacity = 8;
    Queue<int, capacity, exec::inline_scheduler> queue(exec::inline_scheduler{});

    // Fill the queue to its capacity
    for (std::size_t i = 0; i < capacity; ++i)
    {
        EXPECT_TRUE(queue.try_enqueue(static_cast<int>(i)));
    }

    // Ensure the queue is full
    EXPECT_TRUE(queue.full());
    EXPECT_FALSE(queue.try_enqueue(99)); // Enqueue should fail

    // Dequeue all items
    for (std::size_t i = 0; i < capacity; ++i)
    {
        auto item = queue.try_dequeue();
        ASSERT_TRUE(item.has_value());
        EXPECT_EQ(item.value(), static_cast<int>(i));
    }

    // Ensure the queue is empty
    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.try_dequeue().has_value()); // Dequeue should fail
}

TEST(async_queue, interleaved_operations)
{
    using namespace abc::async;

    constexpr std::size_t capacity = 16;
    Queue<int, capacity, exec::inline_scheduler> queue(exec::inline_scheduler{});

    std::atomic<bool> stop{ false };
    std::vector<std::thread> threads;

    // Enqueue thread
    threads.emplace_back([&queue, &stop]() {
        int value = 0;
        while (!stop.load())
        {
            queue.try_enqueue(value++);
        }
    });

    // Dequeue thread
    threads.emplace_back([&queue, &stop]() {
        while (!stop.load())
        {
            auto item = queue.try_dequeue();
            if (item.has_value())
            {
                EXPECT_GE(item.value(), 0);
            }
        }
    });

    // Run threads for a short duration
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    stop.store(true);

    for (auto & thread : threads)
    {
        thread.join();
    }

    EXPECT_TRUE(queue.empty() || !queue.full());
}

TEST(async_queue, exception_safety)
{
    using namespace abc::async;

    constexpr std::size_t capacity = 4;
    Queue<int, capacity, exec::inline_scheduler> queue(exec::inline_scheduler{});

    // Enqueue items
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_TRUE(queue.try_enqueue(i));
    }

    // Simulate exception during enqueue
    try
    {
        throw std::runtime_error("Simulated exception");
    }
    catch (std::exception const & e)
    {
        EXPECT_EQ(std::string(e.what()), "Simulated exception");
    }

    // Ensure queue state is consistent
    EXPECT_EQ(queue.size(), 3);
    EXPECT_FALSE(queue.full());

    // Dequeue items
    for (int i = 0; i < 3; ++i)
    {
        auto item = queue.try_dequeue();
        ASSERT_TRUE(item.has_value());
        EXPECT_EQ(item.value(), i);
    }

    EXPECT_TRUE(queue.empty());
}

TEST(async_queue, async_enqueue_dequeue)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 8;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    auto operation = [&]() -> exec::task<int> {
        // Enqueue a new item
        co_await queue.enqueue(42);

        // Dequeue and return the next value
        co_return co_await queue.dequeue();
    };

    auto [result] = stdexec::sync_wait(operation()).value();

    EXPECT_EQ(result, 42);
    EXPECT_TRUE(queue.empty());
}

TEST(async_queue, async_backpressure)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 4;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    std::atomic<int> completed_enqueues{ 0 };
    std::atomic<bool> dequeue_started{ false };
    std::atomic<int> dequeued_values{ 0 };

    // Fill the queue and attempt to enqueue more
    auto producer = [&]() -> exec::task<void> {
        for (std::size_t i = 0; i < 8; ++i)
        {
            co_await queue.enqueue(i);
            completed_enqueues++;

            // Signal consumer to start after queue is full
            if (i >= capacity - 1)
            {
                dequeue_started = true;
            }
        }
    };

    auto consumer = [&]() -> exec::task<void> {
        // Wait until queue is full before starting
        while (!dequeue_started)
        {
            co_await stdexec::schedule(pool.get_scheduler());
        }

        // Dequeue all values
        int i = 0;
        for (i = 0; i < 8; ++i)
        {
            int value = co_await queue.dequeue();
            EXPECT_EQ(value, i);
            dequeued_values++;
        }
    };

    // Run both operations concurrently
    stdexec::sync_wait(stdexec::when_all(producer(), consumer()));

    EXPECT_EQ(completed_enqueues.load(), 8);
    EXPECT_EQ(dequeued_values.load(), 8);
    EXPECT_TRUE(queue.empty());
}

TEST(async_queue, multiple_async_producers_consumers)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 16;
    constexpr int num_items = 100;

    exec::static_thread_pool pool{ 8 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    std::atomic<int> produced_sum{ 0 };
    std::atomic<int> consumed_sum{ 0 };

    auto producer = [&](int start, int count) -> exec::task<void> {
        for (int i = 0; i < count; ++i)
        {
            int value = start + i;
            co_await queue.enqueue(value);
            produced_sum += value;
        }
    };

    auto consumer = [&](int count) -> exec::task<void> {
        for (int i = 0; i < count; ++i)
        {
            int value = co_await queue.dequeue();
            consumed_sum += value;
        }
    };

    // Create and run all tasks concurrently
    auto combined_sender = stdexec::when_all(
        producer(0, num_items / 2),
        producer(num_items / 2, num_items / 2),
        consumer(num_items / 2),
        consumer(num_items / 2)
    );

    // Run all tasks concurrently
    stdexec::sync_wait(std::move(combined_sender));

    EXPECT_EQ(consumed_sum.load(), produced_sum.load());
    EXPECT_TRUE(queue.empty());
}

TEST(async_queue, interleaved_async_operations)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 4;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    // First fill the queue
    for (std::size_t i = 0; i < capacity; ++i)
    {
        ASSERT_TRUE(queue.try_enqueue(i));
    }
    ASSERT_TRUE(queue.full());

    // Test that dequeue unblocks enqueue
    auto operation = [&]() -> exec::task<int> {
        // Dequeue an item to make space
        int first_value = co_await queue.dequeue();
        EXPECT_EQ(first_value, 0);

        // Enqueue a new item
        co_await queue.enqueue(capacity);

        // Dequeue and return the next value
        int next_value = co_await queue.dequeue();
        co_return next_value;
    };

    auto [result] = stdexec::sync_wait(operation()).value();

    EXPECT_EQ(result, 1);
    EXPECT_EQ(queue.size(), capacity - 1);
}

TEST(async_queue, stress_test)
{
    using namespace abc::async;

    constexpr std::size_t capacity = 1024;
    constexpr std::size_t num_operations = 100000;

    exec::static_thread_pool thread_pool{ 4 };
    stdexec::scheduler auto scheduler = thread_pool.get_scheduler();
    // Create a queue with a specified capacity and inline scheduler
    Queue<int, capacity, decltype(scheduler)> queue(scheduler);

    // Stress test with alternating enqueue and dequeue
    for (std::size_t i = 0; i < num_operations; ++i)
    {
        EXPECT_TRUE(queue.try_enqueue(static_cast<int>(i % capacity)));
        auto item = queue.try_dequeue();
        ASSERT_TRUE(item.has_value());
        EXPECT_EQ(item.value(), static_cast<int>(i % capacity));
    }

    EXPECT_TRUE(queue.empty());
}
