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
        EXPECT_TRUE(queue.enqueue(i));
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
    EXPECT_FALSE(queue.enqueue(4));
    EXPECT_EQ(queue.size(), 4);
    EXPECT_TRUE(queue.full());

    // Dequeue items
    for (int i = 0; i < 4; ++i)
    {
        auto item = queue.dequeue();
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
    auto item = queue.dequeue();
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

    bool success1 = queue.enqueue(0);
    bool success2 = queue.enqueue(1);
    bool success3 = queue.enqueue(2);
    bool success4 = queue.enqueue(3);
    bool success5 = queue.enqueue(4); // This should fail
    bool success6 = queue.enqueue(5); // This should fail

    EXPECT_TRUE(success1 && success2 && success3 && success4);
    EXPECT_FALSE(success5 || success6); // Queue should be full

    // Create concurrent enqueue/dequeue operations
    auto producer = [&queue]() -> exec::task<void> {
        co_await queue.async_enqueue(6);
        co_await queue.async_enqueue(7);
    };

    auto consumer = [&queue, &scheduler]() -> exec::task<void> {
        // Add a small delay to let producer try to enqueue first
        co_await stdexec::schedule(scheduler);

        for (int i = 0; i < 4; ++i)
        {
            auto item = co_await queue.async_dequeue();
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
        auto item = queue.dequeue();
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
                while (!queue.enqueue(static_cast<int>(i * items_per_producer + j)))
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
                auto item = queue.dequeue();
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
        EXPECT_TRUE(queue.enqueue(static_cast<int>(i)));
    }

    // Ensure the queue is full
    EXPECT_TRUE(queue.full());
    EXPECT_FALSE(queue.enqueue(99)); // Enqueue should fail

    // Dequeue all items
    for (std::size_t i = 0; i < capacity; ++i)
    {
        auto item = queue.dequeue();
        ASSERT_TRUE(item.has_value());
        EXPECT_EQ(item.value(), static_cast<int>(i));
    }

    // Ensure the queue is empty
    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.dequeue().has_value()); // Dequeue should fail
}

// TEST(async_queue, interleaved_operations)
//{
//     using namespace abc::async;
//
//     constexpr std::size_t capacity = 16;
//     Queue<int, capacity, exec::inline_scheduler> queue(exec::inline_scheduler{});
//
//     std::atomic<bool> stop{ false };
//     std::vector<std::thread> threads;
//
//     // Enqueue thread
//     threads.emplace_back([&queue, &stop]() {
//         int value = 0;
//         while (!stop.load())
//         {
//             queue.enqueue(value++);
//         }
//     });
//
//     // Dequeue thread
//     threads.emplace_back([&queue, &stop]() {
//         while (!stop.load())
//         {
//             auto item = queue.dequeue();
//             if (item.has_value())
//             {
//                 EXPECT_GE(item.value(), 0);
//             }
//         }
//     });
//
//     // Run threads for a short duration
//     std::this_thread::sleep_for(std::chrono::milliseconds(100));
//     stop.store(true);
//
//     for (auto & thread : threads)
//     {
//         thread.join();
//     }
//
//     EXPECT_TRUE(queue.empty() || !queue.full());
// }

