#include <vector>
#include <cstring>
#include <string>
#include <algorithm>
#include <iostream>
#include "Device.h"

namespace ffGraph
{
namespace Vulkan
{

static bool getQueueFamilyIndices(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface, uint32_t *QueueFamilyIndices)
{
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, 0);

    std::vector<VkQueueFamilyProperties> QueueFamilyPropertiesArray(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, QueueFamilyPropertiesArray.data());

    VkBool32 SurfaceSupported = false;
    int count = 0;
    for (uint32_t i = 0; i < queueCount; i += 1) {
        vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &SurfaceSupported);

        if (SurfaceSupported&& QueueFamilyIndices[1] == UINT32_MAX) {
            QueueFamilyIndices[1] = 1;
            ++count;
        }
        if (QueueFamilyPropertiesArray[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && QueueFamilyIndices[0] == UINT32_MAX) {
            QueueFamilyIndices[0] = i;
            ++count;
        } else if (QueueFamilyPropertiesArray[i].queueFlags & VK_QUEUE_TRANSFER_BIT && !(QueueFamilyPropertiesArray[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) && QueueFamilyIndices[1] == UINT32_MAX) {
            QueueFamilyIndices[2] = i;
            ++count;
        }
        if (count == 3)
            return true;
        SurfaceSupported = false;
    }
    return false;
}

static VkSampleCountFlagBits getMaxUsableSampleCount(Device& Device)
{
    VkSampleCountFlags count = std::min(Device.PhysicalHandleCapabilities.Properties.limits.framebufferColorSampleCounts,
                                        Device.PhysicalHandleCapabilities.Properties.limits.framebufferDepthSampleCounts);
    if (count & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (count & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (count & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (count & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (count & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (count & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

    return VK_SAMPLE_COUNT_1_BIT;
}

static void NewPhysicalDevice(const VkInstance Instance, const VkSurfaceKHR Surface, Device& Device)
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

        if (getQueueFamilyIndices(PhysicalDevices[i], Surface, Device.QueueIndex) == false) continue;

        if (!preffered && Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) preffered = i;
        if (!fallback) fallback = i;
    }

    uint32_t result = preffered ? preffered : fallback;

    Device.PhysicalHandle = PhysicalDevices[result];
    vkGetPhysicalDeviceProperties(Device.PhysicalHandle, &Device.PhysicalHandleCapabilities.Properties);
    vkGetPhysicalDeviceFeatures(Device.PhysicalHandle, &Device.PhysicalHandleCapabilities.Features);
    vkGetPhysicalDeviceMemoryProperties(Device.PhysicalHandle, &Device.PhysicalHandleCapabilities.MemoryProperties);
    Device.PhysicalHandleCapabilities.msaaSamples = getMaxUsableSampleCount(Device);
}

Device NewDevice(const VkInstance Instance, const VkSurfaceKHR Surface, std::vector<std::string> Layers)
{
    Device n;

    memset(&n, 0, sizeof(Device));
    NewPhysicalDevice(Instance, Surface, n);
    if (n.PhysicalHandle == VK_NULL_HANDLE)
        return n;
    float QueuePriority[2] = {1.f, 0.5f};
    uint32_t count = 0;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    std::vector<VkDeviceQueueCreateInfo> QueueCreateInfo = {};

    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = n.QueueIndex[DEVICE_GRAPH_QUEUE];
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &QueuePriority[0];
    QueueCreateInfo.push_back(queueCreateInfo);
    if (n.QueueIndex[DEVICE_GRAPH_QUEUE] != n.QueueIndex[DEVICE_PRESENT_QUEUE]) {

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = n.QueueIndex[DEVICE_PRESENT_QUEUE];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &QueuePriority[0];
        QueueCreateInfo.push_back(queueCreateInfo);
    }
    if (n.QueueIndex[DEVICE_GRAPH_QUEUE] != n.QueueIndex[DEVICE_TRANS_QUEUE] && n.QueueIndex[DEVICE_PRESENT_QUEUE] != n.QueueIndex[DEVICE_TRANS_QUEUE]){

        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = n.QueueIndex[DEVICE_TRANS_QUEUE];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &QueuePriority[1];
        QueueCreateInfo.push_back(queueCreateInfo);
    }

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = QueueCreateInfo.size();
    deviceCreateInfo.pQueueCreateInfos = QueueCreateInfo.data();
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
    if (std::find(Layers.begin(), Layers.end(), "VK_LAYER_KHRONOS_Validation") == Layers.end())
        enabledLayers.push_back("VK_LAYER_KHRONOS_Validation");
#endif
    deviceCreateInfo.enabledLayerCount = enabledLayers.size();
    deviceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

    if (vkCreateDevice(n.PhysicalHandle, &deviceCreateInfo, 0, &n.Handle))
        return n;
    vkGetDeviceQueue(n.Handle, n.QueueIndex[DEVICE_GRAPH_QUEUE], 0, &n.Queue[DEVICE_GRAPH_QUEUE]);
    vkGetDeviceQueue(n.Handle, n.QueueIndex[DEVICE_PRESENT_QUEUE], 0, &n.Queue[DEVICE_PRESENT_QUEUE]);
    vkGetDeviceQueue(n.Handle, n.QueueIndex[DEVICE_TRANS_QUEUE], 0, &n.Queue[DEVICE_TRANS_QUEUE]);
    return n;
}

} // namespace Vulkan
} // namespace ffGraph
