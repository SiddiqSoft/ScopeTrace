# ScopeTrace

[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.ScopeTrace?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=1)
[![GitHub release](https://img.shields.io/github/v/release/SiddiqSoft/ScopeTrace)](https://github.com/SiddiqSoft/ScopeTrace/releases)
[![NuGet](https://img.shields.io/nuget/v/SiddiqSoft.ScopeTrace)](https://www.nuget.org/packages/SiddiqSoft.ScopeTrace/)
[![License](https://img.shields.io/github/license/SiddiqSoft/ScopeTrace)](https://github.com/SiddiqSoft/ScopeTrace/blob/main/LICENSE)

`siddiqsoft::ScopeTrace` is a modern, lightweight, header-only C++23 RAII scope logger designed for performance profiling and scope execution tracing.

- **RAII Scope Timing**: Automatic duration measurement upon scope exit.
- **`std::source_location` Integration**: Capture file, line, and function automatically.
- **Nesting Level Tracking**: Indents nested scope execution trees dynamically using thread-local depth counter.
- **Structured Scope Logging**: Specialized `msg()`, `warn()`, `err()`, and `exp()` methods for formatted console logging with ANSI colors.
- **String Formatting & Stream Support**: Native `to_string()` formatting and `operator<<` stream insertion support.

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
    siddiqsoft::ScopeTrace inner("sub_task");
    inner.msg("Processing items...");
    // Perform work...
}

int main()
{
    siddiqsoft::ScopeTrace scope("main");
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
