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
        auto inner = scope.nest("Nested", siddiqsoft::LogLevel::warning);

        inner.info("From the inner scope line: {} -- FILTERED", __LINE__);
        inner.log<siddiqsoft::LogLevel::warning>("From the inner scope line: {}", __LINE__);
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


TEST(ScopeTraceTest, LogLevelFiltering)
{
    auto test_filter = [](siddiqsoft::LogLevel config_level) {
        std::stringstream buffer;
        auto*             old_cerr = std::cerr.rdbuf(buffer.rdbuf());

        {
            siddiqsoft::ScopeTrace scope{"FilterTest", config_level};
            scope.log<siddiqsoft::LogLevel::critical>("CRIT_MSG");
            scope.log<siddiqsoft::LogLevel::error>("ERR_MSG");
            scope.warn("WARN_MSG");
            scope.info("INFO_MSG");
            scope.debug("DEBG_MSG");
            scope.trace("TRCE_MSG");
        }

        std::cerr.rdbuf(old_cerr);
        return buffer.str();
    };

    // 1. LogLevel::warning -> logs critical, error, warning; excludes info, debug, trace
    std::string warn_output = test_filter(siddiqsoft::LogLevel::warning);
    EXPECT_TRUE(warn_output.contains("CRIT_MSG"));
    EXPECT_TRUE(warn_output.contains("ERR_MSG"));
    EXPECT_TRUE(warn_output.contains("WARN_MSG"));
    EXPECT_FALSE(warn_output.contains("INFO_MSG"));
    EXPECT_FALSE(warn_output.contains("DEBG_MSG"));
    EXPECT_FALSE(warn_output.contains("TRCE_MSG"));

    // 2. LogLevel::info -> logs critical, error, warning, info; excludes debug, trace
    std::string info_output = test_filter(siddiqsoft::LogLevel::info);
    EXPECT_TRUE(info_output.contains("CRIT_MSG"));
    EXPECT_TRUE(info_output.contains("ERR_MSG"));
    EXPECT_TRUE(info_output.contains("WARN_MSG"));
    EXPECT_TRUE(info_output.contains("INFO_MSG"));
    EXPECT_FALSE(info_output.contains("DEBG_MSG"));
    EXPECT_FALSE(info_output.contains("TRCE_MSG"));

    // 3. LogLevel::debug -> logs critical, error, warning, info, debug; excludes trace
    std::string debug_output = test_filter(siddiqsoft::LogLevel::debug);
    EXPECT_TRUE(debug_output.contains("CRIT_MSG"));
    EXPECT_TRUE(debug_output.contains("ERR_MSG"));
    EXPECT_TRUE(debug_output.contains("WARN_MSG"));
    EXPECT_TRUE(debug_output.contains("INFO_MSG"));
    EXPECT_TRUE(debug_output.contains("DEBG_MSG"));
    EXPECT_FALSE(debug_output.contains("TRCE_MSG"));

    // 4. LogLevel::trace -> logs everything including trace
    std::string trace_output = test_filter(siddiqsoft::LogLevel::trace);
    EXPECT_TRUE(trace_output.contains("CRIT_MSG"));
    EXPECT_TRUE(trace_output.contains("ERR_MSG"));
    EXPECT_TRUE(trace_output.contains("WARN_MSG"));
    EXPECT_TRUE(trace_output.contains("INFO_MSG"));
    EXPECT_TRUE(trace_output.contains("DEBG_MSG"));
    EXPECT_TRUE(trace_output.contains("TRCE_MSG"));
}

