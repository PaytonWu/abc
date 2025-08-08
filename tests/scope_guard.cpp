// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/scope_guard.h>

#include <gtest/gtest.h>

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace
{

// Test fixtures
class ScopeExitTest : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        // Reset any global state if needed
    }
};

class ScopeFailTest : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        // Reset any global state if needed
    }
};

class ScopeSuccessTest : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        // Reset any global state if needed
    }
};

// Helper functions
void
throw_exception()
{
    throw std::runtime_error("test exception");
}

void
no_throw_function()
{
    // Do nothing
}

} // namespace

// =============================================================================
// ScopeExit Tests
// =============================================================================

TEST_F(ScopeExitTest, BasicFunctionality)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_exit([&executed] { executed = true; });
        EXPECT_FALSE(executed);
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeExitTest, DirectConstruction)
{
    bool executed = false;
    {
        abc::ScopeExit<std::function<void()>> guard([&executed] { executed = true; });
        EXPECT_FALSE(executed);
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeExitTest, LambdaCapture)
{
    std::string message = "initial";
    {
        auto guard = abc::make_scope_exit([&message] { message = "executed"; });
        EXPECT_EQ(message, "initial");
    }
    EXPECT_EQ(message, "executed");
}

TEST_F(ScopeExitTest, MoveConstructor)
{
    bool executed = false;
    {
        auto guard1 = abc::make_scope_exit([&executed] { executed = true; });

        auto guard2 = std::move(guard1);
        // guard1 should be released after move
        EXPECT_FALSE(executed);
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeExitTest, ReleaseMethod)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_exit([&executed] { executed = true; });
        guard.release();
        EXPECT_FALSE(executed);
    }
    EXPECT_FALSE(executed); // Should still be false after destruction
}

TEST_F(ScopeExitTest, ExecutesOnNormalExit)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_exit([&executed] { executed = true; });
        no_throw_function();
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeExitTest, ExecutesOnException)
{
    bool executed = false;

    try
    {
        auto guard = abc::make_scope_exit([&executed] { executed = true; });
        throw_exception();
    }
    catch (std::runtime_error const &)
    {
        // Exception caught
    }

    EXPECT_TRUE(executed);
}

TEST_F(ScopeExitTest, MultipleGuards)
{
    std::vector<int> execution_order;
    {
        auto guard1 = abc::make_scope_exit([&execution_order] { execution_order.push_back(1); });

        auto guard2 = abc::make_scope_exit([&execution_order] { execution_order.push_back(2); });

        auto guard3 = abc::make_scope_exit([&execution_order] { execution_order.push_back(3); });

        EXPECT_TRUE(execution_order.empty());
    }

    // Guards execute in reverse order of declaration (LIFO)
    ASSERT_EQ(execution_order.size(), 3);
    EXPECT_EQ(execution_order[0], 3);
    EXPECT_EQ(execution_order[1], 2);
    EXPECT_EQ(execution_order[2], 1);
}

TEST_F(ScopeExitTest, FunctionObject)
{
    struct Functor
    {
        bool & executed;

        Functor(bool & exec) : executed(exec)
        {
        }

        void
        operator()() const
        {
            executed = true;
        }
    };

    bool executed = false;
    {
        auto guard = abc::make_scope_exit(Functor(executed));
        EXPECT_FALSE(executed);
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeExitTest, TypeTraits)
{
    auto guard = abc::make_scope_exit([] {});

    // Should not be copy constructible or assignable
    EXPECT_FALSE(std::is_copy_constructible_v<decltype(guard)>);
    EXPECT_FALSE(std::is_copy_assignable_v<decltype(guard)>);
    EXPECT_FALSE(std::is_move_assignable_v<decltype(guard)>);

    // Should be move constructible
    EXPECT_TRUE(std::is_move_constructible_v<decltype(guard)>);
}

// =============================================================================
// ScopeFail Tests
// =============================================================================

TEST_F(ScopeFailTest, BasicFunctionality)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_fail([&executed] { executed = true; });
        EXPECT_FALSE(executed);
    }
    // Should NOT execute on normal exit
    EXPECT_FALSE(executed);
}

TEST_F(ScopeFailTest, ExecutesOnException)
{
    bool executed = false;

    try
    {
        auto guard = abc::make_scope_fail([&executed] { executed = true; });
        throw_exception();
    }
    catch (std::runtime_error const &)
    {
        // Exception caught
    }

    // Should execute when exiting due to exception
    EXPECT_TRUE(executed);
}

TEST_F(ScopeFailTest, DoesNotExecuteOnNormalExit)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_fail([&executed] { executed = true; });
        no_throw_function();
    }
    EXPECT_FALSE(executed);
}

TEST_F(ScopeFailTest, ReleaseMethod)
{
    bool executed = false;

    try
    {
        auto guard = abc::make_scope_fail([&executed] { executed = true; });
        guard.release();
        throw_exception();
    }
    catch (std::runtime_error const &)
    {
        // Exception caught
    }

    // Should not execute because guard was released
    EXPECT_FALSE(executed);
}

