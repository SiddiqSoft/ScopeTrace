# scopelog

[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.scopelog?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=1)
[![GitHub release](https://img.shields.io/github/v/release/SiddiqSoft/scopelog)](https://github.com/SiddiqSoft/scopelog/releases)
[![NuGet](https://img.shields.io/nuget/v/SiddiqSoft.scopelog)](https://www.nuget.org/packages/SiddiqSoft.scopelog/)
[![License](https://img.shields.io/github/license/SiddiqSoft/scopelog)](https://github.com/SiddiqSoft/scopelog/blob/main/LICENSE)

`siddiqsoft::scopelog` is a modern, lightweight, header-only C++23 RAII scope logger designed for performance profiling and scope execution tracing.

- **RAII Scope Timing**: Automatic duration measurement upon scope exit.
- **`std::source_location` Integration**: Capture file, line, and function automatically.
- **Nesting Level Tracking**: Indents nested scope execution trees dynamically.
- **Flexible Logging Sinks**: Thread-safe global callback or per-instance logger callbacks.
- **`std::format` & Stream Support**: Native `std::formatter` specialization and `operator<<` support.

---

## 📚 Documentation Site Links

For full detailed documentation, integration guides, and API specifications, visit our MkDocs documentation site:

- [**Features Overview**](https://SiddiqSoft.github.io/scopelog/features/)
- [**Integration & CMake Guide**](https://SiddiqSoft.github.io/scopelog/integration/cmake/)
- [**Dependency Graph**](https://SiddiqSoft.github.io/scopelog/integration/dependencies/)
- [**API Reference**](https://SiddiqSoft.github.io/scopelog/api/)
- [**Examples & Walkthroughs**](https://SiddiqSoft.github.io/scopelog/examples/)

---

## ⚡ Quick Start

```cpp
#include <iostream>
#include <siddiqsoft/scopelog.hpp>

int main()
{
    // Register global scope logging handler
    siddiqsoft::scopelog::set_global_callback([](const siddiqsoft::scopelog& log) {
        std::cout << log << std::endl;
    });

    siddiqsoft::scopelog scope("main");

    {
        siddiqsoft::scopelog inner("sub_task");
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

CPMAddPackage("gh:SiddiqSoft/scopelog#v1.0.0")

target_link_libraries(your_target PRIVATE siddiqsoft::scopelog)
```

For more details, see the [CMake & Integration Documentation](https://SiddiqSoft.github.io/scopelog/integration/cmake/).

---

## 🛠️ Requirements & Building

- **C++ Compiler**: C++23 compliant compiler (MSVC 2022 v17.10+, GCC 13+, Clang 17+).
- **CMake**: Version >= 3.29.

### Preset Build & Test

```bash
# Configure using Apple-Debug / Linux-GCC-Debug / Windows-x64-Debug preset
cmake --preset Darwin

# Build test binaries
cmake --build --preset Darwin

# Execute unit tests
ctest --preset Darwin
```

---

## 📄 License

Distributed under the [BSD 3-Clause License](LICENSE).
Copyright (c) 2026 Siddiq Software LLC.
