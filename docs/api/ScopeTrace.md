# `ScopeTrace` Class API Reference

Header: `<siddiqsoft/ScopeTrace.hpp>`

```cpp
namespace siddiqsoft {
    class ScopeTrace;
}
```

`siddiqsoft::ScopeTrace` is a modern C++23 RAII scope logging utility that measures duration, tracks nesting levels, records source locations, and formats diagnostic messages.

---

## Public Constructors & Destructor

### `explicit ScopeTrace(std::string_view sn, const std::source_location& sl = std::source_location::current())`
Constructs a scope logger with the specified scope name `sn` and current source location `sl`. Increments the thread-local nesting depth.

- **`sn`**: Scope name or contextual identifier.
- **`sl`**: Source location (defaults to `std::source_location::current()`).

### `~ScopeTrace() noexcept`
Destructor. Decrements the thread-local nesting depth. In debug builds (`DEBUG` / `_DEBUG`), logs a scope completion message with elapsed duration (in microseconds) to `std::cerr`.

### Copy & Move Operations
Copy construction, move construction, copy assignment, and move assignment operators are explicitly `= delete`.

---

## Static Methods

### `static size_t& current_depth() noexcept`
Accesses the thread-local scope depth counter. Returns a reference to the current thread's nesting depth index (`size_t`).

### `[[nodiscard]] static std::string_view extract_func_name(std::string_view full_signature) noexcept`
Extracts the plain function name (matching the `__func__` macro) from a full function signature string (such as `std::source_location::function_name()`).

---

## Accessor Member Functions

### `[[nodiscard]] auto elapsed() const noexcept`
Calculates and returns the duration (`std::chrono::system_clock::duration`) elapsed since the `ScopeTrace` instance was constructed.

### `[[nodiscard]] size_t depth() const noexcept`
Returns the nesting depth index of this scope instance (`0` for top-level scopes).

### `[[nodiscard]] const std::source_location& location() const noexcept`
Returns the `std::source_location` object captured at construction.

### `[[nodiscard]] std::string_view name() const noexcept`
Returns the scope name string view passed at construction.

### `[[nodiscard]] std::string_view function_name() const noexcept`
Returns the plain function name matching the `__func__` macro, extracted from `m_location.function_name()`.

### `[[nodiscard]] std::string_view func_name() const noexcept`
Shorthand alias for `function_name()`.

---

## Logging Member Functions

### `template <typename... Args> void warn(std::format_string<Args...> fmt, Args&&... args)`
Formats and outputs a warning message to `std::cerr` (colored yellow) prefixed by the depth-indented scope name.

### `template <typename... Args> void err(std::format_string<Args...> fmt, Args&&... args)`
Formats and outputs an error message to `std::cerr` (colored red) prefixed by the depth-indented scope name.

### `void exp(const std::exception& e)`
Outputs caught exception details (`typeid(e).name()` and `e.what()`) to `std::cerr` (colored bold red) prefixed by the depth-indented scope name.

### `template <typename... Args> void msg(std::format_string<Args...> fmt, Args&&... args)`
In debug builds (`DEBUG` / `_DEBUG`), formats and outputs an informational message to `std::cerr` prefixed by the depth-indented scope name.

---

## Formatting & Stream Operations

### `template <typename charT = char> [[nodiscard]] auto to_string() const`
Formats the scope log as a string containing line indentation, source location, function name, scope name, and elapsed duration in microseconds.

### `friend std::ostream& operator<<(std::ostream& os, const ScopeTrace& src)`
Stream insertion operator. Writes `src.to_string<char>()` to the standard output stream `os`.