TEST_F(ScopeFailTest, MoveConstructor)
{
    bool executed = false;

    try
    {
        auto guard1 = abc::make_scope_fail([&executed] { executed = true; });

        auto guard2 = std::move(guard1);
        // guard1 should be released after move
        throw_exception();
    }
    catch (std::runtime_error const &)
    {
        // Exception caught
    }

    EXPECT_TRUE(executed); // guard2 should execute
}

TEST_F(ScopeFailTest, NestedExceptions)
{
    bool outer_executed = false;
    bool inner_executed = false;

    try
    {
        auto outer_guard = abc::make_scope_fail([&outer_executed] { outer_executed = true; });

        try
        {
            auto inner_guard = abc::make_scope_fail([&inner_executed] { inner_executed = true; });
            throw_exception();
        }
        catch (std::runtime_error const &)
        {
            // Inner exception caught, so inner_guard should execute
        }

        // After catching the inner exception, we're no longer in an exception state
        // for the outer scope, so outer_guard should NOT execute
    }
    catch (...)
    {
        // Should not reach here
        FAIL() << "Unexpected exception in outer scope";
    }

    EXPECT_TRUE(inner_executed);  // Inner guard should execute due to exception
    EXPECT_FALSE(outer_executed); // Outer guard should not execute (normal exit)
}

TEST_F(ScopeFailTest, MultipleGuardsWithException)
{
    std::vector<int> execution_order;

    try
    {
        auto guard1 = abc::make_scope_fail([&execution_order] { execution_order.push_back(1); });

        auto guard2 = abc::make_scope_fail([&execution_order] { execution_order.push_back(2); });

        auto guard3 = abc::make_scope_fail([&execution_order] { execution_order.push_back(3); });

        throw_exception();
    }
    catch (std::runtime_error const &)
    {
        // Exception caught
    }

    // All guards should execute in reverse order of declaration (LIFO)
    ASSERT_EQ(execution_order.size(), 3);
    EXPECT_EQ(execution_order[0], 3);
    EXPECT_EQ(execution_order[1], 2);
    EXPECT_EQ(execution_order[2], 1);
}

TEST_F(ScopeFailTest, TypeTraits)
{
    auto guard = abc::make_scope_fail([] {});

    // Should not be copy constructible or assignable
    EXPECT_FALSE(std::is_copy_constructible_v<decltype(guard)>);
    EXPECT_FALSE(std::is_copy_assignable_v<decltype(guard)>);
    EXPECT_FALSE(std::is_move_assignable_v<decltype(guard)>);

    // Should be move constructible
    EXPECT_TRUE(std::is_move_constructible_v<decltype(guard)>);
}

// =============================================================================
// ScopeSuccess Tests
// =============================================================================

TEST_F(ScopeSuccessTest, BasicFunctionality)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_success([&executed] { executed = true; });
        EXPECT_FALSE(executed);
    }
    // Should execute on normal exit
    EXPECT_TRUE(executed);
}

TEST_F(ScopeSuccessTest, ExecutesOnNormalExit)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_success([&executed] { executed = true; });
        no_throw_function();
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeSuccessTest, DoesNotExecuteOnException)
{
    bool executed = false;

    try
    {
        auto guard = abc::make_scope_success([&executed] { executed = true; });
        throw_exception();
    }
    catch (std::runtime_error const &)
    {
        // Exception caught
    }

    // Should NOT execute when exiting due to exception
    EXPECT_FALSE(executed);
}

TEST_F(ScopeSuccessTest, ReleaseMethod)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_success([&executed] { executed = true; });
        guard.release();
    }
    // Should not execute because guard was released
    EXPECT_FALSE(executed);
}

TEST_F(ScopeSuccessTest, MoveConstructor)
{
    bool executed = false;
    {
        auto guard1 = abc::make_scope_success([&executed] { executed = true; });

        auto guard2 = std::move(guard1);
        // guard1 should be released after move
        EXPECT_FALSE(executed);
    }
    EXPECT_TRUE(executed); // guard2 should execute
}

TEST_F(ScopeSuccessTest, NestedSuccessScopes)
{
    bool outer_executed = false;
    bool inner_executed = false;

    {
        auto outer_guard = abc::make_scope_success([&outer_executed] { outer_executed = true; });

        {
            auto inner_guard = abc::make_scope_success([&inner_executed] { inner_executed = true; });
            // Normal exit from inner scope
        }

        EXPECT_TRUE(inner_executed);  // Inner guard should execute
        EXPECT_FALSE(outer_executed); // Outer guard not executed yet

        // Normal exit from outer scope
    }

    EXPECT_TRUE(outer_executed); // Outer guard should execute
}

