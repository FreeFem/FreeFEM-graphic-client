cmake_minimum_required(VERSION 3.0)
project(FreeFEM_graphic_client VERSION 0.1.0 LANGUAGES CXX)

macro(SET_OPTION option value)
    set(${option} ${value} CACHE "" INTERNAL FORCE)
endmacro()

# Force you to build inside a subdirectory such as project_folder/build
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -D_DEBUG")

# Adding VulkanSDK include path
include_directories(${vulkan_INCLUDE_DIR})

# Adding glfw to compilation
add_subdirectory(extern/glfw)
include_directories(extern/glfw/include)

# Adding glm to compilation
add_subdirectory(extern/glm)
include_directories(extern/glm/glm)

# Adding asio to include
include_directories(extern/asio/asio/include)

# Adding VulkanMemoryAllocator to include path
include_directories(extern/VulkanMemoryAllocator/src)

# Adding json to include
include_directories(extern/json/include)
set(GLM_TEST_ENABLE OFF CACHE BOOL "Build unit tests" FORCE)
#option(GLM_TEST_ENABLE "Build unit tests" OFF)
option(BUILD_SHARED_LIBS "Build shared library" OFF)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

include(src/CMakelists.txt)

# Telling Cmake to compile a executable
add_executable(FreeFEM_graphic_client ${freeFEM_SOURCES})

# Using C++11 standard
set_target_properties(FreeFEM_graphic_client PROPERTIES CXX_STANDARD 11)

# Linking libraries with target
target_link_libraries(FreeFEM_graphic_client glfw)
target_link_libraries(FreeFEM_graphic_client glm)
target_link_libraries(FreeFEM_graphic_client xcb)
target_link_libraries(FreeFEM_graphic_client ${THREADS_PREFER_PTHREAD_FLAG})
target_link_libraries(FreeFEM_graphic_client ${Vulkan_LIBRARY})
target_link_libraries(FreeFEM_graphic_client m)