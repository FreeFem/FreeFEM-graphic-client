add_library(ffGraph_RM
    ffBuffer.cpp
    ffMesh.cpp
    ffShader.cpp
    ffShaderManager.cpp
)

target_include_directories(ffGraph_RM PRIVATE ${Vulkan_INCLUDE_DIR})
target_include_directories(ffGraph_RM PRIVATE ${CMAKE_SOURCE_DIR}/extern/VulkanMemoryAllocator/src)
target_include_directories(ffGraph_RM PRIVATE ${CMAKE_SOURCE_DIR}/extern/json/include)
target_include_directories(ffGraph_RM PRIVATE ../)
target_link_libraries(ffGraph_RM ${Vulkan_LIBRARY})