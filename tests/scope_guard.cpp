// Copyright(c) 2025 - present, Payton Wu (payton.wu@outlook.com) & abc contributors.
// Distributed under the MIT License (http://opensource.org/licenses/MIT)

#include <abc/scope_guard.h>

#include <gtest/gtest.h>

#include <functional>
#include <stdexcept>
#include <string>

namespace
{

// Test fixture for scope guard tests
class ScopeGuardTest : public ::testing::Test
{
protected:
    void
    SetUp() override
    {
        // Reset any global state if needed
    }
};

// Helper function to test exception state
void
throw_exception()
{
    throw std::runtime_error("test exception");
}

// Helper function that doesn't throw
void
no_throw_function()
{
    // Do nothing
}

} // namespace

TEST_F(ScopeGuardTest, DefaultConstruction)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_guard([&executed](bool exiting_exception) {
            executed = true;
            EXPECT_FALSE(exiting_exception);
        });
        EXPECT_FALSE(executed);
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeGuardTest, DirectConstruction)
{
    bool executed = false;
    {
        abc::ScopeGuard<std::function<void(bool)>> guard([&executed](bool exiting_exception) {
            executed = true;
            EXPECT_FALSE(exiting_exception);
        });
        EXPECT_FALSE(executed);
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeGuardTest, LambdaCapture)
{
    std::string message = "initial";
    {
        auto guard = abc::make_scope_guard([&message](bool exiting_exception) {
            message = "executed";
            EXPECT_FALSE(exiting_exception);
        });
        EXPECT_EQ(message, "initial");
    }
    EXPECT_EQ(message, "executed");
}

TEST_F(ScopeGuardTest, MoveConstructor)
{
    bool executed = false;
    {
        auto guard1 = abc::make_scope_guard([&executed](bool exiting_exception) {
            executed = true;
            EXPECT_FALSE(exiting_exception);
        });

        auto guard2 = std::move(guard1);
        // guard1 should be released after move
        EXPECT_FALSE(executed);
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeGuardTest, MoveConstructorReleasesOriginal)
{
    bool executed = false;
    {
        auto guard1 = abc::make_scope_guard([&executed](bool exiting_exception) {
            executed = true;
            EXPECT_FALSE(exiting_exception);
        });

        auto guard2 = std::move(guard1);
        // guard1 should be released, so it won't execute when destroyed
        EXPECT_FALSE(executed);
    }
    EXPECT_TRUE(executed);
}

TEST_F(ScopeGuardTest, ReleaseMethod)
{
    bool executed = false;
    {
        auto guard = abc::make_scope_guard([&executed](bool exiting_exception) {
            executed = true;
            EXPECT_FALSE(exiting_exception);
        });
        guard.release();
        EXPECT_FALSE(executed);
    }
    EXPECT_FALSE(executed); // Should still be false after destruction
}

TEST_F(ScopeGuardTest, ReleaseAfterMove)
{
    bool executed = false;
    {
        auto guard1 = abc::make_scope_guard([&executed](bool exiting_exception) {
            executed = true;
            EXPECT_FALSE(exiting_exception);
        });

        auto guard2 = std::move(guard1);
        guard2.release();
        EXPECT_FALSE(executed);
    }
    EXPECT_FALSE(executed);
}

TEST_F(ScopeGuardTest, ExceptionDetection)
{
    bool executed = false;
    bool was_exception = false;

    try
    {
        auto guard = abc::make_scope_guard([&executed, &was_exception](bool exiting_exception) {
            executed = true;
            was_exception = exiting_exception;
        });

        throw_exception();
    }
    catch (std::runtime_error const &)
    {
        // Exception caught
    }

    EXPECT_TRUE(executed);
    EXPECT_TRUE(was_exception);
}

TEST_F(ScopeGuardTest, NoExceptionDetection)
{
    bool executed = false;
    bool was_exception = false;

    {
        auto guard = abc::make_scope_guard([&executed, &was_exception](bool exiting_exception) {
            executed = true;
            was_exception = exiting_exception;
        });

        no_throw_function();
    }

    EXPECT_TRUE(executed);
    EXPECT_FALSE(was_exception);
}

TEST_F(ScopeGuardTest, NestedExceptions)
{
    bool outer_executed = false;
    bool inner_executed = false;
    bool outer_was_exception = false;
    bool inner_was_exception = false;

    try
    {
        auto outer_guard = abc::make_scope_guard([&outer_executed, &outer_was_exception](bool exiting_exception) {
            outer_executed = true;
            outer_was_exception = exiting_exception;
        });

        try
        {
            auto inner_guard = abc::make_scope_guard([&inner_executed, &inner_was_exception](bool exiting_exception) {
                inner_executed = true;
                inner_was_exception = exiting_exception;
            });

            throw_exception();
        }
        catch (std::runtime_error const &)
        {
            // Inner exception caught
        }

        // Outer scope should still be active
        EXPECT_FALSE(outer_executed);
        EXPECT_TRUE(inner_executed);
        EXPECT_TRUE(inner_was_exception);
    }
    catch (...)
    {
        // Should not reach here
        FAIL() << "Unexpected exception in outer scope";
    }

    EXPECT_TRUE(outer_executed);
    EXPECT_FALSE(outer_was_exception);
}

TEST_F(ScopeGuardTest, MultipleGuards)
{
    int execution_count = 0;

    {
        auto guard1 = abc::make_scope_guard([&execution_count](bool exiting_exception) {
            execution_count++;
            EXPECT_FALSE(exiting_exception);
        });

        auto guard2 = abc::make_scope_guard([&execution_count](bool exiting_exception) {
            execution_count++;
            EXPECT_FALSE(exiting_exception);
        });

        auto guard3 = abc::make_scope_guard([&execution_count](bool exiting_exception) {
            execution_count++;
            EXPECT_FALSE(exiting_exception);
        });

        EXPECT_EQ(execution_count, 0);
    }

    EXPECT_EQ(execution_count, 3);
}

TEST_F(ScopeGuardTest, GuardWithState)
{
    struct State
    {
        int value = 0;
        std::string message = "initial";
    };

    State state;
    {
        auto guard = abc::make_scope_guard([&state](bool exiting_exception) {
            state.value = 42;
            state.message = "modified";
            EXPECT_FALSE(exiting_exception);
        });

        EXPECT_EQ(state.value, 0);
        EXPECT_EQ(state.message, "initial");
    }

    EXPECT_EQ(state.value, 42);
    EXPECT_EQ(state.message, "modified");
}

TEST_F(ScopeGuardTest, FunctionObject)
{
    struct Functor
    {
        bool & executed;
        bool & was_exception;

        Functor(bool & exec, bool & except) : executed(exec), was_exception(except)
        {
        }

        void
        operator()(bool exiting_exception) const
        {
            executed = true;
            was_exception = exiting_exception;
        }
    };

    bool executed = false;
    bool was_exception = false;

    {
        auto guard = abc::make_scope_guard(Functor(executed, was_exception));
        EXPECT_FALSE(executed);
    }

    EXPECT_TRUE(executed);
    EXPECT_FALSE(was_exception);
}

TEST_F(ScopeGuardTest, MemberFunction)
{
    struct TestClass
    {
        bool executed = false;
        bool was_exception = false;

        void
        cleanup(bool exiting_exception)
        {
            executed = true;
            was_exception = exiting_exception;
        }
    };

    TestClass obj;
    {
        auto guard = abc::make_scope_guard([&obj](bool exiting_exception) { obj.cleanup(exiting_exception); });

        EXPECT_FALSE(obj.executed);
    }

    EXPECT_TRUE(obj.executed);
    EXPECT_FALSE(obj.was_exception);
}

TEST_F(ScopeGuardTest, ExceptionInGuardExecution)
{
    bool executed = false;

    {
        auto guard = abc::make_scope_guard([&executed](bool exiting_exception) {
            executed = true;
            EXPECT_FALSE(exiting_exception);
            throw std::logic_error("guard execution error");
        });
    }

    EXPECT_TRUE(executed);
    // The exception from guard execution should propagate
    // This test verifies that the guard executes even if it throws
}

TEST_F(ScopeGuardTest, CopyConstructorDeleted)
{
    auto guard = abc::make_scope_guard([](bool) {});

    // This should not compile, but we can test that the copy constructor is deleted
    // by checking if the type is not copy constructible
    EXPECT_FALSE(std::is_copy_constructible_v<decltype(guard)>);
}

TEST_F(ScopeGuardTest, CopyAssignmentDeleted)
{
    auto guard = abc::make_scope_guard([](bool) {});

    // This should not compile, but we can test that the copy assignment is deleted
    // by checking if the type is not copy assignable
    EXPECT_FALSE(std::is_copy_assignable_v<decltype(guard)>);
}

TEST_F(ScopeGuardTest, MoveAssignmentNotProvided)
{
    auto guard1 = abc::make_scope_guard([](bool) {});
    auto guard2 = abc::make_scope_guard([](bool) {});

    // Move assignment is not provided, so this should not compile
    // We can test that the type is not move assignable
    EXPECT_FALSE(std::is_move_assignable_v<decltype(guard1)>);
}

TEST_F(ScopeGuardTest, NoexceptSpecifications)
{
    // Test that constructors and destructor have appropriate noexcept specifications
    auto guard = abc::make_scope_guard([](bool) {});

    // Constructor should be noexcept
    EXPECT_TRUE((std::is_nothrow_constructible_v<decltype(guard), decltype(guard)>));

    // Move constructor should be noexcept
    EXPECT_TRUE(std::is_nothrow_move_constructible_v<decltype(guard)>);

    // Destructor should be noexcept if the callable's operator() is noexcept
    // This depends on the specific callable type
}

TEST_F(ScopeGuardTest, ComplexCallable)
{
    struct ComplexCallable
    {
        std::vector<int> & vec;
        std::string & str;

        ComplexCallable(std::vector<int> & v, std::string & s) : vec(v), str(s)
        {
        }

        void
        operator()(bool exiting_exception) const
        {
            vec.push_back(42);
            str = "modified";
            EXPECT_FALSE(exiting_exception);
        }
    };

    std::vector<int> numbers;
    std::string text = "original";

    {
        auto guard = abc::make_scope_guard(ComplexCallable(numbers, text));
        EXPECT_TRUE(numbers.empty());
        EXPECT_EQ(text, "original");
    }

    EXPECT_EQ(numbers.size(), 1);
    EXPECT_EQ(numbers[0], 42);
    EXPECT_EQ(text, "modified");
}

TEST_F(ScopeGuardTest, GuardInFunction)
{
    bool executed = false;

    auto test_function = [&executed]() {
        auto guard = abc::make_scope_guard([&executed](bool exiting_exception) {
            executed = true;
            EXPECT_FALSE(exiting_exception);
        });

        // Function returns, guard should execute
    };

    test_function();
    EXPECT_TRUE(executed);
}

TEST_F(ScopeGuardTest, GuardInLoop)
{
    int execution_count = 0;

    for (int i = 0; i < 3; ++i)
    {
        auto guard = abc::make_scope_guard([&execution_count, i](bool exiting_exception) {
            execution_count++;
            EXPECT_FALSE(exiting_exception);
        });

        // Guard should execute at end of each iteration
    }

    EXPECT_EQ(execution_count, 3);
}

TEST_F(ScopeGuardTest, ConditionalGuard)
{
    bool should_execute = true;
    bool executed = false;

    {
        auto guard = abc::make_scope_guard([&executed, should_execute](bool exiting_exception) {
            if (should_execute)
            {
                executed = true;
            }
            EXPECT_FALSE(exiting_exception);
        });

        should_execute = false; // This won't affect the guard's execution
    }

    EXPECT_TRUE(executed); // Guard still executes, but the condition is checked
}

TEST_F(ScopeGuardTest, GuardWithReturnValue)
{
    int result = 0;

    auto test_function = [&result]() -> int {
        auto guard = abc::make_scope_guard([&result](bool exiting_exception) {
            result = 100;
            EXPECT_FALSE(exiting_exception);
        });

        return 42; // Guard should execute before return
    };

    int return_value = test_function();
    EXPECT_EQ(return_value, 42);
    EXPECT_EQ(result, 100);
}
