# CMake & CPM Integration

## Using CPM.cmake (Recommended)

Add `scopelog` to your CMake project using CPM.cmake:

```cmake
include(CPM.cmake)

CPMAddPackage("gh:SiddiqSoft/scopelog#v1.0.0")

target_link_libraries(your_target PRIVATE siddiqsoft::scopelog)
```

## Using Git Submodules

```cmake
add_subdirectory(third_party/scopelog)

target_link_libraries(your_target PRIVATE siddiqsoft::scopelog)
```
