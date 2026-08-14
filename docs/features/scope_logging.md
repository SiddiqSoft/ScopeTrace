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

## In-Scope Logging Sinks

Within an active scope, you can output formatted contextual messages to `std::cerr` with depth indentation and ANSI color coding:

- **`scope.info("...")`**: Debug information (active in `DEBUG` / `_DEBUG` builds).
- **`scope.trace("...")`**: Trace diagnostic details (light gray, active in `DEBUG_TRACE` builds).
- **`scope.warn("...")`**: Warning messages (colored yellow, active in all build modes).
- **`scope.err("...")`**: Error messages (colored red, active in all build modes).
- **`scope.err_throw<EX>("...")`**: Unified error logging & exception throwing shortcut (colored orange, throws `EX(formatted_msg)`).
- **`scope.exp(e)`**: Exception handler shortcut. Logs exception type (`typeid(e).name()`) in bold red and `e.what()` (active in all build modes).
- **`scope.exp(e, "...")`**: Exception handler shortcut with context. Logs exception type, italicized `e.what()`, and custom formatted contextual details (active in all build modes).

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
