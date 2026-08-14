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
- **`scope.exp(e)`**: Formatted exception log (colored bold red with exception type and `what()`, active in all build modes).
