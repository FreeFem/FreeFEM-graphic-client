add_library(ffGraph_ResourceManager
    ResourceManager.cpp
    vmaDeclaration.cpp

    Shader/ffShader.cpp
    Shader/ffShaderManager.cpp

    Mesh/ffBuffer.cpp
    Mesh/ffMesh.cpp
    Mesh/ffMeshManager.cpp

    Image/ffImageManager.cpp
    Image/ffImage.cpp
)

target_include_directories(ffGraph_ResourceManager PRIVATE ${Vulkan_INCLUDE_DIR})
target_include_directories(ffGraph_ResourceManager PRIVATE ${CMAKE_SOURCE_DIR}/extern/VulkanMemoryAllocator/src)
target_include_directories(ffGraph_ResourceManager PRIVATE ${CMAKE_SOURCE_DIR}/extern/glm)
target_include_directories(ffGraph_ResourceManager PRIVATE ${CMAKE_SOURCE_DIR}/extern/json/include)
target_include_directories(ffGraph_ResourceManager PRIVATE ${CMAKE_SOURCE_DIR}/extern/glm)
target_include_directories(ffGraph_ResourceManager PRIVATE ${CMAKE_SOURCE_DIR}/src/util)
target_include_directories(ffGraph_ResourceManager PRIVATE ${CMAKE_SOURCE_DIR}/src/ResourceManager)
target_link_libraries(ffGraph_ResourceManager ${Vulkan_LIBRARY})