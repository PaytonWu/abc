// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & the contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

// generate test for async queue
#include <abc/async/queue.h>

#include <exec/inline_scheduler.hpp>
#include <gtest/gtest.h>

TEST(async_queue, sync_enqueue_dequeue)
{
    using namespace abc::async;

    // Create a queue with a capacity of 5
    Queue<int, 5, exec::inline_scheduler> queue(exec::inline_scheduler{});

    // Test enqueue and dequeue operations
    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.full());
    EXPECT_EQ(queue.size(), 0);

    // Enqueue items
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_TRUE(queue.try_enqueue(i));
        EXPECT_EQ(queue.size(), i + 1);
        EXPECT_FALSE(queue.empty());
        if (i < 4)
        {
            EXPECT_FALSE(queue.full());
        }
        else
        {
            EXPECT_TRUE(queue.full());
        }
    }

    // Attempt to enqueue when full
    EXPECT_FALSE(queue.try_enqueue(5));
    EXPECT_EQ(queue.size(), 5);
    EXPECT_TRUE(queue.full());

    // Dequeue items
    for (int i = 0; i < 5; ++i)
    {
        auto item = queue.try_dequeue();
        ASSERT_TRUE(item.has_value());
        EXPECT_EQ(item.value(), i);
        EXPECT_EQ(queue.size(), 4 - i);
        if (i < 4)
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

TEST(async_queue, async_enqueue_dequeue)
{
    using namespace abc::async;

    // Create a queue with a capacity of 5
    stdexec::scheduler auto scheduler = exec::inline_scheduler{};
    Queue<int, 5, exec::inline_scheduler> queue(scheduler);

    // Test async enqueue and dequeue operations
    EXPECT_TRUE(queue.empty());
    EXPECT_FALSE(queue.full());
    EXPECT_EQ(queue.size(), 0);

    bool success1 = queue.try_enqueue(0);
    bool success2 = queue.try_enqueue(1);
    bool success3 = queue.try_enqueue(2);
    bool success4 = queue.try_enqueue(3);
    bool success5 = queue.try_enqueue(4);
    bool success6 = queue.try_enqueue(5); // This should fail

    EXPECT_TRUE(success1 && success2 && success3 && success4 && success5);
    EXPECT_FALSE(success6); // Queue should be full

    // Create concurrent enqueue/dequeue operations
    auto producer = [&queue]() -> exec::task<void> {
        co_await queue.enqueue(6);
        co_await queue.enqueue(7);
    };

    auto consumer = [&queue, &scheduler]() -> exec::task<void> {
        // Add a small delay to let producer try to enqueue first
        co_await stdexec::schedule(scheduler);

        for (int i = 0; i < 5; ++i)
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
