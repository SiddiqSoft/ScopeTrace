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
    trace     = 6,  // Logged if m_log_level >= trace (Dark Blue output, all logs)
    none      = 255 // Disabled threshold
};
```

| Log Level | Color Output | ANSI Escape Sequence | Visual Terminal Preview | Standard Use Case |
| :--- | :--- | :--- | :--- | :--- |
| `critical` | **Red** | `\033[0;31m` | <span style="color: #ff7b72; font-weight: 600;">[critical] Out of memory</span> | Fatal system failures |
| `exception` | **Red** | `\033[0;31m` | <span style="color: #ff7b72; font-weight: 600;">[exception] std::runtime_error</span> | Exception handler logging (`exp()`) |
| `error` | **Orange** | `\033[38;5;208m` | <span style="color: #ffa657; font-weight: 600;">[error  ] Network unreachable</span> | Operational errors (`err()`, `err_throw()`) |
| `warning` | **Dark Yellow / Gold** | `\033[38;5;136m` | <span style="color: #d29922; font-weight: 600;">[warning] High memory usage</span> | Recoverable warnings (`warn()`) |
| `info` | **Default / Neutral** | `\033[0m` | <span style="color: #e6edf3;">[info   ] Task starting...</span> | Application status changes (`info()`) |
| `debug` | **Light Gray** | `\033[38;5;250m` | <span style="color: #8b949e;">[debug  ] Step 3 complete</span> | General debug inspection (`debug()`) |
| `trace` | **Dark Blue** | `\033[38;5;19m` | <span style="color: #58a6ff;">[trace  ] RX 1024 bytes</span> | High-frequency I/O operations & packet/buffer dumps (`trace()`) |

> **Advice for High-Volume I/O Logging:**
> High-frequency operations — such as socket data transfers, stream packet dumps, HTTP body payload dumps, or file buffer reads/writes — should **always** use `trace_level::trace` (`scope.trace(...)`). This allows I/O diagnostic noise to be cleanly filtered out when running at `LogLevel::debug` or `info` thresholds without incurring string formatting costs.

### `template <typename... Args> struct source_location_format_string`
A specialized format string wrapper struct in `namespace siddiqsoft` that combines `std::format_string<Args...>` with a `consteval` constructor capturing caller `std::source_location::current()`. Used by `err_throw()` to capture exact call-site file and line numbers.

---

## Public Constructors & Destructor

### `explicit ScopeTrace(std::string_view sn = {}, trace_level level = trace_level::none, const std::source_location& sl = std::source_location::current())`
Constructs a scope logger with the specified scope name `sn`, logging threshold `level`, and caller source location `sl`. Increments the thread-local nesting depth. If `sn` is omitted or empty (`{}`), the scope name automatically defaults to the plain function name extracted via `extract_func_name(sl.function_name())` (evaluating to `"GLOBAL"` when declared in global scope outside of any function).

- **`sn`**: Custom scope label or contextual identifier (defaults to `{}`).
- **`level`**: Scope logging threshold (`LogLevel` / `trace_level`, defaults to `trace_level::none`).
- **`sl`**: Source location (defaults to caller site via `std::source_location::current()`).

### `ScopeTrace nest(std::string_view sn, trace_level level = trace_level::none, const std::source_location& sl = std::source_location::current())`
Creates and returns a new child `ScopeTrace` instance. The child scope name is automatically formatted as `"<parent_scope_name>/<sn>"`. If `level` is set to `trace_level::none` (default), the child scope automatically inherits the parent's logging threshold (`m_log_level`).

- **`sn`**: Sub-scope label string (required).
- **`level`**: Child scope logging threshold (`LogLevel` / `trace_level`, defaults to `trace_level::none` to inherit parent threshold).
- **`sl`**: Source location (defaults to caller site).

### `~ScopeTrace() noexcept`
Destructor. Decrements the thread-local nesting depth counter. Logs a scope completion message with elapsed duration in microseconds (`COMPLETED: time:...us`) at `trace_level::debug` severity.

### Copy & Move Operations
Copy construction, move construction, copy assignment, and move assignment operators are explicitly `= delete`.

---

## Public Accessor & Modifier Member Functions

### `auto& set_level(trace_level level) noexcept`
Updates the scope's logging threshold level (`m_log_level`). Returns a reference to `*this`.

---

## Public Logging Member Functions

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

## Protected & Non-Public Implementation Details

The following internal helper methods, formatting functions, constants, and data members are declared `protected:` (exposed as `public:` to unit test suites only when `SCOPETRACE_PRIVATE_TESTING` is defined):

### Protected Helper Methods (`protected:`)

- **`static size_t& current_depth() noexcept`**  
  Accesses reference to thread-local scope nesting depth index (`thread_local size_t depth = 0`).
- **`[[nodiscard]] auto elapsed() const noexcept`**  
  Calculates duration (`std::chrono::system_clock::duration`) elapsed since scope construction.
- **`[[nodiscard]] size_t depth() const noexcept`**  
  Returns nesting depth index of this scope (`0` for top-level scope).
- **`[[nodiscard]] const std::source_location& location() const noexcept`**  
  Returns `std::source_location` captured at scope entry.
- **`[[nodiscard]] std::string_view name() const noexcept`**  
  Returns scope name string view (`m_scope_name`).
- **`[[nodiscard]] static std::string_view extract_func_name(std::string_view full_signature) noexcept`**  
  Extracts plain function name matching `__func__` from full signature (returns `"GLOBAL"` if empty).
- **`[[nodiscard]] static std::string current_timestamp() noexcept`**  
  Returns ISO 8601 UTC timestamp string with microsecond precision e.g. `"2026-08-13T23:16:00.519049Z  "` (styled with `LTGY` color).
- **`[[nodiscard]] std::string_view function_name() const noexcept`**  
  Returns plain function name matching `__func__` extracted from `m_location.function_name()`.
- **`[[nodiscard]] static constexpr std::string_view logline_start_color(trace_level level) noexcept`**  
  Maps a `trace_level` severity tag to its corresponding ANSI start escape sequence (`RED`, `ORN`, `DKYLW`, `NOC`, `LTGY`, or `DRKBLU`).
- **`[[nodiscard]] static constexpr std::string_view logline_end_color(trace_level) noexcept`**  
  Returns the ANSI color reset escape sequence `NOC` (`\033[0m`).
- **`[[nodiscard]] std::string indent_buffer(trace_level level) const`**  
  Formats a colored `[level]` tag padded to 7 characters followed by `m_scope_depth * 2` spaces indentation.
- **`[[nodiscard]] std::string logline_prefix(trace_level level) const`**  
  Generates line prefix string containing ISO 8601 UTC timestamp in light gray (`LTGY`), colored `[level]` severity tag, and depth-indented spaces (`m_scope_depth * 2`).

### Private Color & String Constants (`private:`)

| Constant | Type | Value | Description |
| :--- | :--- | :--- | :--- |
| `LTGY` | `std::string_view` | `\033[38;5;250m` | Light gray color sequence for timestamps and caller details |
| `DKGY` | `std::string_view` | `\033[1;40m` | Dark gray background sequence |
| `RED` | `std::string_view` | `\033[0;31m` | Red sequence for `critical` and `exception` levels |
| `ORN` | `std::string_view` | `\033[38;5;208m` | Orange sequence for `error` level |
| `BLU` | `std::string_view` | `\033[0;34m` | Standard blue sequence |
| `DRKBLU` | `std::string_view` | `\033[38;5;19m` | Dark blue sequence for `trace` level |
| `GRN` | `std::string_view` | `\033[0;32m` | Green sequence for scope completion duration (`COMPLETED: time:...us`) |
| `YLW` | `std::string_view` | `\033[1;33m` | Yellow sequence modifier |
| `DKYLW` | `std::string_view` | `\033[38;5;136m` | Dark yellow sequence for `warning` level |
| `BOLD` / `NOTBOLD` | `std::string_view` | `\033[1m` / `\033[22m` | Bold text toggle sequences |
| `ITAL` / `NOTITAL` | `std::string_view` | `\033[3m` / `\033[23m` | Italic text toggle sequences |
| `UNDL` / `NOTUNDL` | `std::string_view` | `\033[4m` / `\033[24m` | Underline text toggle sequences |
| `NOC` | `std::string_view` | `\033[0m` | Reset all ANSI styles |
| `global_function_name` | `std::string_view` | `"GLOBAL"` | Fallback function name when instantiated in global scope |

### Private Member Variables (`private:`)

- **`trace_level m_log_level`**: Threshold filter level (`trace_level::error` default).
- **`std::source_location m_location`**: Caller source location recorded upon scope entry.
- **`std::chrono::system_clock::time_point m_start_timestamp`**: Entry timestamp for calculating `elapsed()`.
- **`std::string m_scope_name`**: Custom scope label or auto-extracted plain function name (`"GLOBAL"` or `__func__`).
- **`size_t m_scope_depth`**: Scope nesting depth level captured from `current_depth()`.

