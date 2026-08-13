# ScopeTrace

![](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status%2FSiddiqSoft.ScopeTrace?repoName=SiddiqSoft%2FScopeTrace&branchName=master)
![](https://img.shields.io/nuget/v/SiddiqSoft.ScopeTrace)
![](https://img.shields.io/github/v/tag/SiddiqSoft/ScopeTrace)
![](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/34)
[![NuGet](https://img.shields.io/nuget/v/SiddiqSoft.ScopeTrace)](https://www.nuget.org/packages/SiddiqSoft.ScopeTrace/)

`siddiqsoft::ScopeTrace` is a modern, lightweight, header-only C++23 RAII scope logger designed for performance profiling and scope execution tracing.

- **RAII Scope Timing**: Automatic duration measurement upon scope exit.
- **`std::source_location` Integration**: Capture file, line, and function automatically.
- **Nesting Level Tracking**: Indents nested scope execution trees dynamically using thread-local depth counter.
- **Structured Scope Logging**: Specialized `msg()`, `warn()`, `err()`, and `exp()` methods for formatted console logging with ANSI colors.
- **String Formatting & Stream Support**: Native `to_string()` formatting and `operator<<` stream insertion support.

---

## Motivation

How many of us have had to write

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
        siddiqsoft::ScopeTrace scope; // automatically defaults scope name to plain __func__ ("foo")

        try {
            siddiqsoft::ScopeTrace inner("foo-Nested"); // explicit inner scope label

            inner.msg("From the inner scope line: {}", __LINE__);
            throw std::runtime_error(std::format("Deliberate error from line: {}", __LINE__));
        }
        catch (const std::exception& e) {
            scope.exp(e);
        }
    }
    ```

    === "ScopeTrace output in `DEBUG` mode"

        Note the nesting and color output
        ```
        foo-Nested - From the inner scope line: 66
        foo-Nested - COMPLETED - time:71us
        foo - St13runtime_error - Deliberate error from line: 67
        foo - COMPLETED - time:138us
        ```

    === "ScopeTrace output in `RELEASE` mode"

        ```
        foo - St13runtime_error - Deliberate error from line: 67
        ```

---

## 📚 Documentation Site Links

For full detailed documentation, integration guides, and API specifications, visit our MkDocs documentation site:

- [**Features Overview**](https://SiddiqSoft.github.io/ScopeTrace/features/)
- [**Integration & CMake Guide**](https://SiddiqSoft.github.io/ScopeTrace/integration/cmake/)
- [**Dependency Graph**](https://SiddiqSoft.github.io/ScopeTrace/integration/dependencies/)
- [**API Reference**](https://SiddiqSoft.github.io/ScopeTrace/api/)
- [**Examples & Walkthroughs**](https://SiddiqSoft.github.io/ScopeTrace/examples/)

---

## ⚡ Quick Start

```cpp
#include <iostream>
#include <siddiqsoft/ScopeTrace.hpp>

void sub_task()
{
    siddiqsoft::ScopeTrace inner;
    inner.msg("Processing items...");
    // Perform work...
}

int main()
{
    siddiqsoft::ScopeTrace scope;

    scope.msg("Starting application execution");

    try {
        sub_task();
    }
    catch (const std::exception& e) {
        scope.exp(e);
    }

    return 0;
}
```

---

## 🔍 Function Name Extraction (`__func__`)

`ScopeTrace` provides built-in utilities to extract the clean function name matching `__func__` from verbose `std::source_location::function_name()` signatures:

```cpp
void MyClass::process_data()
{
    siddiqsoft::ScopeTrace scope;

    // Returns "process_data"
    std::string_view name1 = scope.function_name();
    std::string_view name2 = scope.func_name(); // Alias

    // Static helper for any signature string
    std::string_view name3 = siddiqsoft::ScopeTrace::extract_func_name("virtual void MyClass::process_data(int) const");
}
```

---

## 📦 Installation & Integration

Integrate via [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake):

```cmake
include(CPM.cmake)

CPMAddPackage("gh:SiddiqSoft/ScopeTrace#v1.0.0")

target_link_libraries(your_target PRIVATE siddiqsoft::ScopeTrace)
```

For more details, see the [CMake & Integration Documentation](https://SiddiqSoft.github.io/ScopeTrace/integration/cmake/).

---

## 🛠️ Requirements & Building

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

## 📄 License

Distributed under the [BSD 3-Clause License](LICENSE).
Copyright (c) 2026 Siddiq Software LLC.
