#include <vector>
#include <cstring>
#include <string>
#include "ffDevice.h"

namespace ffGraph
{
namespace Vulkan
{

static uint32_t getGraphicsFamilyIndex(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface, uint32_t& QueueFamilyIndex) {
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, 0);

    std::vector<VkQueueFamilyProperties> QueueFamilyPropertiesArray(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, QueueFamilyPropertiesArray.data());

    VkBool32 SurfaceSupported = false;
    for (uint32_t i = 0; i < queueCount; i += 1) {
        vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &SurfaceSupported);
        if (SurfaceSupported && QueueFamilyPropertiesArray[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
            QueueFamilyPropertiesArray[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            QueueFamilyIndex = i;
            return i;
        }
        SurfaceSupported = false;
    }
    return VK_QUEUE_FAMILY_IGNORED;
}

static void ffNewPhysicalDevice(const VkInstance Instance, const VkSurfaceKHR Surface, ffDevice& Device)
{
    uint32_t PhysicalDeviceCount = 0;

    Device.PhysicalHandle = VK_NULL_HANDLE;
    if (vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, 0) != VK_SUCCESS)
        return;
    if (PhysicalDeviceCount < 1)
        return;
    std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
    if (vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, PhysicalDevices.data()) != VK_SUCCESS)
        return;

    uint32_t preffered = 0;
    uint32_t fallback = 0;

    for (uint32_t i = 0; i < PhysicalDeviceCount; ++i) {
        VkPhysicalDeviceProperties Props;
        vkGetPhysicalDeviceProperties(PhysicalDevices[i], &Props);

        if (getGraphicsFamilyIndex(PhysicalDevices[i], Surface, Device.QueueIndex) == VK_QUEUE_FAMILY_IGNORED) continue;

        if (!preffered && Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) preffered = i;
        if (!fallback) fallback = i;
    }

    uint32_t result = preffered ? preffered : fallback;

    Device.PhysicalHandle = PhysicalDevices[result];
    vkGetPhysicalDeviceProperties(Device.PhysicalHandle, &Device.PhysicalHandleCapabilities.Properties);
    vkGetPhysicalDeviceFeatures(Device.PhysicalHandle, &Device.PhysicalHandleCapabilities.Features);
    vkGetPhysicalDeviceMemoryProperties(Device.PhysicalHandle, &Device.PhysicalHandleCapabilities.MemoryProperties);
}

ffDevice ffNewDevice(const VkInstance Instance, const VkSurfaceKHR Surface, std::vector<std::string> Layers)
{
    ffDevice n;

    memset(&n, 0, sizeof(ffDevice));
    ffNewPhysicalDevice(Instance, Surface, n);
    if (n.PhysicalHandle == VK_NULL_HANDLE)
        return n;
    float QueuePriority = 1.f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = n.QueueIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &QueuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &n.PhysicalHandleCapabilities.Features;

    std::vector<const char *> Extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    deviceCreateInfo.enabledExtensionCount = Extensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = Extensions.data();

    std::vector<const char *> enabledLayers(Layers.size());
    for (auto const& layer : Layers) {
        enabledLayers.push_back(layer.data());
    }
#ifdef _DEBUG
    if (std::find(Layers.begin(), eLayers.end(), "VK_LAYER_KHRONOS_Validation") == Layers.end())
        enabledLayers.push_back("VK_LAYER_KHRONOS_Validation");
#endif
    deviceCreateInfo.enabledLayerCount = enabledLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

    if (vkCreateDevice(n.PhysicalHandle, &deviceCreateInfo, 0, &n.Handle))
        return n;
    vkGetDeviceQueue(n.Handle, n.QueueIndex, 0, &n.Queue);
    return n;
}

} // namespace Vulkan
} // namespace ffGraph
