#include "gtest/gtest.h"

#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <future>


#include "../include/siddiqsoft/ScopeTrace.hpp"

siddiqsoft::ScopeTrace g_scope{"test_ScopeTrace", siddiqsoft::LogLevel::debug, std::source_location::current()};


TEST(ScopeTraceTest, HelloWorld)
{
    {
        auto scope = g_scope.nest(__func__, siddiqsoft::LogLevel::trace);

        scope.log<siddiqsoft::LogLevel::info>("Hello, World!");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

TEST(ScopeTraceTest, HelloWorld_Lambda)
{
    std::future<void> f = std::async(std::launch::async, []() {
        auto scope = g_scope.nest("HelloWorld_Lambda");

        scope.trace("Hello, World!");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    });

    f.get();
}


TEST(ScopeTraceTest, ScopeDepthNesting)
{
    std::vector<size_t> depths;

    {
        auto outer = g_scope.nest("Outer");

        {
            auto inner = outer.nest("Inner");
            inner.trace("Inner scope message");

            inner.warn("Inner scope warning message");
            EXPECT_EQ(2, inner.depth());
            inner.err("Inner scope error message");
            {
                // Use this for anonymous or within an exception scope.
                auto innermost = inner.nest("Innermost");
                innermost.trace("Innermost scope message");
                EXPECT_EQ(3, innermost.depth());
            }
        }
        outer.info("Outer scope message");
        EXPECT_EQ(1, outer.depth());
    }
}


TEST(ScopeTraceTest, FormattingAndStream)
{
    siddiqsoft::ScopeTrace scope;
    {
        std::string str = scope.to_string();
        EXPECT_TRUE(str.contains("Test"));
        EXPECT_TRUE(str.contains("took"));

        std::ostringstream os;
        os << scope;
        EXPECT_TRUE(os.str().contains("Test"));
        EXPECT_TRUE(os.str().contains("took"));

        std::string formatted = std::format("{}", scope.to_string());
        EXPECT_TRUE(formatted.contains("Test"));
        EXPECT_TRUE(formatted.contains("took"));
    }
}

TEST(ScopeTraceTest, ExceptionSafety)
{
    auto scope = g_scope.nest(__func__);

    try {
        auto inner = scope.nest("Nested");

        inner.info("From the inner scope line: {}", __LINE__);
        inner.log<siddiqsoft::LogLevel::critical>("From the inner scope line: {}", __LINE__);
        inner.err_throw<std::runtime_error>("Deliberate error from here");
    }
    catch (const std::exception& e) {
        scope.exp(e, "Caught exception in outer scope at line: {}", __LINE__);
    }
}


TEST(ScopeTraceTest, ExceptionSafety_2)
{
    auto scope = g_scope.nest("ExceptionSafety_2", siddiqsoft::LogLevel::info);

    EXPECT_THROW(
            {
                auto inner = scope.nest("inner");

                inner.log<siddiqsoft::LogLevel::info>("From the inner scope line: {}", __LINE__);
                inner.err_throw<std::invalid_argument>("Deliberate error from here");
            },
            std::invalid_argument);
}
