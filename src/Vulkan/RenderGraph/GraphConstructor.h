#ifndef GRAPH_CONSTRUCTOR_H_
#define GRAPH_CONSTRUCTOR_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_mem_alloc.h"
#include "deserializer.h"
#include "Resource/Image/Image.h"
#include "Resource/Mesh/Mesh.h"
#include "Resource/Camera/Camera.h"
#include "Context/Device.h"

namespace ffGraph {
namespace Vulkan {

struct RenderGraphNode {
    VkPrimitiveTopology Topology;
    std::vector<Mesh> Meshes;
    std::vector<VkBuffer> GPUBuffers;
    VkPipeline Handle;
    VkPipelineLayout Layout;
};

struct RenderGraph {
    std::vector<RenderGraphNode> Nodes;

    CameraUniform PushCamera;
    Camera Cam;
};

struct GraphConstructor {
    VkRenderPass RenderPass;
    std::vector<VkFramebuffer> Framebuffers;

    Image DepthImage;
    Image ColorImage;
};

GraphConstructor newGraphConstructor(const Device& D, const VmaAllocator& Allocator, VkFormat SurfaceFormat, VkExtent2D WindowSize, std::vector<VkImageView> SwapchainViews);

void DestroyGraphConstructor(const VkDevice& Device, const VmaAllocator& Allocator, GraphConstructor& Graph);

RenderGraph ConstructRenderGraph(const Device& D, const VkRenderPass& Renderpass, const VmaAllocator& Allocator, JSON::SceneLayout& Layout, const VkShaderModule Modules[2]);

void DestroyRenderGraph(const VkDevice& Device, const VmaAllocator& Allocator, RenderGraph Graph);

} // namespace Vulkan
} // namespace ffGraph

#endif // GRAPH_CONSTRUCTOR_H_