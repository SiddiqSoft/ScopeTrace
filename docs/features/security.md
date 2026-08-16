# Security & Exception Model

## Exception Safety

- **`noexcept` Destructor Guarantee**: `siddiqsoft::ScopeTrace::~ScopeTrace()` is explicitly declared `noexcept` to ensure safety during stack unwinding.
- **Exception Logging Utility**: The `scope.exp(const std::exception& e)` method safe-prints exception types and messages to `std::cerr` without throwing exceptions.

## Thread Safety & Performance

- **Zero-Lock Nesting Depth**: Scope nesting depth is derived directly from parentage (`parent.depth() + 1`), requiring zero lock contention or synchronization overhead across concurrent threads.
- **Header-Only Efficiency**: Small, lightweight header-only implementation with minimal overhead.
