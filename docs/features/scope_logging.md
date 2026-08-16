# Scope Logging

## How RAII Scope Logging Works

`siddiqsoft::ScopeTrace` uses the C++ RAII pattern to measure scope execution duration and emit trace logs upon scope entry and exit.

```cpp
#include <siddiqsoft/ScopeTrace.hpp>

void compute()
{
    // Obtain process-wide root ScopeTrace instance ("compute")
    auto& scope = siddiqsoft::ScopeTrace::GetInstance("compute");
    
    // Explicit nested scope ("compute/stage1")
    auto sub = scope.sub_scope("stage1");
    
    sub.info("Processing stage 1 payload...");
} // Destructors automatically log completion status and elapsed duration at LogLevel::debug severity upon scope exit
```

## Source Location & Function Name Extraction

Using `std::source_location::current()`, `ScopeTrace` captures caller details automatically upon construction:
- Source file path (`m_location.file_name()`)
- Line number (`m_location.line()`)
- Enclosing function signature (`m_location.function_name()`)

`ScopeTrace` also includes internal protected helper routines (`extract_file_name()`, `extract_func_name()`, `function_name()`) to extract clean plain file names (e.g. `"main.cpp"`) and function names matching `__func__` from full signature strings.

## Nesting Depth & ISO 8601 Timestamps

`ScopeTrace` tracks nested scope depth strictly by parentage (`child.depth() = parent.depth() + 1`), producing formatted visual indentation for hierarchical log trees. Each output line is prefixed with an ISO 8601 UTC timestamp:

<div class="terminal-window" style="background-color: #1b1d24; border-radius: 8px; border: 1px solid #2e3240; margin: 1.5rem 0; overflow: hidden; font-family: 'JetBrains Mono', Consolas, monospace;">
  <div class="terminal-header" style="background-color: #242836; padding: 8px 14px; display: flex; align-items: center; gap: 8px; border-bottom: 1px solid #2e3240;">
    <span class="terminal-dot red" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ff5f56; display: inline-block;"></span>
    <span class="terminal-dot yellow" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ffbd2e; display: inline-block;"></span>
    <span class="terminal-dot green" style="width: 12px; height: 12px; border-radius: 50%; background-color: #27c93f; display: inline-block;"></span>
    <span class="terminal-title" style="color: #8b949e; font-size: 12px; margin-left: 6px; font-family: sans-serif;">Console Output (Hierarchical Indentation)</span>
  </div>
  <pre class="terminal-body" style="padding: 14px 18px; color: #e6edf3; font-size: 13px; line-height: 1.65; overflow-x: auto; margin: 0; background: transparent;"><span style="color: #8b949e;">2026-08-15T20:49:27.060564Z</span>|[<span style="color: #e6edf3;">info  </span>]|<span style="color: #8b949e;">compute</span>|Outer computation started
<span style="color: #8b949e;">2026-08-15T20:49:27.060600Z</span>|[<span style="color: #e6edf3;">info  </span>]|<span style="color: #8b949e;">compute/stage1</span>|Processing stage 1 payload...
<span style="color: #8b949e;">2026-08-15T20:49:27.061250Z</span>|[<span style="color: #8b949e;">debug </span>]|<span style="color: #8b949e;">compute/stage1</span>|<span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">650us</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.061300Z</span>|[<span style="color: #8b949e;">debug </span>]|<span style="color: #8b949e;">compute</span>|<span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">1200us</span></pre>
</div>

## Dynamic Log Level Filtering

Each `ScopeTrace` instance maintains a configurable log level threshold (`LogLevel` / `trace_level`, defaulted to `LogLevel::none`). You can set the threshold at instance acquisition or dynamically update it via `set_level()`:

```cpp
// Set logging threshold to debug at instance acquisition
auto& scope = siddiqsoft::ScopeTrace::GetInstance("compute", siddiqsoft::LogLevel::debug);

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

| Log Level / Method | Tag Label | Tag Color Output | ANSI Escape Code | Visual Terminal Preview |
| :--- | :--- | :--- | :--- | :--- |
| **`trace_level::critical`** | `crit  ` | **Reverse Red** | `\033[7;31m` | [<span style="background-color: #da3633; color: #ffffff; font-weight: 600; padding: 0 3px;">crit  </span>] System memory exhaustion |
| **`trace_level::exception`** | `except` | **Reverse Red** | `\033[7;31m` | [<span style="background-color: #da3633; color: #ffffff; font-weight: 600; padding: 0 3px;">except</span>] std::runtime_error - Timeout |
| **`trace_level::error`** | `error ` | **Reverse Orange** | `\033[7;38;5;208m` | [<span style="background-color: #d96c00; color: #ffffff; font-weight: 600; padding: 0 3px;">error </span>] Failed to connect to db host |
| **`trace_level::warning`** | `warn  ` | **Reverse Light Yellow** | `\033[7;38;5;220m` | [<span style="background-color: #ffd700; color: #000000; font-weight: 600; padding: 0 3px;">warn  </span>] Cache capacity reached 92% |
| **`trace_level::info`** | `info  ` | **Default / Neutral** | `\033[0m` | <span style="color: #e6edf3;">[info  ] Processing batch item 42</span> |
| **`trace_level::debug`** | `debug ` | **Light Gray** | `\033[38;5;250m` | <span style="color: #8b949e;">[debug ] Worker thread pool depth: 4</span> |
| **`trace_level::trace`** | `trace ` | **Dark Blue** | `\033[38;5;19m` | <span style="color: #58a6ff;">[trace ] RX buffer dump: 0x41 0x42 0x43</span> |
| **Scope Exit** | `COMPLETED` | **Green Time** | `\033[0;32m` | <span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">450us</span> |

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
    auto scope = siddiqsoft::ScopeTrace::GetInstance().sub_scope("validate_input");

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