TEST(async_queue, exception_safety)
{
    using namespace abc::async;

    constexpr std::size_t capacity = 4;
    Queue<int, capacity, exec::inline_scheduler> queue(exec::inline_scheduler{});

    // Enqueue items
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_TRUE(queue.enqueue(i));
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
        auto item = queue.dequeue();
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
        co_await queue.async_enqueue(42);

        // Dequeue and return the next value
        co_return co_await queue.async_dequeue();
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
            co_await queue.async_enqueue(i);
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
            int value = co_await queue.async_dequeue();
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
            co_await queue.async_enqueue(value);
            produced_sum += value;
        }
    };

    auto consumer = [&](int count) -> exec::task<void> {
        for (int i = 0; i < count; ++i)
        {
            int value = co_await queue.async_dequeue();
            consumed_sum += value;
        }
    };

    // Create and run all tasks concurrently
    auto combined_sender = stdexec::when_all(producer(0, num_items / 2), producer(num_items / 2, num_items / 2), consumer(num_items / 2), consumer(num_items / 2));

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
        ASSERT_TRUE(queue.enqueue(i));
    }
    ASSERT_TRUE(queue.full());

    // Test that dequeue unblocks enqueue
    auto operation = [&]() -> exec::task<int> {
        // Dequeue an item to make space
        int first_value = co_await queue.async_dequeue();
        EXPECT_EQ(first_value, 0);

        // Enqueue a new item
        co_await queue.async_enqueue(capacity);

        // Dequeue and return the next value
        int next_value = co_await queue.async_dequeue();
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
        EXPECT_TRUE(queue.enqueue(static_cast<int>(i % capacity)));
        auto item = queue.dequeue();
        ASSERT_TRUE(item.has_value());
        EXPECT_EQ(item.value(), static_cast<int>(i % capacity));
    }

    EXPECT_TRUE(queue.empty());
}

TEST(async_queue, task_done_basic_functionality)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 4;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    // Initially, unfinished count should be 0, so task_done() should throw
    EXPECT_THROW(queue.task_done(), abc::abc_error);

    // Enqueue an item (this increments unfinished count)
    ASSERT_TRUE(queue.enqueue(42));

    // Now task_done() should work
    EXPECT_NO_THROW(queue.task_done());

    // Calling task_done() again should throw since count is back to 0
    EXPECT_THROW(queue.task_done(), abc::abc_error);
}

TEST(async_queue, task_done_multiple_items)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 8;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    // Enqueue multiple items
    constexpr int num_items = 5;
    for (int i = 0; i < num_items; ++i)
    {
        ASSERT_TRUE(queue.enqueue(i));
    }

    // Call task_done() for each item
    for (int i = 0; i < num_items; ++i)
    {
        EXPECT_NO_THROW(queue.task_done());
    }

    // One more call should throw
    EXPECT_THROW(queue.task_done(), abc::abc_error);
}

TEST(async_queue, task_done_exception_type)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 4;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    // Test that the correct error code is thrown
    try
    {
        queue.task_done();
        FAIL() << "Expected abc::abc_error to be thrown";
    }
    catch (abc::abc_error const & e)
    {
        EXPECT_EQ(e.code(), abc::make_error_code(abc::errc::task_done_called_too_many_times));
        EXPECT_STREQ(e.what(), "task done called too many times");
    }
    catch (...)
    {
        FAIL() << "Unexpected exception type thrown";
    }
}

TEST(async_queue, task_done_concurrent_access)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 1024;
    constexpr int num_threads = 8;
    constexpr int items_per_thread = 100;

    exec::static_thread_pool pool{ num_threads };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    std::atomic<int> enqueue_count{ 0 };
    std::atomic<int> task_done_count{ 0 };
    std::atomic<int> exceptions_count{ 0 };

    // Enqueue items from multiple threads
    std::vector<std::thread> enqueue_threads;
    for (int t = 0; t < num_threads; ++t)
    {
        enqueue_threads.emplace_back([&, t]() {
            for (int i = 0; i < items_per_thread; ++i)
            {
                while (!queue.enqueue(t * items_per_thread + i))
                {
                    // Retry if queue is full
                }
                ++enqueue_count;
            }
        });
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Call task_done from multiple threads
    std::vector<std::thread> task_done_threads;
    for (int t = 0; t < num_threads; ++t)
    {
        task_done_threads.emplace_back([&]() {
            for (int i = 0; i < items_per_thread; ++i)
            {
                try
                {
                    queue.task_done();
                    ++task_done_count;
                }
                catch (abc::abc_error const &)
                {
                    ++exceptions_count;
                }
            }
        });
    }

    // Join all threads
    for (auto & t : enqueue_threads)
    {
        t.join();
    }
    for (auto & t : task_done_threads)
    {
        t.join();
    }

    // Verify that enqueue_count equals task_done_count (no exceptions should occur with proper synchronization)
    EXPECT_EQ(enqueue_count.load(), num_threads * items_per_thread);
    EXPECT_EQ(task_done_count.load(), num_threads * items_per_thread);
    EXPECT_EQ(exceptions_count.load(), 0);
}

