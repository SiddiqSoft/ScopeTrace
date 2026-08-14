# Quick Reference

## Cheat Sheet

### Scope Instantiation
```cpp
// Explicit scope label
siddiqsoft::ScopeTrace scope("step_name");

// Default scope label (extracts plain function name, or "GLOBAL" if declared in global scope)
siddiqsoft::ScopeTrace scope;

// Nested scope creation (automatically formatted as "<parent_name>-<child_name>")
auto inner = scope.nest("sub_task");
```

### Structured Console Logging
```cpp
// Info log (active in DEBUG / _DEBUG builds)
scope.info("Processing item {} of {}", current, total);

// Trace log (light gray output, active in DEBUG_TRACE builds)
scope.trace("Low-level trace data: addr={:p}", ptr);

// Warning log (yellow output, active in all build modes)
scope.warn("Cache miss for key: {}", key);

// Error log (red output, active in all build modes)
scope.err("Failed to open connection to host: {}", host);

// Unified Error & Throw shortcut (orange output, active in all build modes)
scope.err_throw<std::runtime_error>("Fatal database connection timeout: {}s", timeout);

// Exception log shortcuts (active in all build modes)
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
