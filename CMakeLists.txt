cmake_minimum_required(VERSION 3.0)
project(FreeFEM_graphic_client VERSION 0.1.0 LANGUAGES CXX)

# Force you to build inside a subdirectory such as project_folder/build
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Doxygen WIP
# option(BUILD_DOC "Build documentation" ON)

# if (BUILD_DOC)
#     find_package(Doxygen)
#     if (DOXYGEN_FOUND)
#         set(DOXYGEN_IN ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in)
#         set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/GeneratedDoxyfile.in)

#         configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
#         message("Doxygen build started")

#         add_custom_target(doc_doxygen ALL
#                             COMMAND ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}
#                             COMMENT "Generating API documentation with Doxygen"
#                             VERBATIM)
#     else (DOXYGEN_FOUND)
#         message("Doxygen need to be installed to generate the doxygen documentation")
#     endif (DOXYGEN_FOUND)
# endif(BUILD_DOC)

find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -D_DEBUG")

# Adding glfw to compilation
add_subdirectory(extern/glfw)
include_directories(extern/glfw/include)

# Adding asio to include
include_directories(extern/asio/asio/include)

# Adding json to include
include_directories(extern/json/include)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/shaders DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Telling Cmake to compile a executable
add_executable(FreeFEM_graphic_client src/main.cpp src/vkContext.cpp)

# Using C++11 standard
set_target_properties(FreeFEM_graphic_client PROPERTIES CXX_STANDARD 11)

# Linking libraries with target
target_link_libraries(FreeFEM_graphic_client glfw)
target_link_libraries(FreeFEM_graphic_client xcb)
target_link_libraries(FreeFEM_graphic_client ${THREADS_PREFER_PTHREAD_FLAG})
target_link_libraries(FreeFEM_graphic_client ${Vulkan_LIBRARY})
target_link_libraries(FreeFEM_graphic_client m)