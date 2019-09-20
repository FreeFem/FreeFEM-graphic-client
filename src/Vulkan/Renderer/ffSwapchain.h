#ifndef FF_SWAPCHAIN_H_
#define FF_SWAPCHAIN_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "Window/NativeWindow.h"

namespace ffGraph {
namespace Vulkan {

struct ffSwapchain {
    VkSwapchainKHR Handle;
    VkSurfaceFormatKHR Format;
    std::vector<VkImage> Images;
    std::vector<VkImageView> Views;
};

inline bool ffIsSwapchainReady(ffSwapchain Swaphain) { return (Swaphain.Handle || !Swaphain.Images.empty() || !Swaphain.Views.empty()) ? true : false; }

ffSwapchain ffNewSwapchain(const VkPhysicalDevice& PhysicalDevice, const VkDevice& Device, const VkSurfaceKHR Surface, VkExtent2D Extent);

} // namespace Vulkan
} // namespace ffGraph

#endif // FF_SWAPCHAIN_H_