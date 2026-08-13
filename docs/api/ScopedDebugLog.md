# `ScopedDebugLog` API Reference

Defined in `<siddiqsoft/ScopedDebugLog.hpp>`

```cpp
namespace siddiqsoft {
    struct ScopedDebugLog;
    using scopelog = ScopedDebugLog;
    using ScopeLog = ScopedDebugLog;
}
```

## Public Constructors

### `ScopedDebugLog(std::string_view msg = "", const std::source_location& sl = std::source_location::current())`
Constructs a scope logger capturing current time and location.

### `ScopedDebugLog(std::function<void(const ScopedDebugLog&)>&& callback, std::string_view msg = "", const std::source_location& sl = std::source_location::current())`
Constructs a scope logger with a per-instance destruction callback.

## Member Functions

### `elapsed()`
Returns `std::chrono::system_clock::duration` elapsed since construction.

### `depth()`
Returns scope depth index (0 for top level).

### `location()`
Returns `const std::source_location&`.

### `message()`
Returns `std::string_view` custom message.

### `to_string()`
Formats scope log as `std::string`.

## Static Methods

### `set_global_callback(std::function<void(const ScopedDebugLog&)> cb)`
Registers global thread-safe callback.

### `reset_global_callback()`
Clears global callback.
