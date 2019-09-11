#ifndef PIPELINE_H_
#define PIPELINE_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "../core/math/Camera.h"
#include "Buffer.h"

namespace FEM {
namespace VK {

struct PipelineSubResources {
    VkPipeline Handle;
    VkPipelineLayout Layout;

    VkPrimitiveTopology Topology;
    VkClearValue ClearValue;
    std::vector<VertexBuffer> VBuffers;

    VkShaderModule VertexShader;
    VkShaderModule FragmentShader;
    PipelineSubResources *next;
};

struct Pipeline {
    uint32_t SubPipelineCount;
    PipelineSubResources *SubPipelines;

    // TO-DO : USING PUSH CONSTANT FOR NOW.
    // VkDescriptorPool DescriptorPool;
    // VkDescriptorSetLayout DescriptorSetLayout;
    // VkDescriptorSet *DescriptorSets;

    VkRenderPass RenderPass;
    uint32_t FramebufferCount;
    VkFramebuffer *Framebuffers;
    Camera *Cam;
};

bool newPipeline(Pipeline *Handle, const VulkanContext vkContext, const Window Win);

void destroyPipeline(Pipeline Handle, const VulkanContext vkContext);

bool addSubPipeline(PipelineSubResources *SubPipeline, const VulkanContext vkContext, Pipeline *MotherPipeline);

void destroySubPipeline(PipelineSubResources *SubPipeline, const VulkanContext vkContext,
                        const Pipeline MotherPipeline);

PipelineSubResources *newSubPipeline();
bool addShadersToSubPipeline(PipelineSubResources *SubPipeline, VkShaderModule VertexShader, VkShaderModule FragmentShader);
bool addVertexBuffersToSubPipeline(PipelineSubResources *SubPipeline, VkPrimitiveTopology Topology, uint32_t Count, VertexBuffer *VBuffers);
uint32_t CountNbOfVerticesInSubPipeline(PipelineSubResources SubPipeline);

}    // namespace VK
}    // namespace FEM

#endif    // PIPELINE_H_