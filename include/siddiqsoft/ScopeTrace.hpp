/*
    ScopeTrace : Modern C++23 RAII Scope Logger Library
    Version 1.0.0

    https://github.com/SiddiqSoft/ScopeTrace

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
#ifndef SIDDIQSOFT_SCOPETRACE_HPP
#define SIDDIQSOFT_SCOPETRACE_HPP 1

#include <chrono>
#include <format>
#include <ostream>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

namespace siddiqsoft
{
    /// @brief Modern RAII scope logger measuring elapsed duration, location, and nesting level.
    class ScopeTrace
    {
        // Colors for output
        static constexpr std::string_view LTGY {"\033[38;5;250m"}; //< Light gray
        static constexpr std::string_view DKGY {"\033[1;40m"};     //< Dark gray
        static constexpr std::string_view RED {"\033[0;31m"};      //< Red
        static constexpr std::string_view BLU {"\033[0;34m"};      //< Blue
        static constexpr std::string_view GRN {"\033[0;32m"};      //< Green
        static constexpr std::string_view YLW {"\033[1;33m"};      //< Yellow
        static constexpr std::string_view BOLD {"\033[1m"};        //< Bold
        static constexpr std::string_view ITAL {"\033[3m"};        //< Italic
        static constexpr std::string_view NOTBOLD {"\033[22m"};    //< Not bold
        static constexpr std::string_view NOTITAL {"\033[23m"};    //< Not italic
        static constexpr std::string_view NOC {"\033[0m"};         //< No Color
        static constexpr std::string_view global_function_name {"GLOBAL"};

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

        /// @brief Get custom scope label/name attached to scope log
        /// @return Scope name string view
        [[nodiscard]] std::string_view name() const noexcept { return m_scope_name; }

        /// @brief Extract the plain function name matching __func__ from a full function signature
        /// @param full_signature Function signature string (e.g. from std::source_location::function_name())
        /// @return Plain function name string view
        [[nodiscard]] static std::string_view extract_func_name(std::string_view full_signature) noexcept
        {
            if (full_signature.empty()) return global_function_name;

            // Find opening parenthesis of parameter list outside template angle brackets
            size_t param_start = std::string_view::npos;
            size_t angle_depth = 0;
            for (size_t i = 0; i < full_signature.size(); ++i) {
                char c = full_signature[i];
                if (c == '<') {
                    ++angle_depth;
                }
                else if (c == '>') {
                    if (angle_depth > 0) --angle_depth;
                }
                else if (c == '(' && angle_depth == 0) {
                    param_start = i;
                    break;
                }
            }

            std::string_view name_part =
                    (param_start != std::string_view::npos) ? full_signature.substr(0, param_start) : full_signature;

            // Trim trailing whitespace
            while (!name_part.empty() && (name_part.back() == ' ' || name_part.back() == '\t')) {
                name_part.remove_suffix(1);
            }

            if (name_part.empty()) return global_function_name;

            // Scan backwards to find start of plain function name (after last ':', space, '*', '&')
            size_t end_pos   = name_part.size();
            size_t start_pos = end_pos;
            angle_depth      = 0;

            for (size_t i = end_pos; i > 0; --i) {
                char c = name_part[i - 1];
                if (c == '>') {
                    ++angle_depth;
                }
                else if (c == '<') {
                    if (angle_depth > 0) --angle_depth;
                }
                else if (angle_depth == 0) {
                    if (c == ':' || c == ' ' || c == '\t' || c == '*' || c == '&') {
                        start_pos = i;
                        break;
                    }
                }
                if (angle_depth == 0 && i - 1 == 0) {
                    start_pos = 0;
                }
            }

            return name_part.substr(start_pos);
        }

        [[nodiscard]] static std::string current_timestamp() noexcept
        {
            // This gives a timestamp in ISO 8601 format with UTC timezone: "2026-08-13T23:16:00.519049Z"
            // Note we use fractional seconds with microsecond precision.
            return std::format("{}{:%FT%TZ}{}  ", LTGY, std::chrono::system_clock::now(), NOC);
        }

        /// @brief Get the plain function name matching __func__ from the source location
        /// @return Plain function name string view
        [[nodiscard]] std::string_view function_name() const noexcept { return extract_func_name(m_location.function_name()); }

        /// @brief Alias for function_name() matching __func__
        /// @return Plain function name string view
        [[nodiscard]] std::string_view func_name() const noexcept { return function_name(); }

    public:
        /// @brief Construct a ScopeTrace with a scope name and optional source location
        /// @param sn Custom scope label or context name
        /// @param sl Source location (defaults to caller site)
        explicit ScopeTrace(std::string_view sn = {}, const std::source_location& sl = std::source_location::current())
            : m_location(sl)
            , m_start_timestamp(std::chrono::system_clock::now())
            , m_scope_name(sn)
            , m_scope_depth(current_depth()++)
        {
            if (sn.empty()) m_scope_name = extract_func_name(m_location.function_name());
        }

        ScopeTrace nest(std::string_view sn, const std::source_location& sl = std::source_location::current())
        {
            // The scope name is required when nesting.
            // We will add the function name and the scope name to the log message.
            return ScopeTrace {std::format("{}-{}", m_scope_name, sn), sl};
        }

        /// @brief Copying is not supported
        ScopeTrace(const ScopeTrace&) = delete;

        /// @brief Moving is not supported
        ScopeTrace(ScopeTrace&&) = delete;

        /// @brief Copy assignment is not supported
        ScopeTrace& operator=(const ScopeTrace&) = delete;

        /// @brief Move assignment is not supported
        ScopeTrace& operator=(ScopeTrace&&) = delete;

        /// @brief Destructor logs scope completion in debug builds and updates depth counter
        ~ScopeTrace() noexcept
        {
            if (current_depth() > 0) {
                --current_depth();
            }

#if defined(DEBUG) || defined(_DEBUG) || defined(DEBUG_TRACE)
            auto        us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed()).count();
            std::string indent(m_scope_depth * 2, ' ');

            std::println(std::cerr,
                         "{}{}{}{} - COMPLETED - time:{}{}us{}",
                         current_timestamp(),
                         indent,
                         BLU,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         GRN,
                         us,
                         NOC);
#endif
        }

        /// @brief Log a formatted warning message to std::cerr with indentation and scope label
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        template <typename... Args>
        void warn(std::format_string<Args...> fmt, Args&&... args)
        {
            std::string indent(m_scope_depth * 2, ' ');
            std::println(std::cerr,
                         "{}{}{}{} - {}{}",
                         current_timestamp(),
                         indent,
                         YLW,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         std::format(fmt, std::forward<Args>(args)...),
                         NOC);
        }

        /// @brief Log a formatted warning message to std::cerr with indentation and scope label
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        template <typename... Args>
        void trace(std::format_string<Args...> fmt, Args&&... args)
        {
#if defined(DEBUG_TRACE)
            std::string indent(m_scope_depth * 2, ' ');
            std::println(std::cerr,
                         "{}{}{}{} - {}{}",
                         current_timestamp(),
                         indent,
                         LTGY,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         std::format(fmt, std::forward<Args>(args)...),
                         NOC);
#endif
        }

        /// @brief Log a formatted error message to std::cerr with indentation and scope label
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        template <typename... Args>
        void err(std::format_string<Args...> fmt, Args&&... args)
        {
            std::string indent(m_scope_depth * 2, ' ');
            std::println(std::cerr,
                         "{}{}{}{} - {}{}",
                         current_timestamp(),
                         indent,
                         RED,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         std::format(fmt, std::forward<Args>(args)...),
                         NOC);
        }

        /// @brief Log exception details (type and message) to std::cerr with indentation and scope label
        /// @param e Reference to caught exception
        void exp(const std::exception& e)
        {
            std::string indent(m_scope_depth * 2, ' ');
            std::println(std::cerr,
                         "{}{}{}{} - {}{}{} - {}{}",
                         current_timestamp(),
                         indent,
                         RED,
                         m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                         BOLD,
                         typeid(e).name(),
                         NOTBOLD,
                         e.what(),
                         NOC);
        }

        /// @brief Log a formatted debug message to std::cerr in debug builds with indentation and scope label
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        template <typename... Args>
        void info(std::format_string<Args...> fmt, Args&&... args)
        {
#if defined(DEBUG) || defined(_DEBUG)
            std::string indent(m_scope_depth * 2, ' ');
            std::println(std::cerr,
                         "{}{}{}{} - {}{}",
                         current_timestamp(),
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
                    return std::format("{}{}[{}:{}] {} took {}us",
                                       current_timestamp(),
                                       indent,
                                       m_location.file_name(),
                                       m_location.line(),
                                       m_location.function_name(),
                                       us);
                }
                else {
                    return std::format("{}{}[{}:{}] {} ({}) took {}us",
                                       current_timestamp(),
                                       indent,
                                       m_location.file_name(),
                                       m_location.line(),
                                       m_location.function_name(),
                                       m_scope_name,
                                       us);
                }
            }
            else {
                static_assert(std::is_same_v<charT, char>, "ScopeTrace supports char formatting");
            }
        }

        /// @brief Stream insertion operator for ScopeTrace
        friend std::ostream& operator<<(std::ostream& os, const ScopeTrace& src)
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

#endif // SIDDIQSOFT_SCOPETRACE_HPP
