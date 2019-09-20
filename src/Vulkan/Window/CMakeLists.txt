add_library(ffGraph_Window
    NativeWindow.cpp
)

target_include_directories(ffGraph_Window PRIVATE ${Vulkan_INCLUDE_DIR})
target_include_directories(ffGraph_Window PRIVATE ${CMAKE_SOURCE_DIR}/extern/glfw/include)
target_include_directories(ffGraph_Window PRIVATE ${CMAKE_SOURCE_DIR}/src/util)
target_link_libraries(ffGraph_Window glfw)