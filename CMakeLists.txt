cmake_minimum_required(VERSION 3.0)
project(FreeFEM_graphic_client VERSION 0.1.0 LANGUAGES CXX)

file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

set_target_properties(FreeFEM_graphic_client PROPERTIES CXX_STANDARD 11)

# Telling Cmake to compile a executable
add_executable(FreeFEM_graphic_client )
