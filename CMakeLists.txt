# ============================================================================
# CMake Configuration for SiddiqSoft scopelog (C++23 Header-Only Library)
# ============================================================================

cmake_minimum_required(VERSION 3.29)

# Version detection and CPM dependency helper
include(pack/CMakeCommonHelpers.cmake)

# Project metadata definition
project(scopelog
        VERSION         "${CURRENT_PROJECT_VERSION}"
        LANGUAGES       CXX
        HOMEPAGE_URL    "https://github.com/SiddiqSoft/scopelog"
        DESCRIPTION     "Modern C++ RAII scope logger library with std::source_location, duration tracking, and custom sinks")

# Build options
option(scopelog_BUILD_TESTS "Build tests for scopelog" OFF)
option(CI_BUILDID "Build ID for CI builds" "0.0.0")

# Library definition (Interface / Header-Only)
add_library(scopelog INTERFACE)
add_library(siddiqsoft::scopelog ALIAS scopelog)

# Include directories
target_include_directories(scopelog
    INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

# Compile features
target_compile_features(scopelog INTERFACE cxx_std_23)

# Testing configuration
if(scopelog_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests" AND IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    message(STATUS "scopelog: Building tests (scopelog_BUILD_TESTS=ON)")
    enable_testing()
    add_subdirectory(tests)
endif()
