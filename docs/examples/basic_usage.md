# Basic Usage Example

This example demonstrates how to use `siddiqsoft::ScopedDebugLog` for scope tracing, nested scope timing, warning and error logging, exception capture, and string formatting.

```cpp
#include <iostream>
#include <thread>
#include <stdexcept>
#include <siddiqsoft/ScopedDebugLog.hpp>

void worker()
{
    siddiqsoft::ScopedDebugLog scope("worker");
    scope.msg("Worker starting task with ID={}", 42);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    scope.warn("Resource usage reached {}%", 85);
}

void perform_operation()
{
    siddiqsoft::ScopedDebugLog scope("perform_operation");
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
    siddiqsoft::ScopedDebugLog scope("main");
    scope.msg("Application initialization complete");

    perform_operation();

    // Stream formatting example
    std::cout << "Scope summary: " << scope << std::endl;

    return 0;
}
```
