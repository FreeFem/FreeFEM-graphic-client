add_subdirectory(Window)

add_library(ffGraph_Vulkan
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Context/ffContext.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Context/ffInstance.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Context/ffDevice.cpp
)

target_include_directories(ffGraph_Vulkan PRIVATE ${Vulkan_INCLUDE_DIR})
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/src/Vulkan)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/extern/VulkanMemoryAllocator/src)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/extern/glfw/include)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/src/util)
target_link_libraries(ffGraph_Vulkan ${Vulkan_LIBRARY})
target_link_libraries(ffGraph_Vulkan ffGraph_Window)