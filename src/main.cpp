#include "core/Application.h"
#include "vk/Buffer.h"

void InitAppDataToRender(FEM::Application *App)
{
    FEM::VK::newShader("Vertex", "./shaders/geometry.vert.spirv", &App->Shaders, App->vkContext);
    FEM::VK::newShader("Fragment", "./shaders/geometry.frag.spirv", &App->Shaders, App->vkContext);

    VkClearValue ClearValue;
    ClearValue.color = {1.f, 1.f, 1.f, 0.f};

    VkClearValue ClearValue2;
    ClearValue2.color = {1.f, 1.f, 1.f, 0.f};

    FEM::VK::PipelineSubResources *SubPipeline = FEM::VK::newSubPipeline();
    FEM::VK::PipelineSubResources *SubPipeline2 = FEM::VK::newSubPipeline();

    FEM::VK::addShadersToSubPipeline(SubPipeline, FEM::VK::searchShader("Vertex", App->Shaders), FEM::VK::searchShader("Fragment", App->Shaders));
    FEM::VK::addShadersToSubPipeline(SubPipeline2, FEM::VK::searchShader("Vertex", App->Shaders), FEM::VK::searchShader("Fragment", App->Shaders));

    FEM::VK::VertexBuffer tmp_buff;
    FEM::VK::BufferInfos BInfos = {};
    BInfos.ElementCount = NUM_DEMO_VERTICES;
    BInfos.ElementSize = SIZE_DEMO_VERTEX;
    BInfos.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    BInfos.AllocInfos.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    BInfos.AllocInfos.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    BInfos.AllocInfos.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    BInfos.AllocInfos.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkVertexInputAttributeDescription InputAttrib[2] = {};
    InputAttrib[0].binding = 0;
    InputAttrib[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    InputAttrib[0].location = 0;
    InputAttrib[0].offset = 0;

    InputAttrib[1].binding = 0;
    InputAttrib[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    InputAttrib[1].location = 1;
    InputAttrib[1].offset = sizeof(float) * 3;
    if (!FEM::VK::newVertexBuffer(App->vkContext.Allocator, &App->Buffers, &tmp_buff, BInfos, 2, InputAttrib)) {
        LOGI("Loop", "Failed to create tmp vertexBuffer");
        return;
    }

    memcpy(tmp_buff.VulkanData.MemoryInfos.pMappedData, vertices,
           tmp_buff.VulkanData.MemoryInfos.size);

    FEM::VK::VertexBuffer buffer2;
    FEM::VK::BufferInfos BInfos2 = {};
    BInfos2.ElementCount = NUM_DEMO_VERTICES2;
    BInfos2.ElementSize = SIZE_DEMO_VERTEX2;
    BInfos2.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    BInfos2.AllocInfos.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    BInfos2.AllocInfos.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    BInfos2.AllocInfos.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    BInfos2.AllocInfos.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkVertexInputAttributeDescription InputAttrib2[2] = {};
    InputAttrib2[0].binding = 0;
    InputAttrib2[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    InputAttrib2[0].location = 0;
    InputAttrib2[0].offset = 0;

    InputAttrib2[1].binding = 0;
    InputAttrib2[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    InputAttrib2[1].location = 1;
    InputAttrib2[1].offset = sizeof(float) * 3;
    if (!FEM::VK::newVertexBuffer(App->vkContext.Allocator, &App->Buffers, &buffer2, BInfos2, 2, InputAttrib2)) {
        LOGI("Loop", "Failed to create tmp vertexBuffer2");
        return;
    }

    memcpy(buffer2.VulkanData.MemoryInfos.pMappedData, vertices2,
           buffer2.VulkanData.MemoryInfos.size);

    FEM::VK::addClearValueToSubPipeline(SubPipeline, ClearValue);
    FEM::VK::addClearValueToSubPipeline(SubPipeline2, ClearValue2);

    FEM::VK::addVertexBuffersToSubPipeline(SubPipeline, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, 1, &tmp_buff);
    FEM::VK::addVertexBuffersToSubPipeline(SubPipeline2, VK_PRIMITIVE_TOPOLOGY_LINE_LIST, 1, &buffer2);

    if (!FEM::VK::addSubPipeline(SubPipeline, App->vkContext, &App->Renderer)) {
        LOGI("Loop", "Failed to ad subpipeline 1");
        return;
    }
    if (!FEM::VK::addSubPipeline(SubPipeline2, App->vkContext, &App->Renderer)) {
        LOGI("Loop", "Failed to ad subpipeline 2");
        return;
    }

    FEM::computeCamera(App->Renderer.Cam);
}

int main(int ac, char **av) {
    FEM::Application App;
    FEM::ApplicationCreateInfo AppCreateInfos = FEM::getApplicationInfos(ac, av);

    if (!FEM::newGLFWContext( )) return EXIT_FAILURE;

    if (!FEM::newApplication(&App, AppCreateInfos)) {
        LOGE("main", "Failed to initialize Application.");
        return EXIT_FAILURE;
    }

    InitAppDataToRender(&App);

    FEM::runApplication(&App);

    FEM::destroyApplication(&App);

    return EXIT_SUCCESS;
}