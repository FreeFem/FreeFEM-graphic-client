#include "core/Application.h"
#include "vk/Buffer.h"

void InitAppDataToRender(FEM::Application *App)
{
    FEM::VK::newShader("Vertex", "./shaders/geometry.vert.spirv", &App->Shaders, App->vkContext);
    FEM::VK::newShader("Fragment", "./shaders/geometry.frag.spirv", &App->Shaders, App->vkContext);
    FEM::VK::newShader("GridVert", "./shaders/grid.vert.spirv", &App->Shaders, App->vkContext);
    FEM::VK::newShader("GridFrag", "./shaders/grid.frag.spirv", &App->Shaders, App->vkContext);

    VkClearValue ClearValue;
    ClearValue.color = {1.f, 1.f, 1.f, 0.f};

    VkClearValue ClearValue2;
    ClearValue2.color = {1.f, 1.f, 1.f, 0.f};

    FEM::VK::PipelineSubResources *SubPipeline = FEM::VK::newSubPipeline();

    FEM::VK::addShadersToSubPipeline(SubPipeline, FEM::VK::searchShader("Vertex", App->Shaders), FEM::VK::searchShader("GridFrag", App->Shaders));

    FEM::VK::VertexBuffer tmp_buff;
    FEM::VK::BufferInfos BInfos = {};
    BInfos.ElementCount = NUM_DEMO_VERTICES2;
    BInfos.ElementSize = SIZE_DEMO_VERTEX2;
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

    memcpy(tmp_buff.VulkanData.MemoryInfos.pMappedData, vertices2,
           tmp_buff.VulkanData.MemoryInfos.size);

    FEM::VK::addVertexBuffersToSubPipeline(SubPipeline, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 1, &tmp_buff);

    if (!FEM::VK::addSubPipeline(SubPipeline, App->vkContext, &App->Renderer)) {
        LOGI("Loop", "Failed to ad subpipeline 1");
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