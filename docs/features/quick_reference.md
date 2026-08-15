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
// Trace log (light gray output, active when threshold >= LogLevel::trace)
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
std::string_view func = scope.func_name();
```
