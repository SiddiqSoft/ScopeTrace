# Basic Usage Example

This example demonstrates how to use `siddiqsoft::ScopeTrace` for scope tracing, nested scope timing (`sub_scope()`), info, trace, warning, and error logging, exception capture, and string formatting.

```cpp
#include <iostream>
#include <thread>
#include <stdexcept>
#include <siddiqsoft/ScopeTrace.hpp>

void worker()
{
    // Create nested scope from process singleton
    auto scope = siddiqsoft::ScopeTrace::GetInstance().sub_scope("worker", siddiqsoft::LogLevel::trace);
    scope.info("Worker starting task with ID={}", 42);
    scope.trace("Low-level trace details for worker setup");

    // Create a child nested scope ("worker/subtask") with warning threshold
    auto sub = scope.sub_scope("subtask", siddiqsoft::LogLevel::warning);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    sub.warn("Resource usage reached {}%", 85);
}

void perform_operation()
{
    auto scope = siddiqsoft::ScopeTrace::GetInstance().sub_scope("perform_operation", siddiqsoft::LogLevel::trace);
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
    // Obtain process-wide singleton ScopeTrace instance
    auto& scope = siddiqsoft::ScopeTrace::GetInstance("main", siddiqsoft::LogLevel::trace);
    scope.info("Application initialization complete");

    perform_operation();

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
    <span class="terminal-title" style="color: #8b949e; font-size: 12px; margin-left: 6px; font-family: sans-serif;">Console Output (std::cerr)</span>
  </div>
  <pre class="terminal-body" style="padding: 14px 18px; color: #e6edf3; font-size: 13px; line-height: 1.65; overflow-x: auto; margin: 0; background: transparent;"><span style="color: #8b949e;">2026-08-15T20:49:27.060564Z</span>|[<span style="color: #e6edf3;">info  </span>]|<span style="color: #8b949e;">main</span>|Application initialization complete
<span style="color: #8b949e;">2026-08-15T20:49:27.060600Z</span>|[<span style="color: #e6edf3;">info  </span>]|<span style="color: #8b949e;">main/worker</span>|Worker starting task with ID=42
<span style="color: #8b949e;">2026-08-15T20:49:27.060620Z</span>|[<span style="color: #58a6ff;">trace </span>]|<span style="color: #8b949e;">main/worker</span>|<span style="color: #58a6ff;">Low-level trace details for worker setup</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.070800Z</span>|[<span style="background-color: #ffd700; color: #000000; font-weight: 600; padding: 0 3px;">warn  </span>]|<span style="color: #8b949e;">main/worker/subtask</span>|<span style="color: #ffd700;">Resource usage reached 85%</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.070850Z</span>|[<span style="color: #8b949e;">debug </span>]|<span style="color: #8b949e;">main/worker/subtask</span>|<span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">10210us</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.070900Z</span>|[<span style="color: #8b949e;">debug </span>]|<span style="color: #8b949e;">main/worker</span>|<span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">10310us</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.071000Z</span>|[<span style="background-color: #da3633; color: #ffffff; font-weight: 600; padding: 0 3px;">except</span>]|<span style="color: #8b949e;">main/perform_operation</span>|<span style="color: #ff7b72;"><b>std::runtime_error</b> - <i>Simulated failure in sub-system</i> - Error encountered during operation execution</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.071050Z</span>|[<span style="color: #8b949e;">debug </span>]|<span style="color: #8b949e;">main/perform_operation</span>|<span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">10500us</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.071120Z</span>|[<span style="color: #8b949e;">debug </span>]|<span style="color: #8b949e;">main</span>|<span style="color: #8b949e;">COMPLETED: time:</span><span style="color: #3fb950; font-weight: 600;">10620us</span></pre>
</div>

