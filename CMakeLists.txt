cmake_minimum_required(VERSION 3.0)
project(ffGraph VERSION 0.1.0 LANGUAGES CXX)

find_package(Threads REQUIRED)
find_package(Vulkan REQUIRED)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -Wextra -D_DEBUG")

add_subdirectory(${CMAKE_SOURCE_DIR}/src/JSON)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/network)
add_subdirectory(${CMAKE_SOURCE_DIR}/src/Vulkan)
add_subdirectory(${CMAKE_SOURCE_DIR}/extern/glfw)
# Telling Cmake to compile a executable
add_executable(ffGraph ${CMAKE_SOURCE_DIR}/src/main.cpp)

# Compiling shaders
if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/bin/glslangValidator")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
    find_path(GLSL_VALIDATOR glslangValidator PATHS ${VULKAN_SDK})
    # set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/macOS/bin/glslangValidator")
    target_compile_options(ffGraph PRIVATE "-Wc++11-extensions")
elseif (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    set(GLSL_VALIDATOR "$ENV{VULKAN_SDK}/bin/glslangValidator.exe")
endif()

list(APPEND GLSL_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/shaders/Geo2D.vert
    ${CMAKE_SOURCE_DIR}/shaders/Geo3D.vert
    ${CMAKE_SOURCE_DIR}/shaders/Color.frag
    ${CMAKE_SOURCE_DIR}/shaders/UI.frag
    ${CMAKE_SOURCE_DIR}/shaders/UI.vert
)

foreach(GLSL ${GLSL_SOURCE_FILES})
  get_filename_component(FILE_NAME ${GLSL} NAME)
  set(SPIRV "${PROJECT_BINARY_DIR}/shaders/${FILE_NAME}.spirv")
  add_custom_command(
    OUTPUT ${SPIRV}
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${GLSL_VALIDATOR} -V ${GLSL} -o ${SPIRV}
    DEPENDS ${GLSL})
  list(APPEND SPIRV_BINARY_FILES ${SPIRV})
endforeach(GLSL)

add_custom_target(
    Shaders
    DEPENDS ${SPIRV_BINARY_FILES}
)

add_dependencies(ffGraph Shaders)

add_custom_command(TARGET ffGraph POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/shaders/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_BINARY_DIR}/shaders"
        "${PROJECT_BINARY_DIR}/shaders"
)

file(COPY "${CMAKE_SOURCE_DIR}/shaders/grid.json" DESTINATION "${CMAKE_BINARY_DIR}/shaders")

# Done compiling shaders

# Using C++11 standard
set_target_properties(ffGraph PROPERTIES CXX_STANDARD 11)

# Linking libraries with target
target_include_directories(ffGraph PRIVATE ${Vulkan_INCLUDE_DIR})
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/extern/asio/asio/include)
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/extern/glfw/include)
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/extern/VulkanMemoryAllocator/src)
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/extern/glm)
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/extern/json/include)

target_link_libraries(ffGraph Threads::Threads)
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/src/JSON)
target_link_libraries(ffGraph ffGraph_JSON)
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/src/network)
target_link_libraries(ffGraph ffGraph_NET)
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/src/util)
target_link_libraries(ffGraph ffGraph_Vulkan)
target_include_directories(ffGraph PRIVATE ${CMAKE_SOURCE_DIR}/src/Vulkan)
