if(${${PROJECT_NAME}_BUILD_TESTS})
    set(TESTPROJ ${PROJECT_NAME}_tests)

    cpmaddpackage("gh:google/googletest#v1.17.0")

    add_executable(${TESTPROJ})

    target_sources(${TESTPROJ}
                    PRIVATE
                    ${PROJECT_SOURCE_DIR}/tests/test_ScopeTrace.cpp)

    if( CMAKE_BUILD_TYPE MATCHES [Dd][Ee][Bb][Uu][Gg])
        message(STATUS "Enable DEBUG pre-processor macros.. cmake_build_type:${CMAKE_BUILD_TYPE}")
        target_compile_definitions(${TESTPROJ} PRIVATE DEBUG=1 _DEBUG=1)
    endif()

    include(GoogleTest)
    enable_testing()

    target_compile_features(${TESTPROJ} PRIVATE cxx_std_${CMAKE_CXX_STANDARD})
    target_compile_options(${TESTPROJ}
                            PRIVATE
                            $<$<CXX_COMPILER_ID:MSVC>:/std:c++latest> )

    target_link_libraries(${TESTPROJ} PRIVATE GTest::gtest_main ${PROJECT_NAME})

    gtest_discover_tests(${TESTPROJ} XML_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/tests/results")

    include(CTest)
    message(STATUS "  Finished configuring for ${PROJECT_NAME} -- ${TESTPROJ} BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
endif()
