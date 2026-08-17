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
    auto& instance1 = siddiqsoft::ScopeTrace::GetInstance("test_ScopeTrace", siddiqsoft::LogLevel::debug);
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
        auto scope = g_scope.sub_scope(__func__, siddiqsoft::trace_level::info);

        scope.log<siddiqsoft::trace_level::info>("Hello, World!");
        scope.trace("Hello, World! (trace)"); // should not be logged due to log level filtering
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        scope.err("Hello, World! (error)");
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
                auto innermost = inner.sub_scope("Innermost", siddiqsoft::trace_level::debug);
                innermost.trace("Innermost scope trace message");
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

                inner.info("From the inner scope line: {}", __LINE__);
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

TEST(ScopeTraceTest, ExtractHelpers)
{
    // Test extract_file_name with Unix and Windows paths
    EXPECT_EQ("ScopeTrace.hpp",
              siddiqsoft::ScopeTrace::extract_file_name("/Users/test/repos/ScopeTrace/include/siddiqsoft/ScopeTrace.hpp"));
    EXPECT_EQ("main.cpp", siddiqsoft::ScopeTrace::extract_file_name("C:\\Projects\\App\\main.cpp"));
    EXPECT_EQ("test.h", siddiqsoft::ScopeTrace::extract_file_name("test.h"));
    EXPECT_EQ("", siddiqsoft::ScopeTrace::extract_file_name(""));

    // Test extract_func_name with complex function signatures
    EXPECT_EQ("foo", siddiqsoft::ScopeTrace::extract_func_name("void namespace_name::class_name::foo(int, double)"));
    EXPECT_EQ("bar", siddiqsoft::ScopeTrace::extract_func_name("auto bar()"));
}

TEST(ScopeTraceTest, DynamicSetLevel)
{
    std::stringstream buffer;
    auto*             old_cerr = std::cerr.rdbuf(buffer.rdbuf());

    {
        auto scope = g_scope.sub_scope("SetLevelTest", siddiqsoft::trace_level::warning);
        scope.info("BEFORE_SET_LEVEL"); // Should be filtered out

        scope.set_level(siddiqsoft::trace_level::info);
        scope.info("AFTER_SET_LEVEL"); // Should be logged
    }

    std::cerr.rdbuf(old_cerr);
    std::string output = buffer.str();

    EXPECT_FALSE(output.contains("BEFORE_SET_LEVEL"));
    EXPECT_TRUE(output.contains("AFTER_SET_LEVEL"));
}

TEST(ScopeTraceTest, ExplicitNameGetInstance)
{
    auto& instance = siddiqsoft::ScopeTrace::GetInstance("ExplicitAppScope", siddiqsoft::trace_level::debug);
    EXPECT_EQ("ExplicitAppScope", instance.name());

    auto sub = instance.sub_scope("sub_module");
    EXPECT_EQ("ExplicitAppScope/sub_module", sub.name());
}

TEST(ScopeTraceTest, FormattingAndTagOutput)
{
    std::stringstream buffer;
    auto*             old_cerr = std::cerr.rdbuf(buffer.rdbuf());

    {
        auto scope = g_scope.sub_scope("TagFormatTest", siddiqsoft::trace_level::trace);
        scope.log<siddiqsoft::trace_level::critical>("Crit message");
        scope.warn("Warn message");
        scope.info("Info message");
    }

    std::cerr.rdbuf(old_cerr);
    std::string output = buffer.str();

    // Verify presence of abbreviated tag labels in square brackets
    EXPECT_TRUE(output.contains("  crit"));
    EXPECT_TRUE(output.contains("  warn"));
    EXPECT_TRUE(output.contains("  info"));

    // Verify presence of reverse video ANSI escape codes for critical (\033[7;31m) and warn (\033[7;38;5;220m)
    EXPECT_TRUE(output.contains("\033[7;31m"));
    EXPECT_TRUE(output.contains("\033[7;38;5;220m"));
}

TEST(ScopeTraceTest, ConcurrentThreadSafety)
{
    constexpr int                  num_threads           = 8;
    constexpr int                  iterations_per_thread = 50;

    std::vector<std::future<void>> futures;
    futures.reserve(num_threads);

    for (int t = 0; t < num_threads; ++t) {
        futures.push_back(std::async(std::launch::async, [t]() {
            for (int i = 0; i < iterations_per_thread; ++i) {
                auto thread_scope = g_scope.sub_scope(std::format("Thread_{}", t));
                thread_scope.info("Thread {} iteration {}", t, i);
                auto inner = thread_scope.sub_scope("InnerLoop");
                inner.trace("Trace iteration {}", i);
            }
        }));
    }

    for (auto& f : futures) {
        f.get();
    }
}
