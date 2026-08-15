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
        siddiqsoft::ScopeTrace scope; // defaults name to plain __func__ ("foo")

        try {
            auto inner = scope.nest("Nested"); // explicit inner scope label ("foo-Nested")

            inner.info("From the inner scope line: {}", __LINE__);
            // We log information and throw in one shot!
            inner.err_throw<std::runtime_error>("Deliberate error");
        }
        catch (const std::exception& e) {
            // Catch an error and log
            scope.exp(e);
        }
    }
    ```

---

## Key Highlights

- **Zero-Boilerplate Tracing**: Automatically record function name, file path, and line numbers using `std::source_location` and auto-extracted `__func__` names.
- **Nesting Level Tracking**: Indents nested scope execution trees dynamically using thread-local scope depth and `nest()` scope creation.
- **Structured Console Logging**: Specialized logging methods for `info()`, `trace()`, `warn()`, `err()`, and `exp()` with depth indentation, ANSI colors, and ISO 8601 UTC timestamps.
- **C++23 Native Support**: Leverages `std::format`, `std::println`, and `std::ostream`.

---

## Quick Example

```cpp
#include <iostream>
#include <siddiqsoft/ScopeTrace.hpp>

void process_request()
{
    siddiqsoft::ScopeTrace scope("process_request");
    scope.info("Parsing incoming payload...");
    // Work executed here...
}

int main()
{
    siddiqsoft::ScopeTrace scope("main");
    process_request();
    return 0;
}
```

---

## Navigation Guide

- [**Features Overview**](features/index.md)
- [**CMake & Integration**](integration/cmake.md)
- [**API Reference**](api/index.md)
- [**Examples & Walkthroughs**](examples/index.md)
