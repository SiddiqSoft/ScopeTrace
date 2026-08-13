if(${${PROJECT_NAME}_BUILD_TESTS})
    set(TESTPROJ ${PROJECT_NAME}_tests)

    cpmaddpackage("gh:google/googletest#v1.17.0")

    add_executable(${TESTPROJ})

    target_sources(${TESTPROJ}
                    PRIVATE
                    ${PROJECT_SOURCE_DIR}/tests/test_ScopedDebugLog.cpp)

    include(GoogleTest)
    enable_testing()

    target_compile_features(${TESTPROJ} PRIVATE cxx_std_23)
    target_compile_options(${TESTPROJ}
                            PRIVATE
                            $<$<CXX_COMPILER_ID:MSVC>:/std:c++latest> )

    target_link_libraries(${TESTPROJ} PRIVATE GTest::gtest_main ScopedDebugLog)

    gtest_discover_tests(${TESTPROJ} XML_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/tests/results")

    include(CTest)
    message(STATUS "  Finished configuring for ${PROJECT_NAME} -- ${TESTPROJ} BUILD_TESTS = ${${PROJECT_NAME}_BUILD_TESTS}")
endif()
