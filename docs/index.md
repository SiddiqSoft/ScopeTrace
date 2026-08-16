# ScopeTrace

![](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status%2FSiddiqSoft.ScopeTrace?repoName=SiddiqSoft%2FScopeTrace&branchName=master)
[![NuGet](https://img.shields.io/nuget/v/SiddiqSoft.ScopeTrace)](https://www.nuget.org/packages/SiddiqSoft.ScopeTrace/)
![](https://img.shields.io/nuget/v/SiddiqSoft.ScopeTrace)
![](https://img.shields.io/github/v/tag/SiddiqSoft/ScopeTrace)
![](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/34)

`siddiqsoft::ScopeTrace` is a header-only C++23 RAII scope logger designed for execution tracing, performance measurement, and nesting depth visualization.

---

## Motivation

How many of us have had to write code that is surrounded by `#if defined(DEBUG)..` and `std::println(std::cerr, ..)` through out your code.

=== "Before: Using macros.."

    You had to add guards and litter your code with macros..

    ```cpp
    void foo() {
    #if defined(DEBUG)
            std::println(std::cerr, "{} - Something or the other", __func__);
    #endif

        try {
            ...
        } catch(std::exception& e) {
            std::println(std::cerr, "{} - Exception: {}", __func__, e.what());
        }

    #if defined(DEBUG)
            std::println(std::cerr, "{} - COMPLETED", __func__);
    #endif
    }
    ```

=== "With ScopeTrace.."

    Focus on your code and write your message/comments without worrying about formatting strings, colors, indentation and calculating the timings..

    ```cpp
    void foo() {
        // Access process singleton with LogLevel::info threshold
        auto& root = siddiqsoft::ScopeTrace::GetInstance("foo", siddiqsoft::LogLevel::info);

        try {
            auto inner = root.sub_scope("Nested", siddiqsoft::LogLevel::info); // Inner scope label ("foo/Nested")

            inner.info("From the inner scope line: {}", __LINE__);
            // We log information and throw in one shot!
            inner.err_throw<std::runtime_error>("Deliberate error");
        }
        catch (const std::exception& e) {
            // Catch an error and log
            root.exp(e);
        }
    }
    ```

---

## Key Highlights

- **Zero-Boilerplate Tracing**: Automatically record function name, file path, and line numbers using `std::source_location` and auto-extracted `__func__` names.
- **Process Singleton Entry**: Instantiated exclusively via static `ScopeTrace::GetInstance()` process singleton.
- **Nesting Level Tracking**: Indents nested scope execution trees dynamically using parentage depth inheritance (`sub_scope()`).
- **Structured Console Logging**: Specialized logging methods for `info()`, `trace()`, `warn()`, `err()`, and `exp()` with depth indentation, ANSI colors, and ISO 8601 UTC timestamps.
- **C++23 Native Support**: Leverages `std::format` and `std::println` to `std::cerr`.

---

## Quick Example

```cpp
#include <iostream>
#include <siddiqsoft/ScopeTrace.hpp>

void process_request()
{
    // Create nested scope from process singleton
    auto scope = siddiqsoft::ScopeTrace::GetInstance().sub_scope("process_request", siddiqsoft::LogLevel::info);
    scope.info("Parsing incoming payload...");
    scope.warn("Payload buffer usage: 82%");
    // Work executed here...
}

int main()
{
    // Access process singleton root
    auto& scope = siddiqsoft::ScopeTrace::GetInstance("main", siddiqsoft::LogLevel::info);
    process_request();
    return 0;
}
```

### Output & Coloring Preview

<div class="terminal-window" style="background-color: #1b1d24; border-radius: 8px; border: 1px solid #2e3240; margin: 1.5rem 0; overflow: hidden; font-family: 'JetBrains Mono', Consolas, monospace;">
  <div class="terminal-header" style="background-color: #242836; padding: 8px 14px; display: flex; align-items: center; gap: 8px; border-bottom: 1px solid #2e3240;">
    <span class="terminal-dot red" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ff5f56; display: inline-block;"></span>
    <span class="terminal-dot yellow" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ffbd2e; display: inline-block;"></span>
    <span class="terminal-dot green" style="width: 12px; height: 12px; border-radius: 50%; background-color: #27c93f; display: inline-block;"></span>
    <span class="terminal-title" style="color: #8b949e; font-size: 12px; margin-left: 6px; font-family: sans-serif;">Console Output (std::cerr)</span>
  </div>
  <pre class="terminal-body" style="padding: 14px 18px; color: #e6edf3; font-size: 13px; line-height: 1.65; overflow-x: auto; margin: 0; background: transparent;"><span style="color: #8b949e;">2026-08-15T20:49:27.060564Z</span>|[<span style="color: #e6edf3;">info  </span>]|<span style="color: #8b949e;">main/process_request</span>|Parsing incoming payload...
<span style="color: #8b949e;">2026-08-15T20:49:27.060912Z</span>|[<span style="background-color: #ffd700; color: #000000; font-weight: 600; padding: 0 3px;">warn  </span>]|<span style="color: #8b949e;">main/process_request</span>|<span style="color: #ffd700;">Payload buffer usage: 82%</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.061250Z</span>|[<span style="color: #8b949e;">debug </span>]|<span style="color: #8b949e;">main/process_request</span>|<span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">688us</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.061300Z</span>|[<span style="color: #8b949e;">debug </span>]|<span style="color: #8b949e;">main</span>|<span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">1240us</span></pre>
</div>

---

## Navigation Guide

- [**Features Overview**](features/index.md)
- [**CMake & Integration**](integration/cmake.md)
- [**API Reference**](api/index.md)
- [**Examples & Walkthroughs**](examples/index.md)
