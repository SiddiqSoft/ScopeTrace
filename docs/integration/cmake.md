# CMake & CPM Integration

## Using CPM.cmake (Recommended)

Add `ScopedDebugLog` to your CMake project using CPM.cmake:

```cmake
include(CPM.cmake)

CPMAddPackage("gh:SiddiqSoft/ScopedDebugLog#v1.0.0")

target_link_libraries(your_target PRIVATE siddiqsoft::ScopedDebugLog)
```

## Using Git Submodules

```cmake
add_subdirectory(third_party/ScopedDebugLog)

target_link_libraries(your_target PRIVATE siddiqsoft::ScopedDebugLog)
```
