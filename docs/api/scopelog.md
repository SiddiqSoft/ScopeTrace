# `scopelog` API Reference

Defined in `<siddiqsoft/scopelog.hpp>`

```cpp
namespace siddiqsoft {
    struct scopelog;
    using ScopeLog = scopelog;
}
```

## Public Constructors

### `scopelog(std::string_view msg = "", const std::source_location& sl = std::source_location::current())`
Constructs a scope logger capturing current time and location.

### `scopelog(std::function<void(const scopelog&)>&& callback, std::string_view msg = "", const std::source_location& sl = std::source_location::current())`
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

### `set_global_callback(std::function<void(const scopelog&)> cb)`
Registers global thread-safe callback.

### `reset_global_callback()`
Clears global callback.
