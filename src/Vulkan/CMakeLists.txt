add_subdirectory(Window)

add_library(ffGraph_Vulkan
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Instance.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/InstanceReload.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/vmaDeclaration.cpp

    ${CMAKE_SOURCE_DIR}/src/Vulkan/Context/Context.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Context/Device.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Context/Swapchain.cpp

    ${CMAKE_SOURCE_DIR}/src/Vulkan/RenderGraph/GraphConstructor.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/RenderGraph/RenderGraph.cpp

    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Resource.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Image/Image.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Shader/Shader.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Mesh/Mesh.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Buffer/Buffer.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Camera/Camera.cpp

    ${CMAKE_SOURCE_DIR}/src/Vulkan/Renderer/Renderer.cpp
)

target_include_directories(ffGraph_Vulkan PRIVATE ${Vulkan_INCLUDE_DIR})
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/extern/VulkanMemoryAllocator/src)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/extern/glfw/include)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/src/util)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/src/JSON)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/src/Vulkan)
target_link_libraries(ffGraph_Vulkan ${Vulkan_LIBRARY})
target_link_libraries(ffGraph_Vulkan ffGraph_Window)
target_link_libraries(ffGraph_Vulkan ffGraph_JSON)