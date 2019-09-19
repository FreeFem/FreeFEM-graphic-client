cmake_minimum_required(VERSION 3.0)
project(ffGraph VERSION 0.1.0 LANGUAGES CXX)

find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -D_DEBUG")

add_subdirectory(src/ResourceManager)
add_subdirectory(src/network)
# Telling Cmake to compile a executable
add_executable(ffGraph src/main.cpp)

# Using C++11 standard
set_target_properties(ffGraph PROPERTIES CXX_STANDARD 11)

# Linking libraries with target
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/extern/asio/asio/include)

target_link_libraries(ffGraph Threads::Threads)
target_include_directories(ffGraph PRIVATE ./src/ResourceManager)
target_link_libraries(ffGraph ffGraph_ResourceManager)
target_include_directories(ffGraph PRIVATE ./src/network)
target_link_libraries(ffGraph ffGraph_NET)
target_include_directories(ffGraph PRIVATE ./src/util)