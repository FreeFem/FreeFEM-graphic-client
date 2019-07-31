cmake_minimum_required(VERSION 3.0.0)
project(FreeFEM_graphic_client VERSION 0.1.0)

include(CTest)
enable_testing()

find_package(Threads REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/volk)
include_directories(${CMAKE_SOURCE_DIR}/volk)
add_subdirectory(${CMAKE_SOURCE_DIR}/glfw)
include_directories(${CMAKE_SOURCE_DIR}/glfw/include)

add_executable(FreeFEM_graphic_client ${CMAKE_SOURCE_DIR}/src/main.cpp)
target_link_libraries(FreeFEM_graphic_client volk)
target_link_libraries(FreeFEM_graphic_client glfw)
target_link_libraries(FreeFEM_graphic_client ${THREADS_PREFER_PTHREAD_FLAG})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
