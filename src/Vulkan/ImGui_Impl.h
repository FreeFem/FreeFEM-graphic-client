#ifndef IMGUI_IMPL_H_
#define IMGUI_IMPL_H_

#include <glm/vec2.hpp>
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"
#include "Resource/Buffer/Buffer.h"
#include "Resource/Image/Image.h"

namespace ffGraph {
namespace Vulkan {

struct UiPipeline {

    struct ImGuiPushConst {
        glm::vec2 Scale;
        glm::vec2 Translate;
    } ImGuiPushConstant;

    VkSampler ImGuiSampler;

    uint32_t VertexCount = 0;
    Buffer ImGuiVertices;

    uint32_t IndexCount = 0;
    Buffer ImGuiIndices;

    Image FontImage;
    bool to_render = true;
    VkPipeline Handle;
    VkPipelineLayout Layout;
    VkShaderModule Modules[2];

    VkDescriptorPool DescriptorPool;
    VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;

};

UiPipeline NewUiPipeline(const VkShaderModule Shaders[2]);
void UpdateUiPipeline(UiPipeline& n);
void DestroyUiPipeline(UiPipeline& n);

}
}

#endif // IMGUI_IMPL_H_