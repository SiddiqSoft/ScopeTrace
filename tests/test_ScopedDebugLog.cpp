#include "gtest/gtest.h"

#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../include/siddiqsoft/ScopedDebugLog.hpp"

TEST(ScopedDebugLogTest, BasicCallbackExecution)
{
    bool callbackInvoked = false;
    std::string capturedFunc {};
    std::string capturedMsg {};

    {
        siddiqsoft::ScopedDebugLog scope([&](const siddiqsoft::ScopedDebugLog& log) {
            callbackInvoked = true;
            capturedFunc = log.location().function_name();
            capturedMsg = log.message();
        }, "TestScope");

        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }

    EXPECT_TRUE(callbackInvoked);
    EXPECT_EQ("TestScope", capturedMsg);
    EXPECT_FALSE(capturedFunc.empty());
}

TEST(ScopedDebugLogTest, ScopeDepthNesting)
{
    std::vector<size_t> depths;

    {
        siddiqsoft::ScopedDebugLog outer([&](const siddiqsoft::ScopedDebugLog& log) {
            depths.push_back(log.depth());
        }, "Outer");

        {
            siddiqsoft::ScopedDebugLog inner([&](const siddiqsoft::ScopedDebugLog& log) {
                depths.push_back(log.depth());
            }, "Inner");

            EXPECT_EQ(1, inner.depth());
        }

        EXPECT_EQ(0, outer.depth());
    }

    ASSERT_EQ(2, depths.size());
    EXPECT_EQ(1, depths[0]); // Inner destroyed first
    EXPECT_EQ(0, depths[1]); // Outer destroyed second
}

TEST(ScopedDebugLogTest, GlobalCallback)
{
    int globalCount = 0;
    std::string lastMessage {};

    siddiqsoft::ScopedDebugLog::set_global_callback([&](const siddiqsoft::ScopedDebugLog& log) {
        globalCount++;
        lastMessage = log.message();
    });

    {
        siddiqsoft::ScopedDebugLog scope1("GlobalScope1");
    }

    {
        siddiqsoft::ScopedDebugLog scope2("GlobalScope2");
    }

    EXPECT_EQ(2, globalCount);
    EXPECT_EQ("GlobalScope2", lastMessage);

    siddiqsoft::ScopedDebugLog::reset_global_callback();

    {
        siddiqsoft::ScopedDebugLog scope3("GlobalScope3");
    }

    EXPECT_EQ(2, globalCount); // Unchanged after reset
}

TEST(ScopedDebugLogTest, FormattingAndStream)
{
    siddiqsoft::ScopedDebugLog scope([&](const siddiqsoft::ScopedDebugLog& log) {
        std::string str = log.to_string();
        EXPECT_TRUE(str.contains("StreamScope"));
        EXPECT_TRUE(str.contains("took"));

        std::ostringstream os;
        os << log;
        EXPECT_TRUE(os.str().contains("StreamScope"));
        EXPECT_TRUE(os.str().contains("took"));

        std::string formatted = std::format("{}", log);
        EXPECT_TRUE(formatted.contains("StreamScope"));
        EXPECT_TRUE(formatted.contains("took"));
    }, "StreamScope");
}

TEST(ScopedDebugLogTest, ExceptionSafety)
{
    bool ranCallback = false;

    try
    {
        siddiqsoft::ScopedDebugLog scope([&](const siddiqsoft::ScopedDebugLog&) {
            ranCallback = true;
        }, "ExceptionScope");

        throw std::runtime_error("Deliberate error");
    }
    catch (const std::exception&)
    {
        // Caught
    }

    EXPECT_TRUE(ranCallback);
}
