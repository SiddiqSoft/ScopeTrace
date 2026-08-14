#include "gtest/gtest.h"

#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../include/siddiqsoft/ScopeTrace.hpp"

static siddiqsoft::ScopeTrace g_scope;


TEST(ScopeTraceTest, HelloWorld)
{
    {
        auto scope = g_scope.nest(__func__);

        scope.trace("Hello, World!");
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

TEST(ScopeTraceTest, ScopeDepthNesting)
{
    std::vector<size_t> depths;

    {
        auto outer = g_scope.nest("Outer");

        {
            auto inner = outer.nest("Inner");

            inner.warn("Inner scope warning message");
            EXPECT_EQ(2, inner.depth());
            inner.err("Inner scope error message");
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
        throw std::runtime_error(std::format("Deliberate error from line: {}", __LINE__));
    }
    catch (const std::exception& e) {
        scope.exp(e);
    }
}
