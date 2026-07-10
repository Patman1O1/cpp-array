#-----------------------------------------------------------------------------------------------------------------------
# Google Test
#-----------------------------------------------------------------------------------------------------------------------
include(GoogleTest)
enable_testing()

#-----------------------------------------------------------------------------------------------------------------------
# Target: cxx_array_test
# Alias: cxx_array::cxx_array_test
# Type: Executable
# Dependencies: GTest::gtest_main, GTest::gmock_main, cxx_array::cxx_array_headers
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_executable(cxx_array_test)

# Target alias
add_executable(cxx_array::cxx_array_test ALIAS cxx_array_test)

# Target sources
target_sources(cxx_array_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/cxx_array_test.cpp
)

# Target dependencies
target_link_libraries(cxx_array_test
    PRIVATE
        cxx_array::cxx_array_headers
        GTest::gtest_main
        GTest::gmock_main
)

# Test discovery
gtest_discover_tests(cxx_array_test)
