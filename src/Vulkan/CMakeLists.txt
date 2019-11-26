add_subdirectory(Window)

add_library(ffGraph_Vulkan
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Instance.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Environment.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/GraphicManager.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/InstanceReload.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/InstanceEvent.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/vmaDeclaration.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/ImGui_Impl.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Frame.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Loop.cpp

    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Image/Image.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Shader.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Buffer/Buffer.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Camera/Camera.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Resource/Camera/CameraController.cpp

    #ImGui
    ${CMAKE_SOURCE_DIR}/extern/imgui/imgui.cpp
    ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_widgets.cpp
    ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_draw.cpp
    ${CMAKE_SOURCE_DIR}/extern/imgui/imgui_demo.cpp

    ${CMAKE_SOURCE_DIR}/src/Vulkan/Graph/Graph.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Graph/Pipeline.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Graph/Descriptor.cpp
    ${CMAKE_SOURCE_DIR}/src/Vulkan/Graph/BasePipelineCreateInfos.cpp

)

target_include_directories(ffGraph_Vulkan PRIVATE ${Vulkan_INCLUDE_DIR})
target_include_directories(ffGraph_Vulkan SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/extern/VulkanMemoryAllocator/src)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/extern/glfw/include)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/extern/json/include)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/extern/imgui)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/extern/glm)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/src/util)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/src/JSON)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/src/Vulkan)
target_include_directories(ffGraph_Vulkan PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(ffGraph_Vulkan ${Vulkan_LIBRARY})
target_link_libraries(ffGraph_Vulkan ffGraph_Window)
target_link_libraries(ffGraph_Vulkan ffGraph_JSON)