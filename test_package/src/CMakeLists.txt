#-----------------------------------------------------------------------------------------------------------------------
# Target: test_package
# Type: Executable
# Dependencies: cxx_array::cxx_array
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_executable(test_package)

# Target sources
target_sources(test_package
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/main.c
)

# Target dependencies
target_link_libraries(test_package
    PRIVATE
        cxx_array::cxx_array
)
