#include "gtest/gtest.h"

#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <future>

#define SCOPETRACE_PRIVATE_TESTING 1

#include "../include/siddiqsoft/ScopeTrace.hpp"

auto& g_scope = siddiqsoft::ScopeTrace::GetInstance("test_ScopeTrace", siddiqsoft::trace_level::debug);


TEST(ScopeTraceTest, GetInstanceSingleton)
{
    auto& instance1 = siddiqsoft::ScopeTrace::GetInstance("SingletonApp", siddiqsoft::LogLevel::debug);
    auto& instance2 = siddiqsoft::ScopeTrace::GetInstance();

    // Must refer to the exact same process singleton instance
    EXPECT_EQ(&instance1, &instance2);
    EXPECT_EQ(0, instance1.depth());

    auto child = instance1.sub_scope("worker");
    EXPECT_EQ(1, child.depth());
}

TEST(ScopeTraceTest, HelloWorld)
{
    {
        auto scope = g_scope.sub_scope(__func__, siddiqsoft::trace_level::trace);

        scope.log<siddiqsoft::trace_level::info>("Hello, World!");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

TEST(ScopeTraceTest, HelloWorld_Lambda)
{
    std::future<void> f = std::async(std::launch::async, []() {
        auto scope = g_scope.sub_scope("HelloWorld_Lambda");

        scope.trace("Hello, World!");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    });

    f.get();
}

TEST(ScopeTraceTest, ParentageDepthNesting)
{
    // g_scope is depth 0.
    // Multiple sibling sub_scope() calls directly on g_scope must all produce depth = 1
    auto childA = g_scope.sub_scope("childA");
    auto childB = g_scope.sub_scope("childB");
    auto childC = g_scope.sub_scope("childC");

    EXPECT_EQ(1, childA.depth());
    EXPECT_EQ(1, childB.depth());
    EXPECT_EQ(1, childC.depth());

    // Nesting from childA (depth 1) must produce depth 2
    auto grandChildA = childA.sub_scope("grandChildA");
    EXPECT_EQ(2, grandChildA.depth());

    // Sibling nesting from childA must also produce depth 2
    auto grandChildA2 = childA.sub_scope("grandChildA2");
    EXPECT_EQ(2, grandChildA2.depth());
}

TEST(ScopeTraceTest, ScopeDepthNesting)
{
    std::vector<size_t> depths;

    {
        auto outer = g_scope.sub_scope("Outer");

        {
            auto inner = outer.sub_scope("Inner");
            inner.trace("Inner scope message");

            inner.warn("Inner scope warning message");
            EXPECT_EQ(2, inner.depth());
            inner.err("Inner scope error message");
            {
                // Use this for anonymous or within an exception scope.
                auto innermost = inner.sub_scope("Innermost");
                innermost.trace("Innermost scope message");
                EXPECT_EQ(3, innermost.depth());
            }
        }
        outer.info("Outer scope message");
        EXPECT_EQ(1, outer.depth());
    }
}


TEST(ScopeTraceTest, ExceptionSafety)
{
    auto scope = g_scope.sub_scope(__func__);

    try {
        auto inner = scope.sub_scope("Nested", siddiqsoft::trace_level::warning);

        inner.info("From the inner scope line: {} -- FILTERED", __LINE__);
        inner.log<siddiqsoft::trace_level::warning>("From the inner scope line: {}", __LINE__);
        inner.err_throw<std::runtime_error>("Deliberate error from here");
    }
    catch (const std::exception& e) {
        scope.exp(e, "Caught exception in outer scope at line: {}", __LINE__);
    }
}


TEST(ScopeTraceTest, ExceptionSafety_2)
{
    auto scope = g_scope.sub_scope("ExceptionSafety_2", siddiqsoft::trace_level::info);

    EXPECT_THROW(
            {
                auto inner = scope.sub_scope("inner");

                inner.log<siddiqsoft::trace_level::info>("From the inner scope line: {}", __LINE__);
                inner.err_throw<std::invalid_argument>("Deliberate error from here");
            },
            std::invalid_argument);
}


TEST(ScopeTraceTest, LogLevelFiltering)
{
    auto test_filter = [](siddiqsoft::trace_level config_level) {
        std::stringstream buffer;
        auto*             old_cerr = std::cerr.rdbuf(buffer.rdbuf());

        {
            auto scope = siddiqsoft::ScopeTrace::GetInstance().sub_scope("FilterTest", config_level);
            scope.log<siddiqsoft::trace_level::critical>("CRIT_MSG");
            scope.log<siddiqsoft::trace_level::error>("ERR_MSG");
            scope.warn("WARN_MSG");
            scope.info("INFO_MSG");
            scope.debug("DEBG_MSG");
            scope.trace("TRCE_MSG");
        }

        std::cerr.rdbuf(old_cerr);
        return buffer.str();
    };

    // 1. LogLevel::warning -> logs critical, error, warning; excludes info, debug, trace
    std::string warn_output = test_filter(siddiqsoft::trace_level::warning);
    EXPECT_TRUE(warn_output.contains("CRIT_MSG"));
    EXPECT_TRUE(warn_output.contains("ERR_MSG"));
    EXPECT_TRUE(warn_output.contains("WARN_MSG"));
    EXPECT_FALSE(warn_output.contains("INFO_MSG"));
    EXPECT_FALSE(warn_output.contains("DEBG_MSG"));
    EXPECT_FALSE(warn_output.contains("TRCE_MSG"));

    // 2. LogLevel::info -> logs critical, error, warning, info; excludes debug, trace
    std::string info_output = test_filter(siddiqsoft::trace_level::info);
    EXPECT_TRUE(info_output.contains("CRIT_MSG"));
    EXPECT_TRUE(info_output.contains("ERR_MSG"));
    EXPECT_TRUE(info_output.contains("WARN_MSG"));
    EXPECT_TRUE(info_output.contains("INFO_MSG"));
    EXPECT_FALSE(info_output.contains("DEBG_MSG"));
    EXPECT_FALSE(info_output.contains("TRCE_MSG"));

    // 3. LogLevel::debug -> logs critical, error, warning, info, debug; excludes trace
    std::string debug_output = test_filter(siddiqsoft::trace_level::debug);
    EXPECT_TRUE(debug_output.contains("CRIT_MSG"));
    EXPECT_TRUE(debug_output.contains("ERR_MSG"));
    EXPECT_TRUE(debug_output.contains("WARN_MSG"));
    EXPECT_TRUE(debug_output.contains("INFO_MSG"));
    EXPECT_TRUE(debug_output.contains("DEBG_MSG"));
    EXPECT_FALSE(debug_output.contains("TRCE_MSG"));

    // 4. LogLevel::trace -> logs everything including trace
    std::string trace_output = test_filter(siddiqsoft::trace_level::trace);
    EXPECT_TRUE(trace_output.contains("CRIT_MSG"));
    EXPECT_TRUE(trace_output.contains("ERR_MSG"));
    EXPECT_TRUE(trace_output.contains("WARN_MSG"));
    EXPECT_TRUE(trace_output.contains("INFO_MSG"));
    EXPECT_TRUE(trace_output.contains("DEBG_MSG"));
    EXPECT_TRUE(trace_output.contains("TRCE_MSG"));
}
