/*
    ScopedDebugLog : Modern C++23 RAII Scope Logger Library
    Version 1.0.0

    https://github.com/SiddiqSoft/ScopedDebugLog

    BSD 3-Clause License

    Copyright (c) 2026, Siddiq Software LLC
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
       contributors may be used to endorse or promote products derived from
       this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once
#ifndef SIDDIQSOFT_SCOPEDDEBUGLOG_HPP
#define SIDDIQSOFT_SCOPEDDEBUGLOG_HPP 1

#include <chrono>
#include <format>
#include <functional>
#include <mutex>
#include <ostream>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace siddiqsoft
{
    /// @brief Modern RAII scope logger measuring elapsed duration, location, and nesting level.
    struct ScopedDebugLog
    {
        /// @brief Access thread-local scope nesting level counter
        /// @return Reference to current thread scope depth
        static size_t& current_depth() noexcept
        {
            thread_local size_t depth = 0;
            return depth;
        }

        /// @brief Set a thread-safe global callback invoked when any ScopedDebugLog exits (unless overridden)
        /// @param callback Function accepting a const reference to ScopedDebugLog
        static void set_global_callback(std::function<void(const ScopedDebugLog&)> callback)
        {
            std::lock_guard<std::mutex> lock(global_mutex());
            global_callback_storage() = std::move(callback);
        }

        /// @brief Reset the global callback
        static void reset_global_callback()
        {
            std::lock_guard<std::mutex> lock(global_mutex());
            global_callback_storage() = nullptr;
        }

        /// @brief Calculate elapsed time since scope entry
        /// @return Duration elapsed
        [[nodiscard]] auto elapsed() const noexcept
        {
            return std::chrono::system_clock::now() - startTimestamp;
        }

        /// @brief Get the nesting depth of this scope log
        /// @return Depth level (0 for top-level scope)
        [[nodiscard]] size_t depth() const noexcept
        {
            return scopeDepth;
        }

        /// @brief Get source location of this scope log
        /// @return std::source_location instance
        [[nodiscard]] const std::source_location& location() const noexcept
        {
            return sourceLocation;
        }

        /// @brief Get optional user message attached to scope log
        /// @return Message string
        [[nodiscard]] std::string_view message() const noexcept
        {
            return scopeMessage;
        }

        /// @brief Construct a ScopedDebugLog with optional message and location
        /// @param msg Custom scope label or context message
        /// @param sl Source location (defaults to caller site)
        explicit ScopedDebugLog(std::string_view msg = "",
                                const std::source_location& sl = std::source_location::current())
            : sourceLocation(sl),
              startTimestamp(std::chrono::system_clock::now()),
              scopeMessage(msg),
              scopeDepth(current_depth()++)
        {
        }

        /// @brief Construct a ScopedDebugLog with custom callback, optional message, and location
        /// @param callback Callback executed on destruction receiving this ScopedDebugLog
        /// @param msg Custom scope label or context message
        /// @param sl Source location (defaults to caller site)
        explicit ScopedDebugLog(std::function<void(const ScopedDebugLog&)>&& callback,
                                std::string_view msg = "",
                                const std::source_location& sl = std::source_location::current())
            : mCallback(std::move(callback)),
              sourceLocation(sl),
              startTimestamp(std::chrono::system_clock::now()),
              scopeMessage(msg),
              scopeDepth(current_depth()++)
        {
        }

        /// @brief Copying is not supported
        ScopedDebugLog(const ScopedDebugLog&) = delete;

        /// @brief Moving is not supported
        ScopedDebugLog(ScopedDebugLog&&) = delete;

        /// @brief Copy assignment is not supported
        ScopedDebugLog& operator=(const ScopedDebugLog&) = delete;

        /// @brief Move assignment is not supported
        ScopedDebugLog& operator=(ScopedDebugLog&&) = delete;

        /// @brief Destructor triggers callbacks and updates depth counter
        ~ScopedDebugLog() noexcept
        {
            if (current_depth() > 0)
            {
                --current_depth();
            }

            try
            {
                if (mCallback)
                {
                    mCallback(*this);
                }
                else
                {
                    std::lock_guard<std::mutex> lock(global_mutex());
                    auto& gcb = global_callback_storage();
                    if (gcb)
                    {
                        gcb(*this);
                    }
                }
            }
            catch (...)
            {
                // Swallow exceptions to preserve noexcept warranty
            }
        }

        /// @brief Format scope log as string representation
        /// @tparam charT Character type (default char)
        /// @return Formatted scope log description
        template <typename charT = char>
        [[nodiscard]] auto to_string() const
        {
            if constexpr (std::is_same_v<charT, char>)
            {
                auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed()).count();
                std::string indent(scopeDepth * 2, ' ');
                if (scopeMessage.empty())
                {
                    return std::format("{}[{}:{}] {} took {}us",
                                       indent,
                                       sourceLocation.file_name(),
                                       sourceLocation.line(),
                                       sourceLocation.function_name(),
                                       us);
                }
                else
                {
                    return std::format("{}[{}:{}] {} ({}) took {}us",
                                       indent,
                                       sourceLocation.file_name(),
                                       sourceLocation.line(),
                                       sourceLocation.function_name(),
                                       scopeMessage,
                                       us);
                }
            }
            else
            {
                static_assert(std::is_same_v<charT, char>, "ScopedDebugLog supports char formatting");
            }
        }

        /// @brief Stream insertion operator for ScopedDebugLog
        friend std::ostream& operator<<(std::ostream& os, const ScopedDebugLog& src)
        {
            os << src.to_string<char>();
            return os;
        }

    private:
        static std::mutex& global_mutex()
        {
            static std::mutex mtx;
            return mtx;
        }

        static std::function<void(const ScopedDebugLog&)>& global_callback_storage()
        {
            static std::function<void(const ScopedDebugLog&)> callback;
            return callback;
        }

        std::function<void(const ScopedDebugLog&)> mCallback {};
        std::source_location sourceLocation;
        std::chrono::system_clock::time_point startTimestamp;
        std::string scopeMessage {};
        size_t scopeDepth {0};
    };

    /// @brief Aliases for backward compatibility
    using scopelog = ScopedDebugLog;
    using ScopeLog = ScopedDebugLog;
} // namespace siddiqsoft

template <class charT>
struct std::formatter<siddiqsoft::ScopedDebugLog, charT> : std::formatter<basic_string_view<charT>, charT>
{
    template <class FC>
    auto format(const siddiqsoft::ScopedDebugLog& sl, FC& ctx) const
    {
        return std::formatter<basic_string_view<charT>, charT>::format(sl.to_string<charT>(), ctx);
    }
};

#endif // SIDDIQSOFT_SCOPEDDEBUGLOG_HPP
