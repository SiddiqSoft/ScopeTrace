# ScopedDebugLog

[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.ScopedDebugLog?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=1)
[![GitHub release](https://img.shields.io/github/v/release/SiddiqSoft/ScopedDebugLog)](https://github.com/SiddiqSoft/ScopedDebugLog/releases)
[![NuGet](https://img.shields.io/nuget/v/SiddiqSoft.ScopedDebugLog)](https://www.nuget.org/packages/SiddiqSoft.ScopedDebugLog/)
[![License](https://img.shields.io/github/license/SiddiqSoft/ScopedDebugLog)](https://github.com/SiddiqSoft/ScopedDebugLog/blob/main/LICENSE)

`siddiqsoft::ScopedDebugLog` is a modern, lightweight, header-only C++23 RAII scope logger designed for performance profiling and scope execution tracing.

- **RAII Scope Timing**: Automatic duration measurement upon scope exit.
- **`std::source_location` Integration**: Capture file, line, and function automatically.
- **Nesting Level Tracking**: Indents nested scope execution trees dynamically.
- **Flexible Logging Sinks**: Thread-safe global callback or per-instance logger callbacks.
- **`std::format` & Stream Support**: Native `std::formatter` specialization and `operator<<` support.

---

## 📚 Documentation Site Links

For full detailed documentation, integration guides, and API specifications, visit our MkDocs documentation site:

- [**Features Overview**](https://SiddiqSoft.github.io/ScopedDebugLog/features/)
- [**Integration & CMake Guide**](https://SiddiqSoft.github.io/ScopedDebugLog/integration/cmake/)
- [**Dependency Graph**](https://SiddiqSoft.github.io/ScopedDebugLog/integration/dependencies/)
- [**API Reference**](https://SiddiqSoft.github.io/ScopedDebugLog/api/)
- [**Examples & Walkthroughs**](https://SiddiqSoft.github.io/ScopedDebugLog/examples/)

---

## ⚡ Quick Start

```cpp
#include <iostream>
#include <siddiqsoft/ScopedDebugLog.hpp>

int main()
{
    // Register global scope logging handler
    siddiqsoft::ScopedDebugLog::set_global_callback([](const siddiqsoft::ScopedDebugLog& log) {
        std::cout << log << std::endl;
    });

    siddiqsoft::ScopedDebugLog scope("main");

    {
        siddiqsoft::ScopedDebugLog inner("sub_task");
        // Perform work...
    }
    return 0;
}
```

---

## 📦 Installation & Integration

Integrate via [CPM.cmake](https://github.com/cpm-cmake/CPM.cmake):

```cmake
include(CPM.cmake)

CPMAddPackage("gh:SiddiqSoft/ScopedDebugLog#v1.0.0")

target_link_libraries(your_target PRIVATE siddiqsoft::ScopedDebugLog)
```

For more details, see the [CMake & Integration Documentation](https://SiddiqSoft.github.io/ScopedDebugLog/integration/cmake/).

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
