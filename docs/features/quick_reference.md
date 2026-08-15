# Quick Reference

## Cheat Sheet

### Scope Instantiation & Log Level Threshold
```cpp
// Explicit scope label with log level threshold (e.g. LogLevel::debug)
siddiqsoft::ScopeTrace scope("step_name", siddiqsoft::LogLevel::debug);

// Default scope label (defaults to plain function name, LogLevel::critical threshold)
siddiqsoft::ScopeTrace scope;

// Update threshold dynamically
scope.set_level(siddiqsoft::LogLevel::trace);

// Nested scope creation (automatically formatted as "<parent_name>-<child_name>")
auto inner = scope.nest("sub_task", siddiqsoft::LogLevel::info);
```

### Structured Console Logging & Colors
```cpp
// Trace log (dark blue output, active when threshold >= LogLevel::trace)
// RECOMMENDED FOR HIGH-VOLUME I/O: Socket reads/writes, packet dumps, buffer transfers
scope.trace("Socket read {} bytes from {}:{:d}", bytes, host, port);

// Debug log (light gray output, active when threshold >= LogLevel::debug, excludes trace)
scope.debug("Debugging state: count={}", count);

// Info log (default/neutral output, active when threshold >= LogLevel::info)
scope.info("Processing item {} of {}", current, total);

// Warning log (dark yellow/gold output, active when threshold >= LogLevel::warning)
scope.warn("Cache miss for key: {}", key);

// Error log (orange output, always logged regardless of threshold)
scope.err("Failed to open connection to host: {}", host);

// Unified Error & Throw shortcut (orange output, always logged)
scope.err_throw<std::runtime_error>("Fatal database connection timeout: {}s", timeout);

// Exception log shortcuts (always logged)
try {
    // ...
} catch (const std::exception& e) {
    // Basic exception log shortcut (logs typeid name and e.what())
    scope.exp(e);

    // Exception log shortcut with custom formatted context
    scope.exp(e, "Operation failed for request ID: {}", req_id);
}
```

#### Terminal Color Output Preview

<div class="terminal-window" style="background-color: #1b1d24; border-radius: 8px; border: 1px solid #2e3240; margin: 1.5rem 0; overflow: hidden; font-family: 'JetBrains Mono', Consolas, monospace;">
  <div class="terminal-header" style="background-color: #242836; padding: 8px 14px; display: flex; align-items: center; gap: 8px; border-bottom: 1px solid #2e3240;">
    <span class="terminal-dot red" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ff5f56; display: inline-block;"></span>
    <span class="terminal-dot yellow" style="width: 12px; height: 12px; border-radius: 50%; background-color: #ffbd2e; display: inline-block;"></span>
    <span class="terminal-dot green" style="width: 12px; height: 12px; border-radius: 50%; background-color: #27c93f; display: inline-block;"></span>
    <span class="terminal-title" style="color: #8b949e; font-size: 12px; margin-left: 6px; font-family: sans-serif;">Console Log Colors</span>
  </div>
  <pre class="terminal-body" style="padding: 14px 18px; color: #e6edf3; font-size: 13px; line-height: 1.65; overflow-x: auto; margin: 0; background: transparent;"><span style="color: #8b949e;">2026-08-15T20:49:27.060564Z</span>  <span style="color: #58a6ff;">[trace ]</span> scope - Socket read 1024 bytes from 127.0.0.1:8080
<span style="color: #8b949e;">2026-08-15T20:49:27.060600Z</span>  <span style="color: #8b949e;">[debug ]</span> scope - Debugging state: count=42
<span style="color: #8b949e;">2026-08-15T20:49:27.060620Z</span>  <span style="color: #e6edf3;">[info  ]</span> scope - Processing item 1 of 100
<span style="color: #8b949e;">2026-08-15T20:49:27.060700Z</span>  <span style="color: #d29922;">[warning]</span> <span style="color: #d29922;">scope - Cache miss for key: user_123</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.060800Z</span>  <span style="color: #ffa657;">[error  ]</span> <span style="color: #ffa657;">scope - Failed to open connection to host: db.internal</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.060900Z</span>  <span style="color: #ff7b72;">[exception]</span> <span style="color: #ff7b72;">scope - <b>std::runtime_error</b> - <i>Fatal database connection timeout: 5s</i> - from:main.cpp@39</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.061000Z</span>  <span style="color: #ff7b72;">[exception]</span> <span style="color: #ff7b72;">scope - <b>std::exception</b> - <i>Device I/O error</i> - Operation failed for request ID: REQ-99</span>
<span style="color: #8b949e;">2026-08-15T20:49:27.061100Z</span>  <span style="color: #8b949e;">[debug ]</span> scope - COMPLETED: time:<span style="color: #3fb950; font-weight: 600;">540us</span></pre>
</div>

### Formatting & Streaming
```cpp
// Format as std::string (includes ISO 8601 UTC timestamp and location)
std::string formatted = scope.to_string();

// Stream insertion operator
std::cout << scope << std::endl;
```

### Scope Metrics & Nesting
```cpp
// Elapsed time since scope entry
auto duration = scope.elapsed();

// Instance nesting depth level
size_t level = scope.depth();

// Current thread nesting depth counter
size_t thread_depth = siddiqsoft::ScopeTrace::current_depth();

// ISO 8601 UTC timestamp string
std::string ts = siddiqsoft::ScopeTrace::current_timestamp();

// Extract plain function name matching __func__
std::string_view func = scope.function_name();
```
