# `ScopeTrace` Class API Reference

Header: `<siddiqsoft/ScopeTrace.hpp>`

```cpp
namespace siddiqsoft {
    enum class trace_level : uint8_t;
    using LogLevel = trace_level;
    class ScopeTrace;
}
```

`siddiqsoft::ScopeTrace` is a modern C++23 RAII scope logging utility that measures duration, tracks nesting levels, records source locations, and formats diagnostic messages.

---

## Helper Types & Enums

### `enum class trace_level : uint8_t` (Alias: `LogLevel`)
Defines message log levels and scope threshold levels:

```cpp
enum class trace_level : uint8_t {
    critical  = 0,  // Always logged (Red output)
    exception = 1,  // Always logged (Red output)
    error     = 2,  // Always logged (Orange output)
    warning   = 3,  // Logged if m_log_level >= warning (Dark Yellow / Gold output)
    info      = 4,  // Logged if m_log_level >= info (Default/Neutral output)
    debug     = 5,  // Logged if m_log_level >= debug (Light Gray output, excludes trace)
    trace     = 6,  // Logged if m_log_level >= trace (Light Gray output, all logs)
    none      = 255 // Disabled threshold
};
```

| Log Level | Color Output | ANSI Escape Sequence | Standard Use Case |
| :--- | :--- | :--- | :--- |
| `critical` | **Red** | `\033[0;31m` | Fatal system failures |
| `exception` | **Red** | `\033[0;31m` | Exception handler logging (`exp()`) |
| `error` | **Orange** | `\033[38;5;208m` | Operational errors (`err()`, `err_throw()`) |
| `warning` | **Dark Yellow / Gold** | `\033[38;5;136m` | Recoverable warnings (`warn()`) |
| `info` | **Default / Neutral** | `\033[0m` | Application status changes (`info()`) |
| `debug` | **Light Gray** | `\033[38;5;250m` | General debug inspection (`debug()`) |
| `trace` | **Light Gray** | `\033[38;5;250m` | High-frequency I/O operations & packet/buffer dumps (`trace()`) |

> **Advice for High-Volume I/O Logging:**
> High-frequency operations — such as socket data transfers, stream packet dumps, HTTP body payload dumps, or file buffer reads/writes — should **always** use `trace_level::trace` (`scope.trace(...)`). This allows I/O diagnostic noise to be cleanly filtered out when running at `LogLevel::debug` or `info` thresholds without incurring string formatting costs.

### `template <typename... Args> struct source_location_format_string`
A specialized format string wrapper struct in `namespace siddiqsoft` that combines `std::format_string<Args...>` with a `consteval` constructor capturing caller `std::source_location::current()`. Used by `err_throw()` to capture exact call-site file and line numbers.

---

## Public Constructors & Destructor

### `explicit ScopeTrace(std::string_view sn = {}, trace_level level = trace_level::critical, const std::source_location& sl = std::source_location::current())`
Constructs a scope logger with the specified scope name `sn`, logging threshold `level`, and caller source location `sl`. Increments the thread-local nesting depth. If `sn` is omitted or empty (`{}`), the scope name automatically defaults to the plain function name extracted via `extract_func_name(sl.function_name())` (evaluating to `"GLOBAL"` when declared in global scope outside of any function).

- **`sn`**: Custom scope label or contextual identifier (defaults to `{}`).
- **`level`**: Scope logging threshold (`LogLevel` / `trace_level`, defaults to `trace_level::critical`).
- **`sl`**: Source location (defaults to caller site via `std::source_location::current()`).

### `ScopeTrace nest(std::string_view sn, trace_level level = trace_level::critical, const std::source_location& sl = std::source_location::current())`
Creates and returns a new child `ScopeTrace` instance. The child scope name is automatically formatted as `"<parent_scope_name>-<sn>"`.

- **`sn`**: Sub-scope label string.
- **`level`**: Child scope logging threshold (`LogLevel` / `trace_level`, defaults to `trace_level::critical`).
- **`sl`**: Source location (defaults to caller site).

### `~ScopeTrace() noexcept`
Destructor. Decrements the thread-local nesting depth counter. Logs a scope completion message with elapsed duration in microseconds and ISO 8601 UTC timestamp to `std::cerr`.

### Copy & Move Operations
Copy construction, move construction, copy assignment, and move assignment operators are explicitly `= delete`.

---

## Static Methods

### `static size_t& current_depth() noexcept`
Accesses the thread-local scope depth counter. Returns a reference to the current thread's nesting depth index (`size_t`).

### `[[nodiscard]] static std::string_view extract_func_name(std::string_view full_signature) noexcept`
Extracts the plain function name (matching the `__func__` macro) from a full function signature string (such as `std::source_location::function_name()`). Returns `"GLOBAL"` if `full_signature` is empty or instantiated in global scope outside of a function.

