# ScopeTrace Best Practices Guide

This guide outlines recommended usage patterns and architectural best practices when integrating `siddiqsoft::ScopeTrace` into your C++ applications.

---

## 1. High-Frequency I/O Logging (`scope.trace`)

High-volume operations — such as socket data transfers, stream packet dumps, HTTP body payload dumps, file stream buffer reads/writes, or inner loop iterations — should **always** use `trace_level::trace` (`scope.trace(...)`).

```cpp
void read_socket(int fd)
{
    auto scope = siddiqsoft::ScopeTrace::CreateInstance().nest("read_socket", siddiqsoft::LogLevel::info);

    char buffer[1024];
    ssize_t bytes = ::read(fd, buffer, sizeof(buffer));

    // Zero-allocation bypass when m_log_level < trace
    scope.trace("Read {} bytes from fd={}", bytes, fd);
}
```

> **Rationale:**
> When running with standard logging thresholds (`LogLevel::info` or `LogLevel::debug`), all `scope.trace(...)` invocations are bypassed instantly via a zero-allocation threshold check (`level <= m_log_level`). This prevents string formatting overhead and keeps your log buffers clean while maintaining granular diagnostic capabilities when needed.

---

## 2. Dynamic Nesting with `scope.nest()`

Instead of instantiating independent loggers in child helper functions or internal blocks, use `scope.nest()` to pass down context and automatically form parent-child labels (`"parent/child"`):

```cpp
void process_order(const std::string& order_id)
{
    auto scope = siddiqsoft::ScopeTrace::CreateInstance().nest("process_order", siddiqsoft::LogLevel::info);

    {
        auto stage1 = scope.nest("validate_inventory");
        stage1.info("Checking stock levels for order: {}", order_id);
    }

    {
        auto stage2 = scope.nest("charge_payment");
        stage2.info("Processing payment transaction for order: {}", order_id);
    }
}
```

---

## 3. Unified Error Flow (`err_throw` & `exp`)

Combine error logging and exception throwing into a single step with `err_throw<EX>()`, and capture caught exceptions cleanly with `exp(e)` or `exp(e, fmt)`:

```cpp
void load_configuration(const std::filesystem::path& path)
{
    auto scope = siddiqsoft::ScopeTrace::CreateInstance().nest("load_config", siddiqsoft::LogLevel::info);

    if (!std::filesystem::exists(path)) {
        // Logs orange error entry with caller file:line details and throws std::runtime_error
        scope.err_throw<std::runtime_error>("Configuration file not found: {}", path.string());
    }

    try {
        parse_file(path);
    }
    catch (const std::exception& e) {
        // Logs caught exception type, e.what(), and contextual details
        scope.exp(e, "Failed to parse file at path: {}", path.string());
        throw;
    }
}
```

---

## 4. Asynchronous Worker Nesting

When spawning asynchronous worker threads or thread pool tasks, use `ScopeTrace::CreateInstance().nest("worker_thread")` at the entry point of the thread function so that async logs maintain parentage-based depth indentation:

```cpp
std::thread worker_thread([order_id]() {
    auto thread_scope = siddiqsoft::ScopeTrace::CreateInstance().nest("worker_thread", siddiqsoft::LogLevel::info);
    thread_scope.info("Worker thread initialized for order: {}", order_id);
    // Asynchronous task logic...
});
```

---

## 5. Process Singleton Access & Protected Constructors

### Process Singleton (`CreateInstance`)
`ScopeTrace` is designed around a single process-wide root instance managed via `ScopeTrace::CreateInstance(...)`. All client code must obtain instances through `ScopeTrace::CreateInstance(...)` or `parent.nest(...)`.

### Protected Direct Constructors
Direct constructors `ScopeTrace(...)` are `protected:`. Direct stack instantiation (e.g. `siddiqsoft::ScopeTrace scope;`) is disabled in client code and will fail to compile.

### Copy & Move Prevention
`ScopeTrace` explicitly **deletes all copy and move constructors and assignment operators** (`ScopeTrace(const ScopeTrace&) = delete`, `ScopeTrace(ScopeTrace&&) = delete`). Instances cannot be copied or moved.
