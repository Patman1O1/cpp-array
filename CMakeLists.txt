#-----------------------------------------------------------------------------------------------------------------------
# Information
#-----------------------------------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 4.3.0)
project("array"
        VERSION 0.1.0
        DESCRIPTION ""
        LANGUAGES CXX
)

# CMake Information
cmake_file_api(
        QUERY
        API_VERSION 1
        CODEMODEL 2.3
)

#-----------------------------------------------------------------------------------------------------------------------
# Settings
#-----------------------------------------------------------------------------------------------------------------------
if(PROJECT_IS_TOP_LEVEL)
    # CXX Settings
    set(CMAKE_CXX_STANDARD 26)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
    set(CMAKE_CXX_VISIBILITY_PRESET hidden)
    set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

    # Project Settings
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

    # Project Options
    option(BUILD_TESTS "Build the project's test suite" OFF)
    option(BUILD_BENCHMARKS "Build the project benchmarks" OFF)

    # Set default build type if not specified
    if(NOT DEFINED CMAKE_BUILD_TYPE AND NOT DEFINED CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
    endif()
endif()

#-----------------------------------------------------------------------------------------------------------------------
# Dependencies
#-----------------------------------------------------------------------------------------------------------------------
if(BUILD_TESTS)
    find_package(GTest REQUIRED)
    find_package(benchmark REQUIRED)
endif()

include(GNUInstallDirs)

#-----------------------------------------------------------------------------------------------------------------------
# Installation
#-----------------------------------------------------------------------------------------------------------------------
if(NOT CMAKE_SKIP_INSTALL_RULES)
    # Install headers
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
            TYPE INCLUDE)
endif()

#-----------------------------------------------------------------------------------------------------------------------
# Target: array_headers
# Alias: collections::array_headers
# Type: Interface Library
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_library(array_headers INTERFACE)

# Target alias
add_library(collections::array_headers ALIAS array_headers)

# Target headers
target_include_directories(array_headers
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

#-----------------------------------------------------------------------------------------------------------------------
# Target: array
# Alias: collections::array
# Type: Interface Library
# Dependencies: collections::array_headers
#-----------------------------------------------------------------------------------------------------------------------
# Target definition
add_library(array INTERFACE)

# Target alias
add_library(collections::array ALIAS array)

# Target dependencies
target_link_libraries(array
    INTERFACE
        collections::array_headers
)

# Target properties
set_target_properties(array PROPERTIES
        OUTPUT_NAME array
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
)

if(NOT CMAKE_SKIP_INSTALL_RULES)
    # Install array
    install(TARGETS array
            RUNTIME COMPONENT array
            LIBRARY COMPONENT array
            NAMELINK_COMPONENT array
            ARCHIVE COMPONENT array
            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
endif()

#-----------------------------------------------------------------------------------------------------------------------
# Target: uninstall
# Type: Custom
#-----------------------------------------------------------------------------------------------------------------------
add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake
)

#-----------------------------------------------------------------------------------------------------------------------
# Subdirectories
#-----------------------------------------------------------------------------------------------------------------------
if(BUILD_TESTS)
    add_subdirectory(tests)
endif()

if(BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif()
