# Project Dependencies

This document is automatically generated from `CMakeLists.txt` files for `ScopeTrace`.

## Dependency Diagram

```mermaid
graph TD
    ScopeTrace["ScopeTrace::ScopeTrace"]

    subgraph Test["Test Dependencies (Optional)"]
        GOOGLETEST["googletest v1.17.0"]
    end

    ScopeTrace -. "BUILD_TESTS=ON" .-> GOOGLETEST
```

## Dependency Breakdown

| Dependency | Repository / Target | Version | Type | Scope / Platform |
| :--- | :--- | :--- | :--- | :--- |
| **googletest** | [`google/googletest`](https://github.com/google/googletest) | v1.17.0 | `CPM` | Test Target Only (`BUILD_TESTS=ON`) |
