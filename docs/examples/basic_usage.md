# Basic Usage Example

```cpp
#include <iostream>
#include <thread>
#include <siddiqsoft/ScopedDebugLog.hpp>

void worker()
{
    siddiqsoft::ScopedDebugLog scope("worker");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

int main()
{
    siddiqsoft::ScopedDebugLog::set_global_callback([](const siddiqsoft::ScopedDebugLog& log) {
        std::cout << log << std::endl;
    });

    siddiqsoft::ScopedDebugLog scope("main");
    worker();
    return 0;
}
```
