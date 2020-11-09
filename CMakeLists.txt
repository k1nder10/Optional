cmake_minimum_required(VERSION 3.10)

project(optional LANGUAGES CXX)

add_library(optional SHARED palkin/optional.hpp)

set(default_build_type "Debug")    
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message("Message: Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(optional PRIVATE $<$<CONFIG:RELEASE>:-O2>)
    target_compile_options(optional PRIVATE $<$<CONFIG:DEBUG>:-Wall;-Wextra;-Wpedantic>)
endif()

set_target_properties(optional PROPERTIES CXX_STANDARD 14
                                          CXX_STANDARD_REQUIRED on
                                          LINKER_LANGUAGE CXX)
                                        
# Compile tests
option(COMPILE_TESTS "Compile tests for the library" OFF)
if (COMPILE_TESTS)
    list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
    enable_testing()
    add_subdirectory(test)
endif()