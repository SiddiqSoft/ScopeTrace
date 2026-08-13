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
    class ScopedDebugLog
    {
        // Colors for output
        static constexpr std::string_view RED {"\033[0;31m"};
        static constexpr std::string_view BLU {"\033[0;34m"};
        static constexpr std::string_view GRN {"\033[0;32m"};
        static constexpr std::string_view YLW {"\033[1;33m"};
        static constexpr std::string_view BOLD {"\033[1m"};
        static constexpr std::string_view NOTBOLD {"\033[22m"};
        static constexpr std::string_view NOC {"\033[0m"}; // No Color

    public:
        /// @brief Access thread-local scope nesting level counter
        /// @return Reference to current thread scope depth
        static size_t& current_depth() noexcept
        {
            thread_local size_t depth = 0;
            return depth;
        }

        /// @brief Calculate elapsed time since scope entry
        /// @return Duration elapsed
        [[nodiscard]] auto elapsed() const noexcept { return std::chrono::system_clock::now() - m_start_timestamp; }

        /// @brief Get the nesting depth of this scope log
        /// @return Depth level (0 for top-level scope)
        [[nodiscard]] size_t depth() const noexcept { return m_scope_depth; }

        /// @brief Get source location of this scope log
        /// @return std::source_location instance
        [[nodiscard]] const std::source_location& location() const noexcept { return m_location; }

        /// @brief Get optional user message attached to scope log
        /// @return Message string
        [[nodiscard]] std::string_view name() const noexcept { return m_scope_name; }

        /// @brief Construct a ScopedDebugLog with optional message and location
        /// @param msg Custom scope label or context message
        /// @param sl Source location (defaults to caller site)
        explicit ScopedDebugLog(std::string_view sn, const std::source_location& sl = std::source_location::current())
            : m_location(sl)
            , m_start_timestamp(std::chrono::system_clock::now())
            , m_scope_name(sn)
            , m_scope_depth(current_depth()++)
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
            if (current_depth() > 0) {
                --current_depth();
            }

#if defined(DEBUG) || defined(_DEBUG)
            auto        us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed()).count();
            std::string indent(m_scope_depth * 2, ' ');

            std::println(std::cerr,
                         "{}{}{} - COMPLETED - time:{}{}us{}",
                         indent,
                         BLU,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         GRN,
                         us,
                         NOC);
#endif
        }


        template <typename... Args>
        void warn(std::format_string<Args...> fmt, Args&&... args)
        {
            std::string indent(m_scope_depth * 2, ' ');
            std::println(std::cerr,
                         "{}{}{} - {}{}",
                         indent,
                         YLW,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         std::format(fmt, std::forward<Args>(args)...),
                         NOC);
        }

        template <typename... Args>
        void err(std::format_string<Args...> fmt, Args&&... args)
        {
            std::string indent(m_scope_depth * 2, ' ');
            std::println(std::cerr,
                         "{}{}{} - {}{}",
                         indent,
                         RED,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         std::format(fmt, std::forward<Args>(args)...),
                         NOC);
        }

        void exp(const std::exception& e)
        {
            std::string indent(m_scope_depth * 2, ' ');
            std::println(std::cerr,
                         "{}{}{} - {}{}{} - {}{}",
                         indent,
                         RED,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         BOLD,
                         typeid(e).name(),
                         NOTBOLD,
                         e.what(),
                         NOC);
        }

        template <typename... Args>
        void msg(std::format_string<Args...> fmt, Args&&... args)
        {
#if defined(DEBUG) || defined(_DEBUG)
            std::string indent(m_scope_depth * 2, ' ');
            std::println(std::cerr,
                         "{}{}{} - {}{}",
                         indent,
                         NOC,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         std::format(fmt, std::forward<Args>(args)...),
                         NOC);
#endif
        }


        /// @brief Format scope log as string representation
        /// @tparam charT Character type (default char)
        /// @return Formatted scope log description
        template <typename charT = char>
        [[nodiscard]] auto to_string() const
        {
            if constexpr (std::is_same_v<charT, char>) {
                auto        us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed()).count();
                std::string indent(m_scope_depth * 2, ' ');
                if (m_scope_name.empty()) {
                    return std::format("{}[{}:{}] {} took {}us",
                                       indent,
                                       m_location.file_name(),
                                       m_location.line(),
                                       m_location.function_name(),
                                       us);
                }
                else {
                    return std::format("{}[{}:{}] {} ({}) took {}us",
                                       indent,
                                       m_location.file_name(),
                                       m_location.line(),
                                       m_location.function_name(),
                                       m_scope_name,
                                       us);
                }
            }
            else {
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
        std::source_location                  m_location;
        std::chrono::system_clock::time_point m_start_timestamp;
        std::string                           m_scope_name {};
        size_t                                m_scope_depth {0};
    };

} // namespace siddiqsoft

#endif // SIDDIQSOFT_SCOPEDDEBUGLOG_HPP
