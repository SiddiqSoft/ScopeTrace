# Basic Usage Example

```cpp
#include <iostream>
#include <thread>
#include <siddiqsoft/scopelog.hpp>

void worker()
{
    siddiqsoft::scopelog scope("worker");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
}

int main()
{
    siddiqsoft::scopelog::set_global_callback([](const siddiqsoft::scopelog& log) {
        std::cout << log << std::endl;
    });

    siddiqsoft::scopelog scope("main");
    worker();
    return 0;
}
```
