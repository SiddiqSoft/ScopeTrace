# Quick Reference

## Cheat Sheet

### Scope Instantiation
```cpp
siddiqsoft::ScopedDebugLog scope("step_name");
```

### Structured Console Logging
```cpp
// Info log (in debug builds)
scope.msg("Processing item {} of {}", current, total);

// Warning log (yellow output)
scope.warn("Cache miss for key: {}", key);

// Error log (red output)
scope.err("Failed to open connection to host: {}", host);

// Exception log (red bold output)
try {
    // ...
} catch (const std::exception& e) {
    scope.exp(e);
}
```

### Formatting & Streaming
```cpp
// Format as std::string
std::string formatted = scope.to_string();

// Stream insertion operator
std::cout << scope << std::endl;
```

### Scope Metrics & Nesting
```cpp
// Elapsed time
auto duration = scope.elapsed();

// Instance nesting depth
size_t level = scope.depth();

// Current thread nesting depth counter
size_t thread_depth = siddiqsoft::ScopedDebugLog::current_depth();
```
