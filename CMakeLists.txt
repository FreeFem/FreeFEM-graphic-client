cmake_minimum_required(VERSION 3.0.0)
project(FreeFEM_graphic_client VERSION 0.1.0)

include(CTest)
enable_testing()

# finding pthreads as a required lib
find_package(Threads REQUIRED)

# Compiling volk and adding they're headers files to the includes directories
add_subdirectory(${CMAKE_SOURCE_DIR}/volk)
include_directories(${CMAKE_SOURCE_DIR}/volk)

# Compiling glfw and adding they're headers files to the includes directories
add_subdirectory(${CMAKE_SOURCE_DIR}/glfw)
include_directories(${CMAKE_SOURCE_DIR}/glfw/include)

# Adding asio to include directories
include_directories(${CMAKE_SOURCE_DIR}/asio/asio/include)

# Adding json to include directories
include_directories(${CMAKE_SOURCE_DIR}/json/include)

# Telling Cmake to compile a executable
add_executable(FreeFEM_graphic_client ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Linking executable to volk
target_link_libraries(FreeFEM_graphic_client volk)

# Linking executable to glfw
target_link_libraries(FreeFEM_graphic_client glfw)

# Linking executable to pthread
target_link_libraries(FreeFEM_graphic_client ${THREADS_PREFER_PTHREAD_FLAG})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
