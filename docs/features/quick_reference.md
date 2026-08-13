# Quick Reference

## Cheat Sheet

### Basic Instantiation
```cpp
siddiqsoft::scopelog scope("step_name");
```

### Instance Callback
```cpp
siddiqsoft::scopelog scope([](const siddiqsoft::scopelog& log) {
    std::cout << log.to_string() << "\n";
}, "custom_step");
```

### Global Callback
```cpp
siddiqsoft::scopelog::set_global_callback([](const siddiqsoft::scopelog& log) {
    spdlog::info("{}", log);
});
```

### Reset Global Callback
```cpp
siddiqsoft::scopelog::reset_global_callback();
```
