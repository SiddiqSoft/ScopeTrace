# Scope Logging

## How RAII Scope Logging Works

`siddiqsoft::scopelog` uses the C++ RAII pattern to measure duration and emit log entries when execution leaves a block.

```cpp
#include <siddiqsoft/scopelog.hpp>

void compute()
{
    siddiqsoft::scopelog scope("compute_operation");
    // Code block execution...
} // Destruction triggers log callback automatically
```

## Source Location

Using `std::source_location::current()`, `scopelog` records:
- Source file path
- Line number
- Enclosing function signature

## Nesting Depth Tracking

`scopelog` automatically tracks nested scopes using a thread-local counter, producing formatted visual indentation for hierarchical logs:

```text
[main.cpp:12] compute_operation took 150us
  [main.cpp:18] sub_stage took 40us
```
