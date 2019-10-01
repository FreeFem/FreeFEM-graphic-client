#ifndef ff_DEVICE_H_
#define ff_DEVICE_H_

#include <vulkan/vulkan.h>

namespace ffGraph {
namespace Vulkan {

struct PhysicalDeviceCapabilities {
    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;
    VkPhysicalDeviceMemoryProperties MemoryProperties;
    VkSampleCountFlagBits msaaSamples;
};

#define DEVICE_GRAPH_QUEUE 0
#define DEVICE_PRESENT_QUEUE 1
#define DEVICE_TRANS_QUEUE 2

struct Device {
    VkPhysicalDevice PhysicalHandle;
    PhysicalDeviceCapabilities PhysicalHandleCapabilities;

    VkDevice Handle;
    uint32_t QueueIndex[3] = {UINT32_MAX, UINT32_MAX, UINT32_MAX};
    VkQueue Queue[3];
};

Device NewDevice(const VkInstance Instance, const VkSurfaceKHR Surface, std::vector<std::string> Layers);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // ff_DEVICE_H_