#ifndef ff_DEVICE_H_
#define ff_DEVICE_H_

#include <vulkan/vulkan.h>

namespace ffGraph {
namespace Vulkan {

struct PhysicalDeviceCapabilities {
    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
};

struct Device {
    VkPhysicalDevice PhysicalHandle;
    PhysicalDeviceCapabilities PhysicalHandleCapabilities;

    VkDevice Handle;
    uint32_t QueueIndex;
    VkQueue Queue;
};

Device NewDevice(const VkInstance Instance, const VkSurfaceKHR Surface, std::vector<std::string> Layers);

} // namespace Vulkan
} // namespace ffGraph


#endif // ff_DEVICE_H_