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

#-----------------------------------------------------------------------------------------------------------------------
# Target: array_benchmark
# Alias: collections::array_benchmark
# Type: Executable
# Dependencies: benchmark::benchmark_main, collections::array_headers
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_executable(array_benchmark)

# Target alias
add_executable(collections::array_benchmark ALIAS array_benchmark)

# Target sources
target_sources(array_benchmark
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/array_benchmark.cpp
)

# Target dependencies
target_link_libraries(array_benchmark
    PRIVATE
        collections::array_headers
        benchmark::benchmark_main
)

