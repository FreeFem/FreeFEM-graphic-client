#include <vector>
#include <string>
#include <algorithm>
#include "Context.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

bool newContext(Context& vkContext, const VkInstance& Instance, const NativeWindow& Window) {
    vkContext.Surface = ffGetSurface(Instance, Window);
    if (vkContext.Surface == VK_NULL_HANDLE) {
        LogError(GetCurrentLogLocation( ), "Couldn't create the VkSurfaceKHR.");
        return false;
    }
    vkContext.vkDevice = NewDevice(Instance, vkContext.Surface, {});
    if (vkContext.vkDevice.Handle == VK_NULL_HANDLE || vkContext.vkDevice.PhysicalHandle == VK_NULL_HANDLE) {
        LogError(GetCurrentLogLocation( ), "Couldn't create the VkPhysicalDevice and VkDevice.");
        return false;
    }
    vkContext.vkSwapchain = newSwapchain(vkContext.vkDevice, vkContext.Surface, Window.WindowSize);
    if (vkContext.vkSwapchain.Handle == VK_NULL_HANDLE) {
        LogError(GetCurrentLogLocation( ), "Couldn't create the VkSwapchainKHR.");
        return false;
    }
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext.vkDevice.PhysicalHandle, vkContext.Surface, &formatCount, 0);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext.vkDevice.PhysicalHandle, vkContext.Surface, &formatCount,
                                         formats.data( ));

    VkFormat finalFormat;
    if (formatCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED) {
        finalFormat = VK_FORMAT_R8G8B8A8_UNORM;
    }
    for (uint32_t i = 0; i < formatCount; ++i) {
        if (formats[i].format == VK_FORMAT_R8G8B8A8_UNORM || formats[i].format == VK_FORMAT_B8G8R8A8_UNORM)
            finalFormat = formats[i].format;
    }
    vkContext.SurfaceFormat.format = finalFormat;
    vkContext.SurfaceFormat.colorSpace = formats[0].colorSpace;
    return true;
}

void destroyContext(Context vkContext, const VkInstance& Instance) {
    for (uint32_t i = 0; i < vkContext.vkSwapchain.Views.size( ); ++i) {
        vkDestroyImageView(vkContext.vkDevice.Handle, vkContext.vkSwapchain.Views[i], 0);
    }
    vkDestroySwapchainKHR(vkContext.vkDevice.Handle, vkContext.vkSwapchain.Handle, 0);
    vkDestroySurfaceKHR(Instance, vkContext.Surface, 0);
    vkDestroyDevice(vkContext.vkDevice.Handle, 0);
}

}    // namespace Vulkan
}    // namespace ffGraph