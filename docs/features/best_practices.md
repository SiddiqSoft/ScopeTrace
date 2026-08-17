# ScopeTrace Best Practices Guide

This guide outlines recommended usage patterns and architectural best practices when integrating `siddiqsoft::ScopeTrace` into your C++ applications.

---

## 1. High-Frequency I/O Logging (`scope.trace`)

High-volume operations — such as socket data transfers, stream packet dumps, HTTP body payload dumps, file stream buffer reads/writes, or inner loop iterations — should **always** use `trace_level::trace` (`scope.trace(...)`).

```cpp
void read_socket(int fd)
{
    auto scope = siddiqsoft::ScopeTrace::GetInstance().sub_scope("read_socket", siddiqsoft::LogLevel::info);

    char buffer[1024];
    ssize_t bytes = ::read(fd, buffer, sizeof(buffer));

    // Zero-allocation bypass when m_log_level < trace
    scope.trace("Read {} bytes from fd={}", bytes, fd);
}
```

> **Rationale:**
> When running with standard logging thresholds (`LogLevel::info` or `LogLevel::debug`), all `scope.trace(...)` invocations are bypassed instantly via a zero-allocation threshold check (`level <= m_log_level`). This prevents string formatting overhead and keeps your log buffers clean while maintaining granular diagnostic capabilities when needed.

---

## 2. Dynamic Nesting with `scope.sub_scope()`

Instead of instantiating independent loggers in child helper functions or internal blocks, use `scope.sub_scope()` to pass down context and automatically form parent-child labels (`"parent/child"`):

```cpp
void process_order(const std::string& order_id)
{
    auto scope = siddiqsoft::ScopeTrace::GetInstance().sub_scope("process_order", siddiqsoft::LogLevel::info);

    {
        auto stage1 = scope.sub_scope("validate_inventory");
        stage1.info("Checking stock levels for order: {}", order_id);
    }

    {
        auto stage2 = scope.sub_scope("charge_payment");
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
    auto scope = siddiqsoft::ScopeTrace::GetInstance().sub_scope("load_config", siddiqsoft::LogLevel::info);

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

When spawning asynchronous worker threads or thread pool tasks, use `ScopeTrace::GetInstance().sub_scope("worker_thread")` at the entry point of the thread function so that async logs maintain parentage-based depth indentation:

```cpp
std::thread worker_thread([order_id]() {
    auto thread_scope = siddiqsoft::ScopeTrace::GetInstance().sub_scope("worker_thread", siddiqsoft::LogLevel::info);
    thread_scope.info("Worker thread initialized for order: {}", order_id);
    // Asynchronous task logic...
});
```

---

## 5. Process Singleton Access & Protected Constructors

### Process Singleton (`GetInstance`)
`ScopeTrace` is designed around a single process-wide root instance managed via `ScopeTrace::GetInstance(...)`. All client code must obtain instances through `ScopeTrace::GetInstance(...)` or `parent.sub_scope(...)`.

### Protected Direct Constructors
Direct constructors `ScopeTrace(...)` are `protected:`. Direct stack instantiation (e.g. `siddiqsoft::ScopeTrace scope;`) is disabled in client code and will fail to compile.

### Copy & Move Prevention
`ScopeTrace` explicitly **deletes all copy and move constructors and assignment operators** (`ScopeTrace(const ScopeTrace&) = delete`, `ScopeTrace(ScopeTrace&&) = delete`). Instances cannot be copied or moved.

---

## 6. Caching Logger References (`GetInstance()` Performance Best Practice)

### Recommended Pattern: Module/File Static Reference

While `ScopeTrace::GetInstance(...)` can be called from anywhere, invoking `GetInstance()` repeatedly on every single log call incurs small runtime overheads:
- **Atomic Guard Check**: C++11 "Magic Statics" emit an atomic memory barrier check on every call to verify singleton initialization.
- **Parameter Evaluation**: Default argument `std::source_location::current()` constructs a temporary stack object on every call.
- **Scope Name Check**: Internal string assignment check (`if (!sn.empty())`) evaluates on every call.

To eliminate these overheads, **cache a static reference per file or module**:

```cpp
// In your cpp file or module scope:
static auto& Log = siddiqsoft::ScopeTrace::GetInstance("MY_MODULE", siddiqsoft::LogLevel::info);

void process_data()
{
    // Fast direct call bypassing GetInstance() atomic guard checks & argument evaluation
    Log.info("Data processing started");

    auto scope = Log.sub_scope("inner_task");
    scope.trace("Tracing details...");
}
```

> **Performance Advantage:**  
> The `GetInstance()` initialization, atomic barrier check, string assignment, and parameter evaluation happen **once** when the translation unit loads. Subsequent `Log.info(...)` or `Log.sub_scope(...)` calls execute via direct reference lookup without function wrapper overhead.

### Thread-Local Instances for Worker Threads

For high-throughput worker threads that require independent local threshold filtering or isolated thread scopes, cache a `thread_local` reference:

```cpp
thread_local auto& t_Log = siddiqsoft::ScopeTrace::GetInstance("WORKER", siddiqsoft::LogLevel::debug);

void worker_loop()
{
    t_Log.debug("Worker iteration starting...");
}
```

