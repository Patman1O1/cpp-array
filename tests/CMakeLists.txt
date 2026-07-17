#-----------------------------------------------------------------------------------------------------------------------
# Google Test
#-----------------------------------------------------------------------------------------------------------------------
include(GoogleTest)
enable_testing()

#-----------------------------------------------------------------------------------------------------------------------
# Target: array_test
# Alias: collections::array_test
# Type: Executable
# Dependencies: GTest::gtest_main, GTest::gmock_main, collections::array_headers
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_executable(array_test)

# Target alias
add_executable(collections::array_test ALIAS array_test)

# Target sources
target_sources(array_test
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/array_test.cpp
)

# Target dependencies
target_link_libraries(array_test
    PRIVATE
        collections::array_headers
        GTest::gtest_main
        GTest::gmock_main)

# Test discovery
gtest_discover_tests(array_test)


