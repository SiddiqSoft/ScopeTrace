#include "gtest/gtest.h"

#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../include/siddiqsoft/ScopeTrace.hpp"

TEST(ScopeTraceTest, BasicCallbackExecution)
{
    {
        siddiqsoft::ScopeTrace scope(__func__);

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
}

TEST(ScopeTraceTest, ScopeDepthNesting)
{
    std::vector<size_t> depths;

    {
        siddiqsoft::ScopeTrace outer("Outer");

        {
            siddiqsoft::ScopeTrace inner("Inner");

            inner.warn("Inner scope warning message");
            EXPECT_EQ(1, inner.depth());
            inner.err("Inner scope error message");
        }
        outer.msg("Outer scope message");
        EXPECT_EQ(0, outer.depth());
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
    siddiqsoft::ScopeTrace scope;

    try {
        auto inner = scope.nest("Nested");

        inner.msg("From the inner scope line: {}", __LINE__);
        throw std::runtime_error(std::format("Deliberate error from line: {}", __LINE__));
    }
    catch (const std::exception& e) {
        scope.exp(e);
    }
}