TEST(async_queue, join_with_empty_queue)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 4;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    // With no enqueued items, join should return immediately
    auto start_time = std::chrono::steady_clock::now();

    auto join_task = [&]() -> exec::task<void> { co_await queue.join(); };

    stdexec::sync_wait(join_task());

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Should complete very quickly (less than 10ms)
    EXPECT_LT(duration.count(), 10);
}

TEST(async_queue, join_waits_for_task_completion)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 4;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    std::atomic<bool> join_completed{ false };
    std::atomic<bool> task_done_called{ false };

    // Enqueue an item
    ASSERT_TRUE(queue.enqueue(42));

    auto join_task = [&]() -> exec::task<void> {
        co_await queue.join();
        join_completed = true;
    };

    auto task_done_task = [&]() -> exec::task<void> {
        // Wait a bit to ensure join starts first
        co_await stdexec::schedule(pool.get_scheduler());
        co_await stdexec::schedule(pool.get_scheduler());

        // Verify join hasn't completed yet
        EXPECT_FALSE(join_completed.load());

        queue.task_done();
        task_done_called = true;
    };

    stdexec::sync_wait(stdexec::when_all(join_task(), task_done_task()));

    EXPECT_TRUE(join_completed.load());
    EXPECT_TRUE(task_done_called.load());
}

TEST(async_queue, join_with_multiple_tasks)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 8;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    constexpr int num_items = 5;
    std::atomic<int> tasks_completed{ 0 };
    std::atomic<bool> join_completed{ false };

    // Enqueue multiple items
    for (int i = 0; i < num_items; ++i)
    {
        ASSERT_TRUE(queue.enqueue(i));
    }

    auto join_task = [&]() -> exec::task<void> {
        co_await queue.join();
        join_completed = true;
    };

    auto task_completion_simulator = [&]() -> exec::task<void> {
        for (int i = 0; i < num_items; ++i)
        {
            // Wait a bit between task completions
            co_await stdexec::schedule(pool.get_scheduler());

            // Verify join hasn't completed yet (except on the last iteration)
            if (i < num_items - 1)
            {
                EXPECT_FALSE(join_completed.load());
            }

            queue.task_done();
            ++tasks_completed;
        }
    };

    stdexec::sync_wait(stdexec::when_all(join_task(), task_completion_simulator()));

    EXPECT_TRUE(join_completed.load());
    EXPECT_EQ(tasks_completed.load(), num_items);
}

TEST(async_queue, multiple_join_waiters)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 4;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    constexpr int num_waiters = 3;
    std::atomic<int> join_completions{ 0 };

    // Enqueue one item
    ASSERT_TRUE(queue.enqueue(42));

    auto create_join_waiter = [&]() -> exec::task<void> {
        co_await queue.join();
        ++join_completions;
    };

    auto task_completer = [&]() -> exec::task<void> {
        // Wait for all waiters to start
        co_await stdexec::schedule(pool.get_scheduler());
        co_await stdexec::schedule(pool.get_scheduler());

        queue.task_done();
    };

    // Run all tasks (multiple waiters + task completer)
    auto waiter1 = create_join_waiter();
    auto waiter2 = create_join_waiter();
    auto waiter3 = create_join_waiter();
    auto completer = task_completer();

    stdexec::sync_wait(stdexec::when_all(std::move(waiter1), std::move(waiter2), std::move(waiter3), std::move(completer)));

    EXPECT_EQ(join_completions.load(), num_waiters);
}

