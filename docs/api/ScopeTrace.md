# `ScopeTrace` Class API Reference

Header: `<siddiqsoft/ScopeTrace.hpp>`

```cpp
namespace siddiqsoft {
    class ScopeTrace;
}
```

`siddiqsoft::ScopeTrace` is a modern C++23 RAII scope logging utility that measures duration, tracks nesting levels, records source locations, and formats diagnostic messages.

---

## Helper Types

### `template <typename... Args> struct source_location_format_string`
A specialized format string wrapper struct in `namespace siddiqsoft` that combines `std::format_string<Args...>` with a `consteval` constructor capturing caller `std::source_location::current()`. Used by `err_throw()` to capture exact call-site file and line numbers.

---

## Public Constructors & Destructor

### `explicit ScopeTrace(std::string_view sn = {}, const std::source_location& sl = std::source_location::current())`
Constructs a scope logger with the specified scope name `sn` and caller source location `sl`. Increments the thread-local nesting depth. If `sn` is omitted or empty (`{}`), the scope name automatically defaults to the plain function name extracted via `extract_func_name(sl.function_name())` (evaluating to `"GLOBAL"` when declared in global scope outside of any function).

- **`sn`**: Custom scope label or contextual identifier (defaults to `{}`).
- **`sl`**: Source location (defaults to caller site via `std::source_location::current()`).

### `ScopeTrace nest(std::string_view sn, const std::source_location& sl = std::source_location::current())`
Creates and returns a new child `ScopeTrace` instance. The child scope name is automatically formatted as `"<parent_scope_name>-<sn>"`.

- **`sn`**: Sub-scope label string.
- **`sl`**: Source location (defaults to caller site).

### `~ScopeTrace() noexcept`
Destructor. Decrements the thread-local nesting depth counter. In debug builds (`DEBUG`, `_DEBUG`, or `DEBUG_TRACE`), logs a scope completion message with elapsed duration in microseconds and ISO 8601 UTC timestamp to `std::cerr`.

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

## Accessor Member Functions

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

### `[[nodiscard]] std::string_view func_name() const noexcept`
Shorthand alias for `function_name()`.

---

## Logging Member Functions

### `template <typename... Args> void info(std::format_string<Args...> fmt, Args&&... args)`
In debug builds (`DEBUG` / `_DEBUG`), formats and outputs an informational message to `std::cerr` prefixed by timestamp and depth-indented scope name.

### `template <typename... Args> void trace(std::format_string<Args...> fmt, Args&&... args)`
In trace debug builds (`DEBUG_TRACE`), formats and outputs a trace-level diagnostic message to `std::cerr` (colored light gray) prefixed by timestamp and depth-indented scope name.

### `template <typename... Args> void warn(std::format_string<Args...> fmt, Args&&... args)`
Formats and outputs a warning message to `std::cerr` (colored yellow) prefixed by timestamp and depth-indented scope name. Active in all build modes.

### `template <typename... Args> void err(std::format_string<Args...> fmt, Args&&... args)`
Formats and outputs an error message to `std::cerr` (colored red) prefixed by timestamp and depth-indented scope name. Active in all build modes.

### `template <typename EX = std::exception, typename... Args> void err_throw(source_location_format_string<std::type_identity_t<Args>...> fmt_loc, Args&&... args) noexcept(false)`
Unified error logging and exception throwing shortcut. Uses `source_location_format_string` to capture the caller's exact call-site `std::source_location`. Formats and logs an error message to `std::cerr` (colored orange) with ISO 8601 UTC timestamp, depth-indented scope name, bold exception type name (`typeid(EX).name()`), formatted message in italics, and call-site `file:line` details. Then constructs and throws `EX(formatted_message)`. Active in all build modes.

### `void exp(const std::exception& e)`
Shortcut for logging caught exceptions in `catch` blocks. Outputs exception type (`typeid(e).name()`) in bold red and exception message (`e.what()`) to `std::cerr` prefixed by timestamp and depth-indented scope name. Active in all build modes.

### `template <typename... Args> void exp(const std::exception& e, std::format_string<Args...> fmt, Args&&... args)`
Enhanced exception logging shortcut for `catch` blocks. Outputs exception type (`typeid(e).name()`) in bold red, exception message (`e.what()`) in italic, followed by custom formatted contextual details (`fmt`, `args...`) to `std::cerr` prefixed by timestamp and depth-indented scope name. Active in all build modes.

---

## Formatting & Stream Operations

### `template <typename charT = char> [[nodiscard]] auto to_string() const`
Formats the scope log as a string containing ISO 8601 UTC timestamp, line indentation, source location, function name, scope name, and elapsed duration in microseconds.

### `friend std::ostream& operator<<(std::ostream& os, const ScopeTrace& src)`
Stream insertion operator. Writes `src.to_string<char>()` to the standard output stream `os`.
