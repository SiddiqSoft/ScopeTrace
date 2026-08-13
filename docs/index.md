# ScopeTrace

![](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status%2FSiddiqSoft.ScopeTrace?repoName=SiddiqSoft%2FScopeTrace&branchName=master)
[![NuGet](https://img.shields.io/nuget/v/SiddiqSoft.ScopeTrace)](https://www.nuget.org/packages/SiddiqSoft.ScopeTrace/)
![](https://img.shields.io/nuget/v/SiddiqSoft.ScopeTrace)
![](https://img.shields.io/github/v/tag/SiddiqSoft/ScopeTrace)
![](https://img.shields.io/azure-devops/tests/siddiqsoft/siddiqsoft/34)

`siddiqsoft::ScopeTrace` is a header-only C++23 RAII scope logger designed for execution tracing, performance measurement, and nesting depth visualization.

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
