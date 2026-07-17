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

#-----------------------------------------------------------------------------------------------------------------------
# Target: diff
# Alias: collections::diff
# Type: Executable
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_executable(diff)

# Target alias
add_executable(collections::diff ALIAS diff)

# Target sources
target_sources(diff
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/diff.cpp
)

