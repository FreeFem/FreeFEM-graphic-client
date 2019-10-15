#ifndef GRAPH_CONSTRUCTOR_H_
#define GRAPH_CONSTRUCTOR_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_mem_alloc.h"
#include "deserializer.h"
#include "Resource/Shader.h"
#include "Resource/Image/Image.h"
#include "Resource/Mesh/Mesh.h"
#include "Resource/Buffer/Buffer.h"
#include "Resource/Camera/CameraController.h"

namespace ffGraph {
namespace Vulkan {

struct RenderUiNode {
    struct ImGuiInfos {
        glm::vec2 Scale;
        glm::vec2 Translate;
    } ImGuiData;
    VkSampler Sampler;
    Buffer ImGuiBufferVertices;
    uint32_t vertexCount = 0;
    Buffer ImGuiBufferIndices;
    uint32_t indexCount = 0;
    Image FontImage;

    bool Update = false;
    bool to_render;
    VkPipeline Handle;
    VkPipelineLayout Layout;
    VkShaderModule Modules[2];

    VkDescriptorPool DescriptorPool;
    VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;

    bool SomeBool = false;
};

struct RenderGraphNode {
    bool Update = false;
    bool to_render;
    VkPipeline Handle;
    VkPipelineLayout Layout;
    VkShaderModule Modules[2];
    JSON::GeometryType GeoType;
    JSON::Dimension BatchDimension;
    VkPolygonMode PolygonMode = VK_POLYGON_MODE_LINE;

    VkDescriptorPool DescriptorPool;
    VkDescriptorSetLayout DescriptorSetLayout;
    VkDescriptorSet DescriptorSet;

    uint8_t LineWidth = 1;
    Batch CPUMeshData;
};

struct DescriptorInfos {
    VkDescriptorType Type;

    VkShaderStageFlags Stage;

    VkBuffer* Uniform;
    VkDeviceSize Size;

    VkSampler* Sampler;
    VkImageView* View;
};

struct RenderGraphCreateInfos {
    VkDevice Device;
    VkRenderPass RenderPass;

    VkDeviceSize PushConstantSize;
    void* PushConstantPTR;
    VkShaderStageFlags Stage;

    std::vector<DescriptorInfos> Descriptors;

    VkSampleCountFlagBits msaaSamples;
    float AspectRatio;
};

struct RenderGraph {
    bool Update = false;
    Buffer PushBuffer;
    std::vector<RenderGraphNode> Nodes;
    JSON::SceneLayout Layout;
    RenderUiNode UiNode;

    bool someBool = false;

    CameraUniform PushCamera;
    CameraController Cam;
};

RenderGraph ConstructRenderGraph(RenderGraphCreateInfos CreateInfos, const VmaAllocator& Allocator,
                                 JSON::SceneLayout& Layout, const ShaderLibrary& Shaders);

void DestroyRenderGraph(const VkDevice& Device, const VmaAllocator& Allocator, RenderGraph Graph);

void ReloadRenderGraph(RenderGraphCreateInfos CreateInfos, RenderGraph& Graph);

RenderGraphNode FillRenderGraphNode(Array& Data, JSON::GeometryType GeoType, JSON::Dimension n, int LineWidth);

bool ConstructUiNode(RenderGraphCreateInfos CreateInfos, const ShaderLibrary& Shaders, RenderUiNode& Node);

void UpdateUIBuffers(RenderUiNode& Node);

void DestroyUiNode(RenderUiNode& Node);

void ReloadUINode(RenderGraphCreateInfos CreateInfos, RenderUiNode& Node);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // GRAPH_CONSTRUCTOR_H_