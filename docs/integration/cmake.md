# CMake & CPM Integration

## Using CPM.cmake (Recommended)

Add `ScopeTrace` to your CMake project using CPM.cmake:

```cmake
include(CPM.cmake)

CPMAddPackage("gh:SiddiqSoft/ScopeTrace#v1.0.0")

target_link_libraries(your_target PRIVATE siddiqsoft::ScopeTrace)
```

## Using Git Submodules

```cmake
add_subdirectory(third_party/ScopeTrace)

target_link_libraries(your_target PRIVATE siddiqsoft::ScopeTrace)
```
