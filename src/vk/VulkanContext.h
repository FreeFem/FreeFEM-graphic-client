#ifndef VULKAN_CONTEXT_H_
#define VULKAN_CONTEXT_H_

#include <vulkan/vulkan.h>
#include "../util/utils.h"
#include "vk_mem_alloc.h"
#include "Image.h"

namespace FEM
{
namespace VK
{

struct QueueInfo {
    uint32_t Index;
    uint32_t TypeFlags;
};

#define SCREENBUFFER_NB 2

struct FrameInfo {
    VkFence Fence;
    VkSemaphore Semaphores[2];
    bool initialize;
};

struct VulkanContext {

    VkInstance Instance;
    uint32_t InstanceExtensionsCount;
    char **InstanceExtensions;

    VmaAllocator Allocator;

    VkPhysicalDevice PhysicalDevice;
    struct {
        VkPhysicalDeviceProperties Properties;
        VkPhysicalDeviceMemoryProperties MemoryProperties;
        VkPhysicalDeviceFeatures Features;
        VkPhysicalDeviceLimits Limits;
    } PhysicalDeviceInfos;

    VkDevice Device;
    uint32_t NumberOfQueue;
    VkQueue Queue;
    QueueInfo QueueInfos;

    VkSurfaceKHR Surface;
    VkSurfaceFormatKHR SurfaceFormat;

    VkSwapchainKHR Swapchain;
    uint32_t SwapchainImageCount;
    VkImage *SwapchainImages;
    VkImageView *SwapchainImageViews;

    Image DepthImage;
    VkCommandPool CommandPool;

    uint8_t CurrentFrame;
    VkCommandBuffer CommandBuffers[SCREENBUFFER_NB + 1];
    FrameInfo FrameInfos[SCREENBUFFER_NB];

#ifdef _DEBUG
    VkDebugReportCallbackEXT DebugReportCallback;
    VkDebugUtilsMessengerEXT DebugMessenger;

    PFN_vkDebugMarkerSetObjectNameEXT DebugMarkerSetObjectNameEXT_PFN;
    PFN_vkCmdDebugMarkerBeginEXT CmdDebugMarkerBeginEXT_PFN;
    PFN_vkCmdDebugMarkerEndEXT CmdDebugMarkerEndEXT_PFN;
    PFN_vkCreateDebugUtilsMessengerEXT CreateDebugUtilsMessengerEXT_PFN;
#endif
};

#ifdef _DEBUG
static const uint32_t LayerCount = 1;
static const char *Layers[] = {
    "VK_LAYER_LUNARG_standard_validation"
};

VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(UNUSED_PARAM VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                      UNUSED_PARAM VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                      UNUSED_PARAM void* pUserData);

VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackEXT(UNUSED_PARAM VkDebugReportFlagsEXT flags,
                                                      UNUSED_PARAM VkDebugReportObjectTypeEXT objectType,
                                                      UNUSED_PARAM uint64_t object, UNUSED_PARAM size_t location,
                                                      UNUSED_PARAM int32_t messageCode,
                                                      UNUSED_PARAM const char *pLayerPrefix, const char *pMessage,
                                                      UNUSED_PARAM void *pUserData);
#endif

bool newVulkanContext(VulkanContext *vkContext, const Window *Win);

void destroyVulkanContext(VulkanContext *vkContext);

} // namespace VK
} // namespace FEM


#endif // VULKAN_CONTEXT_H_