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

struct RenderGraphNode {
    bool Update = false;
    bool to_render;
    VkPipeline Handle;
    VkPipelineLayout Layout;
    VkShaderModule Modules[2];
    JSON::GeometryType GeoType;
    JSON::Dimension BatchDimension;
    VkPolygonMode PolygonMode = VK_POLYGON_MODE_LINE;

    uint8_t LineWidth = 1;
    Batch CPUMeshData;
};

struct RenderGraphCreateInfos {
    VkDevice Device;
    VkRenderPass RenderPass;

    VkSampleCountFlagBits msaaSamples;
};

struct RenderGraph {
    bool Update = false;
    Buffer PushBuffer;
    std::vector<RenderGraphNode> Nodes;
    JSON::SceneLayout Layout;

    CameraUniform PushCamera;
    CameraController Cam;
};

RenderGraph ConstructRenderGraph(RenderGraphCreateInfos CreateInfos, const VmaAllocator& Allocator,
                                 JSON::SceneLayout& Layout, const ShaderLibrary& Shaders);

void DestroyRenderGraph(const VkDevice& Device, const VmaAllocator& Allocator, RenderGraph Graph);

void ReloadRenderGraph(RenderGraphCreateInfos CreateInfos, RenderGraph& Graph);

RenderGraphNode FillRenderGraphNode(Array& Data, JSON::GeometryType GeoType, JSON::Dimension n, int LineWidth);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // GRAPH_CONSTRUCTOR_H_