# Scope Logging

## How RAII Scope Logging Works

`siddiqsoft::ScopeTrace` uses the C++ RAII pattern to measure scope execution duration and emit trace logs upon scope entry and exit.

```cpp
#include <siddiqsoft/ScopeTrace.hpp>

void compute()
{
    // Defaults scope name to plain function name ("compute")
    siddiqsoft::ScopeTrace scope;
    
    // Explicit nested scope ("compute-stage1")
    auto sub = scope.nest("stage1");
    
    sub.info("Processing stage 1 payload...");
} // Destructors log completion status and elapsed duration automatically in debug builds
```

## Source Location & Function Name Extraction

Using `std::source_location::current()`, `ScopeTrace` captures caller details automatically:
- Source file path (`location().file_name()`)
- Line number (`location().line()`)
- Enclosing function signature (`location().function_name()`)

`ScopeTrace` also includes built-in parsing (`extract_func_name()` / `func_name()`) to extract clean plain function names matching `__func__` from full signature strings. When `ScopeTrace` is declared in the global scope (outside of any enclosing function), `extract_func_name()` evaluates to `"GLOBAL"`.

## Nesting Depth & ISO 8601 Timestamps

`ScopeTrace` automatically tracks nested scopes using a thread-local counter (`current_depth()`), producing formatted visual indentation for hierarchical log trees. Each output line is prefixed with an ISO 8601 UTC timestamp (`current_timestamp()`):

```text
2026-08-13T23:16:00.519049Z   GLOBAL - COMPLETED - time:150us
2026-08-13T23:16:00.519100Z     GLOBAL-stage1 - COMPLETED - time:40us
```

## Dynamic Log Level Filtering

Each `ScopeTrace` instance maintains a configurable log level threshold (`LogLevel` / `trace_level`, defaulted to `LogLevel::critical` upon construction). You can set the threshold at scope creation or dynamically update it via `set_level()`:

```cpp
// Set logging threshold to debug at construction
siddiqsoft::ScopeTrace scope("compute", siddiqsoft::LogLevel::debug);

// Dynamically change logging threshold
scope.set_level(siddiqsoft::LogLevel::trace);
```

### Filtering Rules

- **Always Logged:** Messages logged with `critical`, `exception`, or `error` level are **always output** regardless of `m_log_level`.
- **`LogLevel::trace`**: Enables all diagnostic logs (`warning`, `info`, `debug`, `trace`). Recommended for high-volume I/O operations.
- **`LogLevel::debug`**: Enables `warning`, `info`, and `debug` logs (excludes `trace`).
- **`LogLevel::info`**: Enables `warning` and `info` logs (excludes `debug` and `trace`).
- **`LogLevel::warning`**: Enables `warning` logs only.

## Log Level Colors & Output Styling

`ScopeTrace` applies distinct ANSI color escape codes to each log level tag to make terminal output visually scannable:

| Log Level / Method | Tag Label | Color Output | ANSI Code | Purpose & Typical Use |
| :--- | :--- | :--- | :--- | :--- |
| **`trace_level::critical`** | `[critical]` | **Red** | `\033[0;31m` | System-critical failures requiring immediate action. Always logged. |
| **`trace_level::exception`** | `[exception]` | **Red / Bold Red** | `\033[0;31m` | Caught exception details and `err_throw()` events. Always logged. |
| **`trace_level::error`** | `[error]` | **Orange** | `\033[38;5;208m` | Operational errors and failed preconditions. Always logged. |
| **`trace_level::warning`** | `[warning]` | **Dark Yellow / Gold** | `\033[38;5;136m` | Recoverable warnings and degraded performance. Logged if threshold `>= warning`. |
| **`trace_level::info`** | `[info]` | **Default / Neutral** | `\033[0m` | General informational events and application state changes. Logged if threshold `>= info`. |
| **`trace_level::debug`** | `[debug]` | **Light Gray** | `\033[38;5;250m` | Detailed internal algorithm state and variable values. Logged if threshold `>= debug`. |
| **`trace_level::trace`** | `[trace]` | **Light Gray** | `\033[38;5;250m` | High-frequency I/O operations, raw packets, & buffer dumps. Logged if threshold `>= trace`. |

> [!TIP]
> **Best Practice: High-Frequency I/O Operations Should Use `trace` (`scope.trace(...)`)**
> 
> High-volume diagnostic logging — such as socket reads/writes, raw payload packet dumps, file stream buffer transfers, HTTP payload tracing, or inner loop iterations — should always use `trace_level::trace` (`scope.trace(...)`).
> 
> When `m_log_level` is set to `LogLevel::debug` or `info`, all `scope.trace(...)` statements are bypassed via a zero-allocation threshold check (`level <= m_log_level`), eliminating string formatting and stream output overhead during standard debugging while keeping log buffers clean.

## In-Scope Logging Sinks

Within an active scope, you can output formatted contextual messages to `std::cerr` with depth indentation and ANSI color coding:

- **`scope.trace("...")`**: Trace diagnostic details (light gray, active when `m_log_level >= trace`). Ideal for I/O packet/buffer dumps.
- **`scope.debug("...")`**: Debug diagnostic details (light gray, active when `m_log_level >= debug`).
- **`scope.info("...")`**: Informational messages (active when `m_log_level >= info`).
- **`scope.warn("...")`**: Warning messages (colored dark yellow, active when `m_log_level >= warning`).
- **`scope.err("...")`**: Error messages (colored orange, always logged).
- **`scope.err_throw<EX>("...")`**: Unified error logging & exception throwing shortcut (colored orange, throws `EX(formatted_msg)`, always logged).
- **`scope.exp(e)`**: Exception handler shortcut. Logs exception type (`typeid(e).name()`) in bold red and `e.what()` (always logged).
- **`scope.exp(e, "...")`**: Exception handler shortcut with context. Logs exception type, italicized `e.what()`, and custom formatted contextual details (always logged).

## Unified Error Throwing & Catching Patterns

`ScopeTrace` simplifies error flow by combining console logging with exception throwing (`err_throw()`) and exception handling (`exp()`):

### 1. Throw Site (`err_throw`)
Instead of separate log and throw statements, `err_throw<EX>(fmt, args...)` logs the formatted error message with timestamps, depth indentation, and exception type details, then immediately throws `EX`:

```cpp
void validate_input(int value)
{
    siddiqsoft::ScopeTrace scope;

    if (value < 0) {
        // Logs orange error message and throws std::invalid_argument
        scope.err_throw<std::invalid_argument>("Value must be non-negative, got: {}", value);
    }
}
```

### 2. Catch Site (`exp`)
Inside `catch` blocks, `exp()` logs caught exception details with zero boilerplate:

```cpp
try {
    validate_input(-5);
}
catch (const std::exception& e) {
    // Basic exception log: outputs typeid name and e.what()
    scope.exp(e);

    // Exception log with custom formatted contextual details:
    scope.exp(e, "Validation failed during request ID: {}", request_id);
}
```
