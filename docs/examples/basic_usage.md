# Basic Usage Example

This example demonstrates how to use `siddiqsoft::ScopeTrace` for scope tracing, nested scope timing (`nest()`), info, trace, warning, and error logging, exception capture, and string formatting.

```cpp
#include <iostream>
#include <thread>
#include <stdexcept>
#include <siddiqsoft/ScopeTrace.hpp>

void worker()
{
    // Initialize scope with plain function name ("worker") and trace threshold
    siddiqsoft::ScopeTrace scope("worker", siddiqsoft::LogLevel::trace);
    scope.info("Worker starting task with ID={}", 42);
    scope.trace("Low-level trace details for worker setup");

    // Create a child nested scope ("worker-subtask") with warning threshold
    auto sub = scope.nest("subtask", siddiqsoft::LogLevel::warning);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    sub.warn("Resource usage reached {}%", 85);
}

void perform_operation()
{
    siddiqsoft::ScopeTrace scope("perform_operation", siddiqsoft::LogLevel::trace);
    worker();

    try {
        throw std::runtime_error("Simulated failure in sub-system");
    }
    catch (const std::exception& e) {
        // Exception handler shortcut with custom formatted contextual details
        scope.exp(e, "Error encountered during operation execution");
    }
}

int main()
{
    siddiqsoft::ScopeTrace scope("main", siddiqsoft::LogLevel::trace);
    scope.info("Application initialization complete");

    perform_operation();

    // Stream formatting example
    std::cout << "Scope summary: " << scope << std::endl;

    return 0;
}
```

---

## Sample Console Output & Coloring

Executing the example above outputs formatted log entries to `std::cerr` with dynamic depth spaces, ISO 8601 UTC timestamps, and ANSI colors:

<div class="terminal-window" style="background-color: #1b1d24; border-radius: 8px; border: 1px solid #2e3240; margin: 1.5rem 0; overflow: hidden; font-family: 'JetBrains Mono', Consolas, monospace;">
  <div class="terminal-header" style="background-color: #242836; padding: 8px 14px; display: flex; align-items: center; gap: 8px; border-bottom: 1px solid #2e3240;">
    <span class="terminal-dot red" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ff5f56; display: inline-block;"></span>
    <span class="terminal-dot yellow" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ffbd2e; display: inline-block;"></span>
    <span class="terminal-dot green" style="width: 12px; height: 12px; border-radius: 50%; background-color: #27c93f; display: inline-block;"></span>
    <span class="terminal-title" style="color: #8b949e; font-size: 12px; margin-left: 6px; font-family: sans-serif;">Console Output (std::cerr & std::cout)</span>
  </div>
  <pre class="terminal-body" style="padding: 14px 18px; color: #e6edf3; font-size: 13px; line-height: 1.65; overflow-x: auto; margin: 0; background: transparent;"><span style="color: #6e7681;">  Creating NEW SCOPE main:6</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.060564Z</span>  <span style="color: #e6edf3;">[info  ]</span> main - Application initialization complete
<span style="color: #6e7681;">  Creating NEW SCOPE perform_operation:6</span>
<span style="color: #6e7681;">  Creating NEW SCOPE worker:6</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.060600Z</span>  <span style="color: #e6edf3;">[info  ]</span>     worker - Worker starting task with ID=42
<span style="color: #8b949e;">2026-08-15T20:49:27.060620Z</span>  <span style="color: #58a6ff;">[trace ]</span>     worker - Low-level trace details for worker setup
<span style="color: #6e7681;">  Creating NEW SCOPE worker-subtask:3</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.070800Z</span>  <span style="color: #d29922;">[warning]</span>       <span style="color: #d29922;">worker-subtask - Resource usage reached 85%</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.070850Z</span>  <span style="color: #8b949e;">[debug ]</span>       <span style="color: #8b949e;">worker-subtask - COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">10210us</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.070900Z</span>  <span style="color: #8b949e;">[debug ]</span>     <span style="color: #8b949e;">worker - COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">10310us</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.071000Z</span>  <span style="color: #ff7b72;">[exception]</span>   <span style="color: #ff7b72;">perform_operation - <b>std::runtime_error</b> - <i>Simulated failure in sub-system</i> - Error encountered during operation execution</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.071050Z</span>  <span style="color: #8b949e;">[debug ]</span>   <span style="color: #8b949e;">perform_operation - COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">10500us</span>
Scope summary: 2026-08-15T20:49:27.071100Z [main.cpp:41] main (main) took 10600us
<span style="color: #8b949e;">2026-08-15T20:49:27.071120Z</span>  <span style="color: #8b949e;">[debug ]</span> main - COMPLETED: time:<span style="color: #3fb950; font-weight: 600;">10620us</span></pre>
</div>

