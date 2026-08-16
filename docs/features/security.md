# Security & Exception Model

## Exception Safety

- **`noexcept` Destructor Guarantee**: `siddiqsoft::ScopeTrace::~ScopeTrace()` is explicitly declared `noexcept` to ensure safety during stack unwinding.
- **Exception Logging Utility**: The `scope.exp(const std::exception& e)` method safe-prints exception types and messages to `std::cerr` without throwing exceptions.

## Thread Safety & Performance

- **Zero-Lock Nesting Depth**: Scope nesting depth is derived directly from parentage (`parent.depth() + 1`), requiring zero lock contention or synchronization overhead across concurrent threads.
- **Header-Only Efficiency**: Small, lightweight header-only implementation with minimal overhead.

## Cross-Platform & Windows Compatibility

- **Path Separators**: `extract_file_name()` handles both Unix (`/`) and Windows (`\`) path separators seamlessly.
- **Automatic Windows Console Color Support**: On Windows, `GetInstance()` automatically enables Virtual Terminal Processing (`ENABLE_VIRTUAL_TERMINAL_PROCESSING`) on `STD_ERROR_HANDLE`, guaranteeing native ANSI color escape code and reverse video tag rendering across `cmd.exe`, PowerShell, and Windows Terminal.
- **MSVC & Clang-CL Safe**: Protected against Windows macro pollution (`WIN32_LEAN_AND_MEAN` and `NOMINMAX`). Fully compatible with MSVC 2022 and Clang-CL with `/std:c++23` / `/std:c++latest`.
