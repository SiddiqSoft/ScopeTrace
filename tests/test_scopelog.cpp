#include "gtest/gtest.h"

#include <chrono>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "../include/siddiqsoft/scopelog.hpp"

TEST(ScopeLogTest, BasicCallbackExecution)
{
    bool callbackInvoked = false;
    std::string capturedFunc {};
    std::string capturedMsg {};

    {
        siddiqsoft::scopelog scope([&](const siddiqsoft::scopelog& log) {
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

TEST(ScopeLogTest, ScopeDepthNesting)
{
    std::vector<size_t> depths;

    {
        siddiqsoft::scopelog outer([&](const siddiqsoft::scopelog& log) {
            depths.push_back(log.depth());
        }, "Outer");

        {
            siddiqsoft::scopelog inner([&](const siddiqsoft::scopelog& log) {
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

TEST(ScopeLogTest, GlobalCallback)
{
    int globalCount = 0;
    std::string lastMessage {};

    siddiqsoft::scopelog::set_global_callback([&](const siddiqsoft::scopelog& log) {
        globalCount++;
        lastMessage = log.message();
    });

    {
        siddiqsoft::scopelog scope1("GlobalScope1");
    }

    {
        siddiqsoft::scopelog scope2("GlobalScope2");
    }

    EXPECT_EQ(2, globalCount);
    EXPECT_EQ("GlobalScope2", lastMessage);

    siddiqsoft::scopelog::reset_global_callback();

    {
        siddiqsoft::scopelog scope3("GlobalScope3");
    }

    EXPECT_EQ(2, globalCount); // Unchanged after reset
}

TEST(ScopeLogTest, FormattingAndStream)
{
    siddiqsoft::scopelog scope([&](const siddiqsoft::scopelog& log) {
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

TEST(ScopeLogTest, ExceptionSafety)
{
    bool ranCallback = false;

    try
    {
        siddiqsoft::scopelog scope([&](const siddiqsoft::scopelog&) {
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
