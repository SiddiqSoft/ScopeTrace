# Basic Usage Example

This example demonstrates how to use `siddiqsoft::ScopeTrace` for scope tracing, nested scope timing (`nest()`), info, trace, warning, and error logging, exception capture, and string formatting.

```cpp
#include <iostream>
#include <thread>
#include <stdexcept>
#include <siddiqsoft/ScopeTrace.hpp>

void worker()
{
    // Default constructor automatically uses plain function name ("worker")
    siddiqsoft::ScopeTrace scope;
    scope.info("Worker starting task with ID={}", 42);
    scope.trace("Low-level trace details for worker setup");

    // Create a child nested scope ("worker-subtask")
    auto sub = scope.nest("subtask");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    sub.warn("Resource usage reached {}%", 85);
}

void perform_operation()
{
    siddiqsoft::ScopeTrace scope;
    worker();

    try {
        throw std::runtime_error("Simulated failure in sub-system");
    }
    catch (const std::exception& e) {
        scope.exp(e);
    }
}

int main()
{
    siddiqsoft::ScopeTrace scope;
    scope.info("Application initialization complete");

    perform_operation();

    // Stream formatting example
    std::cout << "Scope summary: " << scope << std::endl;

    return 0;
}
```
