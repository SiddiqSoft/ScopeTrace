# ScopeTrace

![](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status%2FSiddiqSoft.ScopeTrace?repoName=SiddiqSoft%2FScopeTrace&branchName=master)
![](https://img.shields.io/nuget/v/SiddiqSoft.ScopeTrace)
![](https://img.shields.io/github/v/tag/SiddiqSoft/ScopeTrace)
![](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/34)
[![NuGet](https://img.shields.io/nuget/v/SiddiqSoft.ScopeTrace)](https://www.nuget.org/packages/SiddiqSoft.ScopeTrace/)

`siddiqsoft::ScopeTrace` is a modern, lightweight, header-only C++23 RAII scope logger designed for performance profiling and scope execution tracing.

- **RAII Scope Timing**: Automatic duration measurement upon scope exit.
- **`std::source_location` Integration**: Capture file, line, and function automatically.
- **Nesting Level Tracking**: Indents nested scope execution trees dynamically using thread-local depth counter (`current_depth()`) and `nest()` method.
- **Dynamic Log Level Filtering**: Fine-grained threshold control (`LogLevel` / `trace_level`). Critical, exception, and error logs are always output, while warning, info, debug, and trace are filtered according to threshold (`m_log_level`).
- **Structured Console Logging**: Specialized `info()`, `debug()`, `trace()`, `warn()`, `err()`, `err_throw()`, and `exp()` methods for formatted console logging with ANSI colors and ISO 8601 UTC timestamps.
- **String Formatting & Stream Support**: Native `to_string()` formatting and `operator<<` stream insertion support.

---

## Documentation Site Links

For full detailed documentation, integration guides, and API specifications, visit our MkDocs documentation site:

- [**Features Overview**](https://SiddiqSoft.github.io/ScopeTrace/features/)
- [**Integration & CMake Guide**](https://SiddiqSoft.github.io/ScopeTrace/integration/cmake/)
- [**Dependency Graph**](https://SiddiqSoft.github.io/ScopeTrace/integration/dependencies/)
- [**API Reference**](https://SiddiqSoft.github.io/ScopeTrace/api/)
- [**Examples & Walkthroughs**](https://SiddiqSoft.github.io/ScopeTrace/examples/)

---

## Quick Start

```cpp
#include <iostream>
#include <siddiqsoft/ScopeTrace.hpp>

// Global instance configured with trace threshold
static siddiqsoft::ScopeTrace Log{"MYPROJECT", siddiqsoft::LogLevel::trace};

void sub_task()
{
    // Create nested context ("MYPROJECT-sub_task") with info threshold
    auto inner = Log.nest(__func__, siddiqsoft::LogLevel::info);
    auto last_line = __LINE__;

    try {
        last_line = __LINE__;
        inner.info("Processing items...");

        // Perform work...
        last_line = __LINE__;
        call_something(); // throws std::runtime_error("Device non-responsive")
    } catch (const std::exception& ex) {
        // Logs exception type, e.what(), and contextual line information
        inner.exp(ex, "Got exception last_line: {}", last_line);
    }

    // Upon scope exit, destructors log completed message and elapsed time
}

int main()
{
    Log.info("Starting application execution");
    sub_task();
    return 0;
}
```

---

## Sample Output & Console Coloring

When running the quick start application, `siddiqsoft::ScopeTrace` outputs depth-indented log lines to `std::cerr` prefixed by ISO 8601 UTC timestamps and styled with ANSI level color codes:

<div class="terminal-window" style="background-color: #1b1d24; border-radius: 8px; border: 1px solid #2e3240; margin: 1.5rem 0; overflow: hidden; font-family: 'JetBrains Mono', Consolas, monospace;">
  <div class="terminal-header" style="background-color: #242836; padding: 8px 14px; display: flex; align-items: center; gap: 8px; border-bottom: 1px solid #2e3240;">
    <span class="terminal-dot red" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ff5f56; display: inline-block;"></span>
    <span class="terminal-dot yellow" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ffbd2e; display: inline-block;"></span>
    <span class="terminal-dot green" style="width: 12px; height: 12px; border-radius: 50%; background-color: #27c93f; display: inline-block;"></span>
    <span class="terminal-title" style="color: #8b949e; font-size: 12px; margin-left: 6px; font-family: sans-serif;">Console Output (std::cerr)</span>
  </div>
  <pre class="terminal-body" style="padding: 14px 18px; color: #e6edf3; font-size: 13px; line-height: 1.65; overflow-x: auto; margin: 0; background: transparent;"><span style="color: #6e7681;">  Creating NEW SCOPE MYPROJECT:6</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.060564Z</span>  <span style="color: #e6edf3;">[info  ]</span> MYPROJECT - Starting application execution
<span style="color: #6e7681;">  Creating NEW SCOPE MYPROJECT-sub_task:4</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.060600Z</span>  <span style="color: #e6edf3;">[info  ]</span>   MYPROJECT-sub_task - Processing items...
<span style="color: #8b949e;">2026-08-15T20:49:27.061200Z</span>  <span style="color: #ff7b72;">[exception]</span>   <span style="color: #ff7b72;">MYPROJECT-sub_task - <b>std::runtime_error</b> - <i>Device non-responsive</i> - Got exception last_line: 54</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.061250Z</span>  <span style="color: #8b949e;">[debug ]</span>   <span style="color: #8b949e;">MYPROJECT-sub_task - COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">650us</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.061300Z</span>  <span style="color: #8b949e;">[debug ]</span> MYPROJECT - COMPLETED: time:<span style="color: #3fb950; font-weight: 600;">7360us</span></pre>
</div>

### Log Level Color Palette

| Log Level / Method | Tag Label | Color Output | ANSI Code | Visual Output Preview |
| :--- | :--- | :--- | :--- | :--- |
| **`trace_level::critical`** | `[critical]` | **Red** | `\033[0;31m` | <span style="color: #ff7b72; font-weight: 600;">[critical] System memory exhaustion</span> |
| **`trace_level::exception`** | `[exception]` | **Red** | `\033[0;31m` | <span style="color: #ff7b72; font-weight: 600;">[exception] std::runtime_error - Connection refused</span> |
| **`trace_level::error`** | `[error]` | **Orange** | `\033[38;5;208m` | <span style="color: #ffa657; font-weight: 600;">[error  ] Failed to open configuration file</span> |
| **`trace_level::warning`** | `[warning]` | **Dark Yellow** | `\033[38;5;136m` | <span style="color: #d29922; font-weight: 600;">[warning] Cache capacity reached 92%</span> |
| **`trace_level::info`** | `[info]` | **Default / Neutral** | `\033[0m` | <span style="color: #e6edf3;">[info   ] Server listening on port 8080</span> |
| **`trace_level::debug`** | `[debug]` | **Light Gray** | `\033[38;5;250m` | <span style="color: #8b949e;">[debug  ] Worker thread depth: 2</span> |
| **`trace_level::trace`** | `[trace]` | **Dark Blue** | `\033[38;5;19m` | <span style="color: #58a6ff;">[trace  ] RX payload: 0x41 0x42 0x43</span> |
| **Scope Exit** | `COMPLETED` | **Green Time** | `\033[0;32m` | <span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">450us</span> |

---

## Installation & Integration

Integrate via [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake):

```cmake
include(CPM.cmake)

CPMAddPackage("gh:SiddiqSoft/ScopeTrace#v1.0.0")

target_link_libraries(your_target PRIVATE siddiqsoft::ScopeTrace)
```

For more details, see the [CMake & Integration Documentation](https://SiddiqSoft.github.io/ScopeTrace/integration/cmake/).

---

## Requirements & Building

- **C++ Compiler**: C++23 compliant compiler (MSVC 2022 v17.10+, GCC 13+, Clang 17+).
- **CMake**: Version >= 3.29.

### Preset Build & Test

```bash
# Configure using Apple-Debug / Linux-GCC-Debug / Windows-x64-Debug preset
cmake --preset Darwin

# Build test binaries
cmake --build build/Darwin

# Execute unit tests
ctest --test-dir build/Darwin
```

---

## License

Distributed under the [BSD 3-Clause License](LICENSE).
Copyright (c) 2026 Siddiq Software LLC.
