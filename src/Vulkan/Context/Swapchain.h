#ifndef SWAPCHAIN_H_
#define SWAPCHAIN_H_

#include <vulkan/vulkan.h>
#include <vector>

namespace ffGraph {
namespace Vulkan {

struct Swapchain {
    VkSwapchainKHR Handle;
    VkSurfaceFormatKHR Format;
    std::vector<VkImage> Images;
    std::vector<VkImageView> Views;
};

Swapchain newSwapchain(const Device& Device, const VkSurfaceKHR Surface, VkExtent2D Extent);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // SWAPCHAIN_H_