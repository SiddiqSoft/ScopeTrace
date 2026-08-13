# ScopedDebugLog

[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.ScopedDebugLog?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=1)
[![GitHub release](https://img.shields.io/github/v/release/SiddiqSoft/ScopedDebugLog)](https://github.com/SiddiqSoft/ScopedDebugLog/releases)
[![NuGet](https://img.shields.io/nuget/v/SiddiqSoft.ScopedDebugLog)](https://www.nuget.org/packages/SiddiqSoft.ScopedDebugLog/)
[![License](https://img.shields.io/github/license/SiddiqSoft/ScopedDebugLog)](https://github.com/SiddiqSoft/ScopedDebugLog/blob/main/LICENSE)

`siddiqsoft::ScopedDebugLog` is a header-only C++23 RAII scope logger designed for execution tracing, performance measurement, and nesting depth visualization.

---

## Key Highlights

- **Zero-Boilerplate Tracing**: Automatically record function name, file path, and line numbers using `std::source_location`.
- **Nesting Level Tracking**: Indents nested scope execution trees dynamically using thread-local scope depth.
- **Structured Console Logging**: Methods for `msg()`, `warn()`, `err()`, and `exp()` with depth indentation and ANSI colors.
- **C++23 Native Support**: Leverages `std::format`, `std::println`, and `std::ostream`.

---

## Quick Example

```cpp
#include <iostream>
#include <siddiqsoft/ScopedDebugLog.hpp>

void process_request()
{
    siddiqsoft::ScopedDebugLog scope("process_request");
    scope.msg("Parsing incoming payload...");
    // Work executed here...
}

int main()
{
    siddiqsoft::ScopedDebugLog scope("main");
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
