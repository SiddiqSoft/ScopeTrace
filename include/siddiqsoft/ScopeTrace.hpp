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
#include <cstdint>
#ifndef SIDDIQSOFT_SCOPETRACE_HPP
#define SIDDIQSOFT_SCOPETRACE_HPP 1

#include <chrono>
#include <concepts>
#include <format>
#include <iostream>
#include <ostream>
#include <print>
#include <source_location>
#include <string>
#include <string_view>
#include <type_traits>

#if defined(_WIN32) || defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif
#include <utility>

namespace siddiqsoft
{
    enum class trace_level : uint8_t
    {
        critical  = 0,
        exception = 1,
        error     = 2,
        warning   = 3,
        info      = 4,
        debug     = 5,
        trace     = 6,
        none      = 255,
    };

    using LogLevel = trace_level;
} // namespace siddiqsoft

// Specialize std::formatter for trace_level
template <>
struct std::formatter<siddiqsoft::trace_level> : std::formatter<std::string_view>
{
    auto format(const siddiqsoft::trace_level& c, auto& ctx) const
    {
        std::string_view name = "unknown";
        switch (c) {
            case siddiqsoft::trace_level::critical: name = "crit"; break;
            case siddiqsoft::trace_level::exception: name = "except"; break;
            case siddiqsoft::trace_level::error: name = "error"; break;
            case siddiqsoft::trace_level::warning: name = "warn"; break;
            case siddiqsoft::trace_level::info: name = "info"; break;
            case siddiqsoft::trace_level::debug: name = "debug"; break;
            case siddiqsoft::trace_level::trace: name = "trace"; break;
            case siddiqsoft::trace_level::none: name = "none"; break;
            default: break;
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

namespace siddiqsoft
{
    /// @brief Wrapper combining std::format_string with automatic caller std::source_location capture.
    template <typename... Args>
    struct source_location_format_string
    {
        std::format_string<Args...> fmt;
        std::source_location        location;

        template <typename S>
            requires std::convertible_to<const S&, std::string_view>
        consteval source_location_format_string(const S& str, std::source_location loc = std::source_location::current())
            : fmt(str)
            , location(loc)
        {
        }
    };

    /// @brief Modern RAII scope logger measuring elapsed duration, location, and nesting level.
    class ScopeTrace
    {
        // Colors for output
        static constexpr std::string_view LTGY {"\033[38;5;250m"};       //< Light gray
        static constexpr std::string_view DKGY {"\033[1;40m"};           //< Dark gray
        static constexpr std::string_view RED {"\033[0;31m"};            //< Red
        static constexpr std::string_view REV_RED {"\033[7;31m"};        //< Reverse Red
        static constexpr std::string_view ORN {"\033[38;5;208m"};        //< Orange
        static constexpr std::string_view REV_ORN {"\033[7;38;5;208m"};  //< Reverse Orange
        static constexpr std::string_view BLU {"\033[0;34m"};            //< Blue
        static constexpr std::string_view DRKBLU {"\033[38;5;19m"};      //< Dark Blue
        static constexpr std::string_view GRN {"\033[0;32m"};            //< Green
        static constexpr std::string_view YLW {"\033[1;33m"};            //< Yellow
        static constexpr std::string_view LTYLW {"\033[38;5;220m"};      //< Light Bright Yellow
        static constexpr std::string_view REV_LTYLW {"\033[7;38;5;220m"};//< Reverse Light Bright Yellow
        static constexpr std::string_view BOLD {"\033[1m"};              //< Bold
        static constexpr std::string_view ITAL {"\033[3m"};              //< Italic
        static constexpr std::string_view UNDL {"\033[4m"};              //< Underline
        static constexpr std::string_view NOTUNDL {"\033[24m"};          //< Not underline
        static constexpr std::string_view NOTBOLD {"\033[22m"};          //< Not bold
        static constexpr std::string_view NOTITAL {"\033[23m"};          //< Not italic
        static constexpr std::string_view NOC {"\033[0m"};               //< No Color
        static constexpr std::string_view global_function_name {};

        // We're using this to allow unit tests to access protected members of ScopeTrace for testing purposes.
#if defined(SCOPETRACE_PRIVATE_TESTING)
    public:
#else
    protected:
#endif
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

        /// @brief Extract the plain file name from a full file path string
        /// @param file_path File path string (e.g. from std::source_location::file_name())
        /// @return Plain file name string view
        [[nodiscard]] static std::string_view extract_file_name(std::string_view file_path) noexcept
        {
            if (file_path.empty()) return global_function_name;
            size_t last_slash = file_path.find_last_of("/\\");
            if (last_slash != std::string_view::npos) {
                return file_path.substr(last_slash + 1);
            }
            return file_path;
        }

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
            return std::format("{}{:%FT%TZ}{} ", LTGY, std::chrono::system_clock::now(), NOC);
        }

        /// @brief Get the plain function name matching __func__ from the source location
        /// @return Plain function name string view
        [[nodiscard]] std::string_view function_name() const noexcept { return extract_func_name(m_location.function_name()); }

        /// @brief Helper constructor for creating child scopes with explicit parent depth
        ScopeTrace(std::string_view            sn,
                   trace_level                 level,
                   size_t                      depth_val,
                   const std::source_location& sl = std::source_location::current())
            : m_location(sl)
            , m_start_timestamp(std::chrono::system_clock::now())
            , m_scope_name(sn)
            , m_scope_depth(depth_val)
            , m_log_level(level)
        {
            if (sn.empty()) m_scope_name = extract_file_name(m_location.file_name());
        }

        /// @brief Construct a ScopeTrace with a scope name, log level threshold, and optional source location
        /// @note Direct construction is protected; obtain the process singleton via ScopeTrace::GetInstance(...) or
        /// parent.sub_scope(...)
        explicit ScopeTrace(std::string_view            sn    = {},
                            trace_level                 level = trace_level::none,
                            const std::source_location& sl    = std::source_location::current())
            : ScopeTrace(sn, level, 0, sl)
        {
        }

    public:
        /// @brief Obtain the process-wide singleton ScopeTrace instance
        /// @param sn Custom scope label for the process (defaults to file name on first initialization)
        /// @param level Process-wide logging threshold (defaults to trace_level::none on first initialization)
        /// @param sl Source location (defaults to caller site on first initialization)
        /// @return Reference to the single process-wide ScopeTrace instance
        static ScopeTrace& GetInstance(std::string_view            sn    = {},
                                       trace_level                 level = trace_level::none,
                                       const std::source_location& sl    = std::source_location::current())
        {
#if defined(_WIN32) || defined(_WIN64)
            // We need to enable ANSI escape code processing on Windows 10+ for colored output in the console.
            static const bool win_ansi_initialized = []() {
                HANDLE hErr = GetStdHandle(STD_ERROR_HANDLE);
                if (hErr != INVALID_HANDLE_VALUE && hErr != NULL) {
                    DWORD mode = 0;
                    if (GetConsoleMode(hErr, &mode)) {
                        SetConsoleMode(hErr, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
                    }
                }
                return true;
            }();
            (void)win_ansi_initialized;
#endif

            static ScopeTrace instance {sn, level, 0, sl};
            if (!sn.empty()) {
                instance.m_scope_name = sn;
            }
            if (level != trace_level::none) {
                instance.m_log_level = level;
            }
            return instance;
        }

        /// @brief Create a nested sub-scope with child scope name and logging threshold
        /// @note The new sub_scope will inherit the parent's logging threshold if the child level is set to trace_level::none.
        /// The child scope depth is derived directly from the parent's depth (parent.depth() + 1).
        /// @param sn Custom sub-scope label (appended as "parent/child")
        /// @param level Logging threshold for child scope (defaults to trace_level::none)
        /// @param sl Source location (defaults to caller site)
        /// @return New ScopeTrace instance
        ScopeTrace sub_scope(std::string_view            sn, //< required sub-scope name
                             trace_level                 level = trace_level::none,
                             const std::source_location& sl    = std::source_location::current()) const
        {
            return ScopeTrace {m_scope_name.empty() ? std::string(sn) : std::format("{}/{}", m_scope_name, sn),
                               level == trace_level::none ? m_log_level : level,
                               m_scope_depth + 1,
                               sl};
        }

        /// @brief Copying is not supported
        ScopeTrace(const ScopeTrace&) = delete;

        /// @brief Moving is not supported
        ScopeTrace(ScopeTrace&&) = delete;

        /// @brief Copy assignment is not supported
        ScopeTrace& operator=(const ScopeTrace&) = delete;

        /// @brief Move assignment is not supported
        ScopeTrace& operator=(ScopeTrace&&) = delete;

    protected:
        [[nodiscard]] static constexpr auto logline_tag_color(trace_level level) noexcept -> std::string_view
        {
            switch (level) {
                case trace_level::critical: return REV_RED;
                case trace_level::exception: return REV_RED;
                case trace_level::error: return REV_ORN;
                case trace_level::warning: return REV_LTYLW;
                case trace_level::info: return NOC;
                case trace_level::debug: return LTGY;
                case trace_level::trace: return DRKBLU;
                default: return NOC;
            }
        }

        [[nodiscard]] static constexpr auto logline_start_color(trace_level level) noexcept -> std::string_view
        {
            switch (level) {
                case trace_level::critical: return RED;
                case trace_level::exception: return RED;
                case trace_level::error: return ORN;
                case trace_level::warning: return LTYLW;
                case trace_level::info: return NOC;
                case trace_level::debug: return LTGY;
                case trace_level::trace: return DRKBLU;
                default: return NOC;
            }
        }

        [[nodiscard]] static constexpr auto logline_end_color(trace_level) noexcept -> std::string_view { return NOC; }

        [[nodiscard]] auto                  logline_prefix(trace_level level) const -> std::string
        {
            return std::format(
                    "{}{:%FT%TZ}{} {: <6} {}", LTGY, std::chrono::system_clock::now(), logline_tag_color(level), level, NOC);
        }

    public:
        /// @brief Output formatted log message filtered by level threshold
        /// @tparam level Message severity (defaults to trace_level::critical). Always logged if critical/exception/error; filtered
        /// by m_log_level for warning/info/debug/trace.
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        /// @return Reference to this ScopeTrace instance
        template <trace_level level = trace_level::critical, typename... Args>
        auto& log(std::format_string<Args...> fmt, Args&&... args)
        {
            constexpr bool is_always_logged =
                    (level == trace_level ::critical || level == trace_level ::exception || level == trace_level ::error);

            if (is_always_logged || level <= m_log_level) {
                std::println(std::cerr,
                             "{}{}{}{}{}|{}|{}",
                             logline_prefix(level),
                             LTGY, // the name of the scope is always light gray, regardless of log level
                             m_scope_name.empty() ? m_location.file_name() : m_scope_name,
                             NOC,
                             logline_start_color(level),
                             std::format(fmt, std::forward<Args>(args)...),
                             logline_end_color(level));
            }

            return *this;
        }


        /// @brief Log a formatted warning message to std::cerr with indentation and scope label
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        template <typename... Args>
        auto& warn(std::format_string<Args...> fmt, Args&&... args)
        {
            return log<siddiqsoft::trace_level ::warning>(fmt, std::forward<Args>(args)...);
        }

        /// @brief Log a formatted warning message to std::cerr with indentation and scope label
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        template <typename... Args>
        auto& trace(std::format_string<Args...> fmt, Args&&... args)
        {
            return log<siddiqsoft::trace_level ::trace>(fmt, std::forward<Args>(args)...);
        }

        /// @brief Log a formatted warning message to std::cerr with indentation and scope label
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        template <typename... Args>
        auto& debug(std::format_string<Args...> fmt, Args&&... args)
        {
            return log<siddiqsoft::trace_level ::debug>(fmt, std::forward<Args>(args)...);
        }


        /// @brief Log a formatted error message to std::cerr with indentation and scope label
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        template <typename... Args>
        auto& err(std::format_string<Args...> fmt, Args&&... args)
        {
            return log<siddiqsoft::trace_level ::error>(fmt, std::forward<Args>(args)...);
        }

        /// @brief Log exception details (type and message) to std::cerr with indentation and scope label
        /// @param e Reference to caught exception
        void exp(const std::exception& e)
        {
            log<siddiqsoft::trace_level ::exception>("{}{}{}{}", BOLD, typeid(e).name(), NOTBOLD, e.what());
        }

        template <typename... Args>
        void exp(const std::exception& e, std::format_string<Args...> fmt, Args&&... args)
        {
            log<siddiqsoft::trace_level ::exception>("{}{}{} - {}{}{} - {}",
                                                     BOLD,
                                                     typeid(e).name(),
                                                     NOTBOLD,
                                                     ITAL,
                                                     e.what(),
                                                     NOTITAL,
                                                     std::format(fmt, std::forward<Args>(args)...));
        }


        /// @brief Log error with caller source location and throw exception
        /// @tparam EX Exception type to throw (defaults to std::exception)
        /// @tparam Args Format argument types
        /// @param fmt_loc Format string with captured call-site std::source_location
        /// @param args Format arguments
        template <typename EX = std::exception, typename... Args>
        void err_throw(source_location_format_string<std::type_identity_t<Args>...> fmt_loc, Args&&... args) noexcept(false)
        {
            const auto& loc = fmt_loc.location;
            std::string msg = std::format(fmt_loc.fmt, std::forward<Args>(args)...);

            log<siddiqsoft::trace_level::exception>("{}{}{} - {}{}{}{} - {}from:{}@{}{}",
                                                    BOLD,
                                                    typeid(EX).name(),
                                                    NOTBOLD,
                                                    ITAL,
                                                    msg,
                                                    NOTITAL,
                                                    NOC,
                                                    UNDL,
                                                    loc.file_name(),
                                                    loc.line(),
                                                    NOTUNDL);

            throw EX(msg);
        }


        /// @brief Log a formatted debug message to std::cerr in debug builds with indentation and scope label
        /// @tparam Args Format argument types
        /// @param fmt Format string
        /// @param args Format arguments
        template <typename... Args>
        auto& info(std::format_string<Args...> fmt, Args&&... args)
        {
            return log<siddiqsoft::trace_level ::info>(fmt, std::forward<Args>(args)...);
        }

        /// @brief Update the logging threshold level for this scope
        /// @param level New trace_level / LogLevel threshold
        /// @return Reference to this ScopeTrace instance
        auto& set_level(trace_level level) noexcept
        {
            m_log_level = level;
            return *this;
        }

        /// @brief Destructor logs scope completion in debug builds
        ~ScopeTrace() noexcept
        {
            auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed()).count();

            log<siddiqsoft::trace_level::debug>("COMPLETED: time:{}{}us{}", GRN, us, NOC);
        }

    private:
        trace_level                           m_log_level {trace_level ::error};
        std::source_location                  m_location;
        std::chrono::system_clock::time_point m_start_timestamp;
        std::string                           m_scope_name {};
        size_t                                m_scope_depth {0};
    };

} // namespace siddiqsoft

#endif // SIDDIQSOFT_SCOPETRACE_HPP
