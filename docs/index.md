# scopelog

[![Build Status](https://dev.azure.com/siddiqsoft/siddiqsoft/_apis/build/status/SiddiqSoft.scopelog?branchName=main)](https://dev.azure.com/siddiqsoft/siddiqsoft/_build/latest?definitionId=1)
[![GitHub release](https://img.shields.io/github/v/release/SiddiqSoft/scopelog)](https://github.com/SiddiqSoft/scopelog/releases)
[![NuGet](https://img.shields.io/nuget/v/SiddiqSoft.scopelog)](https://www.nuget.org/packages/SiddiqSoft.scopelog/)
[![License](https://img.shields.io/github/license/SiddiqSoft/scopelog)](https://github.com/SiddiqSoft/scopelog/blob/main/LICENSE)

`siddiqsoft::scopelog` is a lightweight, zero-overhead C++23 header-only RAII scope logger. It automatically captures function location (`std::source_location`), elapsed duration, nesting depth, and dispatches log messages to instance, global, or stream sinks.

---

## Key Features

- **RAII Scope Timing**: Automatic duration measurement upon scope exit.
- **`std::source_location` Integration**: Capture file, line, and function name automatically.
- **Nesting Level Tracking**: Indents nested scope execution trees dynamically.
- **Flexible Logging Sinks**: Thread-safe global callback or per-instance logger callbacks.
- **`std::format` & Stream Support**: Native `std::formatter` specialization and `operator<<` support.
- **Exception Safe**: Guaranteed `noexcept` destruction swallows callback errors safely.

---

## Quick Start

```cpp
#include <iostream>
#include <siddiqsoft/scopelog.hpp>

void process_data()
{
    // Global sink registration
    siddiqsoft::scopelog::set_global_callback([](const siddiqsoft::scopelog& log) {
        std::clog << log << std::endl;
    });

    siddiqsoft::scopelog scope("process_data");

    {
        siddiqsoft::scopelog inner("sub-task");
        // ... perform work ...
    }
}
```

---

## Requirements Matrix

| Operating System | Compiler Requirement | C++ Standard | Build System |
| :--- | :--- | :--- | :--- |
| **Windows 10/11** | MSVC 2022 (v17.10+) | C++23 (`/std:c++latest`) | CMake >= 3.29, Ninja |
| **Linux (Ubuntu 22.04+)** | GCC 13+ or Clang 17+ | C++23 (`-std=c++23`) | CMake >= 3.29, Ninja |
| **macOS (Darwin)** | Apple Clang 15+ / Homebrew LLVM | C++23 (`-std=c++23`) | CMake >= 3.29, Ninja |
