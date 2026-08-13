# Scope Logging

## How RAII Scope Logging Works

`siddiqsoft::ScopedDebugLog` uses the C++ RAII pattern to measure scope execution duration and emit trace logs upon scope entry and exit.

```cpp
#include <siddiqsoft/ScopedDebugLog.hpp>

void compute()
{
    siddiqsoft::ScopedDebugLog scope("compute_operation");
    // Code block execution...
} // Destruction logs completion status and elapsed duration automatically in debug builds
```

## Source Location

Using `std::source_location::current()`, `ScopedDebugLog` captures caller details automatically:
- Source file path (`location().file_name()`)
- Line number (`location().line()`)
- Enclosing function signature (`location().function_name()`)

## Nesting Depth Tracking

`ScopedDebugLog` automatically tracks nested scopes using a thread-local counter (`current_depth()`), producing formatted visual indentation for hierarchical log trees:

```text
[main.cpp:12] compute_operation took 150us
  [main.cpp:18] sub_stage took 40us
```

## In-Scope Logging Sinks

Within an active scope, you can output formatted contextual messages to `std::cerr` with depth indentation and ANSI color coding:

- **`scope.msg("...")`**: Debug information (active in `DEBUG` / `_DEBUG` builds).
- **`scope.warn("...")`**: Warning messages (colored yellow).
- **`scope.err("...")`**: Error messages (colored red).
- **`scope.exp(e)`**: Formatted exception log (colored bold red with exception type and `what()`).
