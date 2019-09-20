#ifndef FF_RENDERER_H_
#define FF_RENDERER_H_

#include <array>
#include <vulkan/vulkan.h>
#include "Context/ffDevice.h"
#include "../../ResourceManager/Image/ffImage.h"
#include "ffSwapchain.h"

namespace ffGraph {
namespace Vulkan {

struct ffRenderer {
    VkQueue *Queue_REF;

    VkCommandPool CommandPool;
    ffSwapchain Swapchain;

    VkImage DepthImage;
    VkImageView DepthImageView;
    VkDeviceMemory DepthImageMemory;

    std::array<VkCommandBuffer, 2> CommandBuffers;
    std::array<VkFence, 2> Fences;
    std::array<VkSemaphore, 4> Semaphores;
};

inline bool ffIsRendererReady(ffRenderer Renderer) { return (ffIsSwapchainReady(Renderer.Swapchain) || Renderer.CommandPool || Renderer.DepthImage) ? true : false; }

ffRenderer ffNewRenderer(const VkPhysicalDevice& PhysicalDevice, ffDevice& Device, const VkSurfaceKHR& Surface, VkExtent2D Extent);

}
}

#endif // FF_RENDERER_H_