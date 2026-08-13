# Project Dependencies

This document is automatically generated from `CMakeLists.txt` files for `scopelog`.

## Dependency Diagram

```mermaid
graph TD
    scopelog["scopelog::scopelog"]

    subgraph Test["Test Dependencies (Optional)"]
        GOOGLETEST["googletest v1.17.0"]
    end

    scopelog -. "BUILD_TESTS=ON" .-> GOOGLETEST
```

## Dependency Breakdown

| Dependency | Repository / Target | Version | Type | Scope / Platform |
| :--- | :--- | :--- | :--- | :--- |
| **googletest** | [`google/googletest`](https://github.com/google/googletest) | v1.17.0 | `CPM` | Test Target Only (`BUILD_TESTS=ON`) |
