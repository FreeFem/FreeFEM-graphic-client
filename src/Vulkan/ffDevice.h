#ifndef ff_DEVICE_H_
#define ff_DEVICE_H_

#include <vulkan/vulkan.h>

namespace ffGraph {
namespace Vulkan {

struct ffPhysicalDeviceCapabilities {
    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
};

struct ffDevice {
    VkPhysicalDevice PhysicalHandle;
    ffPhysicalDeviceCapabilities PhysicalHandleCapabilities;

    VkDevice Handle;
    uint32_t QueueIndex;
    VkQueue Queue;
};

ffDevice ffNewDevice(const VkInstance Instance, const VkSurfaceKHR Surface, std::vector<std::string> Layers);

} // namespace Vulkan
} // namespace ffGraph


#endif // ff_DEVICE_H_