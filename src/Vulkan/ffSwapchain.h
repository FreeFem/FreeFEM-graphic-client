#ifndef FF_SWAPCHAIN_H_
#define FF_SWAPCHAIN_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "Window/NativeWindow.h"

namespace ffGraph
{
namespace Vulkan
{

struct ffSwapchain {
    VkSwapchainKHR Handle;
    VkSurfaceFormatKHR Format;
    std::vector<VkImage> Images;
    std::vector<VkImageView> Views;
};

ffSwapchain ffNewSwapchain(const VkDevice& Device, const NativeWindow& Window);

} // namespace Vulkan
} // namespace ffGraph

#endif // FF_SWAPCHAIN_H_