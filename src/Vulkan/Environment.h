#ifndef ENVIRONMENT_H_
#define ENVIRONMENT_H_

#include <vulkan/vulkan.h>
#include "Window/NativeWindow.h"
#include "Resource/Image/Image.h"
#include "vk_mem_alloc.h"

namespace ffGraph {
namespace Vulkan {

struct PhysicalDeviceCapabilities {
    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    VkSampleCountFlagBits msaaSamples;
};

struct Environment {
    struct GPUInformations {
        VkPhysicalDevice PhysicalDevice = VK_NULL_HANDLE;
        PhysicalDeviceCapabilities Capabilities;
        VkDevice Device = VK_NULL_HANDLE;

        static constexpr uint8_t GraphicQueueIndex = 0;
        static constexpr uint8_t PresentQueueIndex = 1;
        static constexpr uint8_t TransferQueueIndex = 2;
        uint32_t QueueIndex[3] = {UINT32_MAX, UINT32_MAX, UINT32_MAX};
        VkQueue Queues[3] = {VK_NULL_HANDLE, VK_NULL_HANDLE, VK_NULL_HANDLE};
    } GPUInfos;

    struct ScreenInformations {
        VkSurfaceKHR Surface = VK_NULL_HANDLE;
        VkSurfaceFormatKHR SurfaceFormat;
        VkPresentModeKHR PresentMode;
        VkSurfaceCapabilitiesKHR SurfaceCapabilities;

        VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
        std::vector<VkImage> Images;
        std::vector<VkImageView> Views;
    } ScreenInfos;

    VmaAllocator Allocator = 0;

    struct GraphicInformations {
        VkRenderPass RenderPass = VK_NULL_HANDLE;
        VkCommandPool CommandPool = VK_NULL_HANDLE;
        VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;

        std::vector<VkFramebuffer> Framebuffers;
        Image DepthImage;
        Image ColorImage;
    } GraphManager;
};

bool CreateEnvironment(Environment& Env, const VkInstance& Instance, const NativeWindow& Window);
void DestroyEnvironment(Environment& Env, const VkInstance& Instance);

bool CreateGraphicInformations(Environment::GraphicInformations& GInfos, const Environment& Env,
                               const NativeWindow& Window);
void DestroyGraphicManager(Environment::GraphicInformations& GInfos, const Environment& Env);

bool CreateScreenInfos(Environment& Env, const NativeWindow& Window);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // ENVIRONMENT_H_