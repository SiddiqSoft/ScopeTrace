# Quick Reference

## Cheat Sheet

### Basic Instantiation
```cpp
siddiqsoft::ScopedDebugLog scope("step_name");
```

### Instance Callback
```cpp
siddiqsoft::ScopedDebugLog scope([](const siddiqsoft::ScopedDebugLog& log) {
    std::cout << log.to_string() << "\n";
}, "custom_step");
```

### Global Callback
```cpp
siddiqsoft::ScopedDebugLog::set_global_callback([](const siddiqsoft::ScopedDebugLog& log) {
    spdlog::info("{}", log);
});
```

### Reset Global Callback
```cpp
siddiqsoft::ScopedDebugLog::reset_global_callback();
```
