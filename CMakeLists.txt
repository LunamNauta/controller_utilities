cmake_minimum_required(VERSION 3.5)
project(controller_utilities)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE sources src *.cpp)

add_library(controller_utilities SHARED ${sources})
target_include_directories(controller_utilities PUBLIC include)

set_target_properties(controller_utilities
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
)
