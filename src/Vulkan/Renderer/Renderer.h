#ifndef RENDERER_H_
#define RENDERER_H_

#include <array>
#include <vulkan/vulkan.h>
#include "../RenderGraph/GraphConstructor.h"
#include "../Context/Context.h"

namespace ffGraph {
namespace Vulkan {

struct PerFrame {
    bool FenceInitialized = false;
    VkFence PresentFence;
    VkSemaphore Acquire;
    VkSemaphore Render;
    VkCommandBuffer CmdBuffer;
};

struct Renderer {
    VkQueue *Queue_REF;

    VkCommandPool CommandPool;

    uint8_t frame = 0;
    std::array<PerFrame, 2> Frames;
    // Framebuffers
};

Renderer NewRenderer(const VkDevice& Device, VkQueue *Queue, uint32_t QueueIndex, const VkSurfaceKHR& Surface, VkExtent2D Extent);

void DestroyRenderer(const VkDevice& Device, Renderer& vkRenderer);

void Render(const Context& vkContext, const VkRenderPass RenderPass, std::vector<VkFramebuffer> Framebuffers, Renderer& vkRenderer, const RenderGraph& Graph, const VkExtent2D Extent);

bool pushInitCmdBuffer(const Device& D, const Image DepthImage, const Image& ColorImage, const VkCommandPool& Pool);
}
}

#endif // RENDERER_H_