### `[[nodiscard]] static std::string current_timestamp() noexcept`
Returns an ISO 8601 UTC timestamp string with microsecond precision e.g. `"2026-08-13T23:16:00.519049Z  "` (styled with ANSI light gray color codes).

---

## Accessor & Modifier Member Functions

### `auto& set_level(trace_level level) noexcept`
Updates the scope's logging threshold level (`m_log_level`). Returns a reference to `*this`.

### `[[nodiscard]] auto elapsed() const noexcept`
Calculates and returns the duration (`std::chrono::system_clock::duration`) elapsed since the `ScopeTrace` instance was constructed.

### `[[nodiscard]] size_t depth() const noexcept`
Returns the nesting depth index of this scope instance (`0` for top-level scopes).

### `[[nodiscard]] const std::source_location& location() const noexcept`
Returns the `std::source_location` object captured at construction.

### `[[nodiscard]] std::string_view name() const noexcept`
Returns the scope name string view passed at construction (or the auto-extracted plain function name if `sn` was empty).

### `[[nodiscard]] std::string_view function_name() const noexcept`
Returns the plain function name matching the `__func__` macro, extracted from `m_location.function_name()`.

---

## Logging Member Functions

### `template <trace_level level = trace_level::critical, typename... Args> auto& log(std::format_string<Args...> fmt, Args&&... args)`
Core formatted logging template. Formats and outputs a diagnostic message to `std::cerr`.
- **Filtering Logic**: Messages with `level` equal to `critical`, `exception`, or `error` are **always logged** regardless of `m_log_level`. For other levels (`warning`, `info`, `debug`, `trace`), the message is logged if `level <= m_log_level`.
- **Formatting**: Output line includes ISO 8601 UTC timestamp, formatted level tag (e.g. `[critical]`, `[warning]`, `[debug]`), nesting depth spaces, ANSI colors, scope name, and the formatted message.

### `template <typename... Args> auto& trace(std::format_string<Args...> fmt, Args&&... args)`
Logs a trace message (`log<trace_level::trace>`). Active if `m_log_level >= trace_level::trace`.

### `template <typename... Args> auto& debug(std::format_string<Args...> fmt, Args&&... args)`
Logs a debug message (`log<trace_level::debug>`). Active if `m_log_level >= trace_level::debug`. Excludes `trace` level logs.

### `template <typename... Args> auto& info(std::format_string<Args...> fmt, Args&&... args)`
Logs an info message (`log<trace_level::info>`). Active if `m_log_level >= trace_level::info`. Excludes `debug` and `trace` logs.

### `template <typename... Args> auto& warn(std::format_string<Args...> fmt, Args&&... args)`
Logs a warning message (`log<trace_level::warning>`). Active if `m_log_level >= trace_level::warning`.

### `template <typename... Args> auto& err(std::format_string<Args...> fmt, Args&&... args)`
Logs an error message (`log<trace_level::error>`). Always logged regardless of `m_log_level`.

### `template <typename EX = std::exception, typename... Args> void err_throw(source_location_format_string<std::type_identity_t<Args>...> fmt_loc, Args&&... args) noexcept(false)`
Unified error logging and exception throwing shortcut. Uses `source_location_format_string` to capture the caller's exact call-site `std::source_location`. Formats and logs an error message to `std::cerr` (colored orange) with ISO 8601 UTC timestamp, depth-indented scope name, bold exception type name (`typeid(EX).name()`), formatted message in italics, and call-site `file:line` details. Then constructs and throws `EX(formatted_message)`. Always logged.

### `void exp(const std::exception& e)`
Shortcut for logging caught exceptions in `catch` blocks. Outputs exception type (`typeid(e).name()`) in bold red and exception message (`e.what()`) to `std::cerr` prefixed by timestamp and depth-indented scope name. Always logged.

### `template <typename... Args> void exp(const std::exception& e, std::format_string<Args...> fmt, Args&&... args)`
Enhanced exception logging shortcut for `catch` blocks. Outputs exception type (`typeid(e).name()`) in bold red, exception message (`e.what()`) in italic, followed by custom formatted contextual details (`fmt`, `args...`) to `std::cerr` prefixed by timestamp and depth-indented scope name. Always logged.

---

## Formatting & Stream Operations

### `template <typename charT = char> [[nodiscard]] auto to_string() const`
Formats the scope log as a string containing ISO 8601 UTC timestamp, line indentation, source location, function name, scope name, and elapsed duration in microseconds.

### `friend std::ostream& operator<<(std::ostream& os, const ScopeTrace& src)`
Stream insertion operator. Writes `src.to_string<char>()` to the standard output stream `os`.