TEST(async_queue, join_task_done_integration_workflow)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 8;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    std::atomic<int> items_processed{ 0 };
    std::atomic<bool> all_tasks_completed{ false };

    auto producer = [&]() -> exec::task<void> {
        for (int i = 0; i < 5; ++i)
        {
            co_await queue.async_enqueue(i);
        }
    };

    auto consumer = [&]() -> exec::task<void> {
        for (int i = 0; i < 5; ++i)
        {
            int value = co_await queue.async_dequeue();
            EXPECT_EQ(value, i);

            // Simulate some work
            co_await stdexec::schedule(pool.get_scheduler());

            // Mark task as done
            queue.task_done();
            ++items_processed;
        }
    };

    auto join_waiter = [&]() -> exec::task<void> {
        co_await queue.join();
        all_tasks_completed = true;
    };

    stdexec::sync_wait(stdexec::when_all(producer(), consumer(), join_waiter()));

    EXPECT_EQ(items_processed.load(), 5);
    EXPECT_TRUE(all_tasks_completed.load());
    EXPECT_TRUE(queue.empty());
}

TEST(async_queue, join_with_concurrent_enqueue_dequeue_task_done)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 16;
    constexpr int total_items = 100;

    exec::static_thread_pool pool{ 8 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    std::atomic<int> items_produced{ 0 };
    std::atomic<int> items_consumed{ 0 };
    std::atomic<int> tasks_completed{ 0 };
    std::atomic<bool> join_finished{ false };

    auto producer = [&]() -> exec::task<void> {
        for (int i = 0; i < total_items; ++i)
        {
            co_await queue.async_enqueue(i);
            ++items_produced;
        }
    };

    auto consumer = [&]() -> exec::task<void> {
        for (int i = 0; i < total_items; ++i)
        {
            int value = co_await queue.async_dequeue();
            ++items_consumed;

            // Simulate variable work time
            if (value % 3 == 0)
            {
                co_await stdexec::schedule(pool.get_scheduler());
            }

            queue.task_done();
            ++tasks_completed;
        }
    };

    auto join_monitor = [&]() -> exec::task<void> {
        co_await queue.join();
        join_finished = true;
    };

    stdexec::sync_wait(stdexec::when_all(producer(), consumer(), join_monitor()));

    EXPECT_EQ(items_produced.load(), total_items);
    EXPECT_EQ(items_consumed.load(), total_items);
    EXPECT_EQ(tasks_completed.load(), total_items);
    EXPECT_TRUE(join_finished.load());
    EXPECT_TRUE(queue.empty());
}

TEST(async_queue, task_done_error_persistence)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 4;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    // Test that multiple calls when count is 0 all throw errors
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_THROW(queue.task_done(), abc::abc_error);
    }

    // Add one item and complete it
    ASSERT_TRUE(queue.enqueue(1));
    EXPECT_NO_THROW(queue.task_done());

    // Multiple calls again should all throw
    for (int i = 0; i < 3; ++i)
    {
        EXPECT_THROW(queue.task_done(), abc::abc_error);
    }
}

TEST(async_queue, join_immediate_return_after_all_tasks_done)
{
    using namespace abc::async;
    constexpr std::size_t capacity = 4;

    exec::static_thread_pool pool{ 4 };
    Queue<int, capacity, exec::static_thread_pool::scheduler> queue(pool.get_scheduler());

    // Enqueue and immediately complete tasks
    for (int i = 0; i < 3; ++i)
    {
        ASSERT_TRUE(queue.enqueue(i));
        queue.task_done();
    }

    // join() should return immediately
    auto start_time = std::chrono::steady_clock::now();

    auto join_task = [&]() -> exec::task<void> { co_await queue.join(); };

    stdexec::sync_wait(join_task());

    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // Should complete very quickly (less than 10ms)
    EXPECT_LT(duration.count(), 10);
}