TEST_F(ScopeSuccessTest, NestedWithException)
{
    bool outer_executed = false;
    bool inner_executed = false;

    try
    {
        auto outer_guard = abc::make_scope_success([&outer_executed] { outer_executed = true; });

        try
        {
            auto inner_guard = abc::make_scope_success([&inner_executed] { inner_executed = true; });
            throw_exception();
        }
        catch (std::runtime_error const &)
        {
            // Inner exception caught
        }

        // After catching the inner exception, we continue normally
        // so the outer guard should execute
    }
    catch (...)
    {
        // Should not reach here
        FAIL() << "Unexpected exception in outer scope";
    }

    EXPECT_FALSE(inner_executed); // Inner guard should not execute due to exception
    EXPECT_TRUE(outer_executed);  // Outer guard should execute (normal exit)
}

TEST_F(ScopeSuccessTest, MultipleGuardsNormalExit)
{
    std::vector<int> execution_order;
    {
        auto guard1 = abc::make_scope_success([&execution_order] { execution_order.push_back(1); });

        auto guard2 = abc::make_scope_success([&execution_order] { execution_order.push_back(2); });

        auto guard3 = abc::make_scope_success([&execution_order] { execution_order.push_back(3); });

        // Normal exit
    }

    // All guards should execute in reverse order of declaration (LIFO)
    ASSERT_EQ(execution_order.size(), 3);
    EXPECT_EQ(execution_order[0], 3);
    EXPECT_EQ(execution_order[1], 2);
    EXPECT_EQ(execution_order[2], 1);
}

TEST_F(ScopeSuccessTest, TypeTraits)
{
    auto guard = abc::make_scope_success([] {});

    // Should not be copy constructible or assignable
    EXPECT_FALSE(std::is_copy_constructible_v<decltype(guard)>);
    EXPECT_FALSE(std::is_copy_assignable_v<decltype(guard)>);
    EXPECT_FALSE(std::is_move_assignable_v<decltype(guard)>);

    // Should be move constructible
    EXPECT_TRUE(std::is_move_constructible_v<decltype(guard)>);
}

// =============================================================================
// Combined Tests
// =============================================================================

TEST(ScopeGuardsIntegrationTest, AllThreeTypesInFunction)
{
    bool exit_executed = false;
    bool fail_executed = false;
    bool success_executed = false;

    auto test_function = [&](bool should_throw) {
        auto exit_guard = abc::make_scope_exit([&exit_executed] { exit_executed = true; });

        auto fail_guard = abc::make_scope_fail([&fail_executed] { fail_executed = true; });

        auto success_guard = abc::make_scope_success([&success_executed] { success_executed = true; });

        if (should_throw)
        {
            throw_exception();
        }
    };

    // Test normal execution
    try
    {
        test_function(false);
    }
    catch (...)
    {
        FAIL() << "Unexpected exception";
    }

    EXPECT_TRUE(exit_executed);    // Always executes
    EXPECT_FALSE(fail_executed);   // Only on exception
    EXPECT_TRUE(success_executed); // Only on success

    // Reset flags
    exit_executed = false;
    fail_executed = false;
    success_executed = false;

    // Test exception execution
    try
    {
        test_function(true);
    }
    catch (std::runtime_error const &)
    {
        // Expected exception
    }

    EXPECT_TRUE(exit_executed);     // Always executes
    EXPECT_TRUE(fail_executed);     // Only on exception
    EXPECT_FALSE(success_executed); // Only on success
}

TEST(ScopeGuardsIntegrationTest, ComplexResourceManagement)
{
    struct Resource
    {
        bool & acquired;
        bool & released;
        bool & cleaned_on_failure;
        bool & finalized_on_success;

        Resource(bool & acq, bool & rel, bool & fail, bool & succ) : acquired(acq), released(rel), cleaned_on_failure(fail), finalized_on_success(succ)
        {
            acquired = true;
        }
    };

    bool acquired = false;
    bool released = false;
    bool cleaned_on_failure = false;
    bool finalized_on_success = false;

    auto use_resource = [&](bool simulate_error) {
        Resource res(acquired, released, cleaned_on_failure, finalized_on_success);

        auto cleanup = abc::make_scope_exit([&] { released = true; });

        auto failure_handler = abc::make_scope_fail([&] { cleaned_on_failure = true; });

        auto success_handler = abc::make_scope_success([&] { finalized_on_success = true; });

        if (simulate_error)
        {
            throw std::runtime_error("Simulated error");
        }
    };

    // Test successful resource usage
    try
    {
        use_resource(false);
    }
    catch (...)
    {
        FAIL() << "Unexpected exception";
    }

    EXPECT_TRUE(acquired);
    EXPECT_TRUE(released);
    EXPECT_FALSE(cleaned_on_failure);
    EXPECT_TRUE(finalized_on_success);

    // Reset for error case
    acquired = false;
    released = false;
    cleaned_on_failure = false;
    finalized_on_success = false;

    // Test resource usage with error
    try
    {
        use_resource(true);
    }
    catch (std::runtime_error const &)
    {
        // Expected
    }

    EXPECT_TRUE(acquired);
    EXPECT_TRUE(released);
    EXPECT_TRUE(cleaned_on_failure);
    EXPECT_FALSE(finalized_on_success);
}
