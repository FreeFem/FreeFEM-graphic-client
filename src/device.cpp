#include <vulkan/vulkan.h>
#include <vector>
#include <cstring>
#include "utils.h"
#include "instance.h"

bool createDeviceAndQueue(const VkPhysicalDevice phyDev, const VkSurfaceKHR surface, const std::vector<const char *>& enabledLayers, VkDevice *outDevice, VkQueue *outQueue, uint32_t *outQueueFamilyIndex)
{
    VkResult result;

    uint32_t deviceExtensionCount = 0;
    result = vkEnumerateDeviceExtensionProperties(phyDev, 0, &deviceExtensionCount, 0);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to get number of devices. [%s]\n", VkResultToStr(result));
        return false;
    } else if (deviceExtensionCount == 0) {
        dprintf(2, "Didn't find any devices.\n");
        return false;
    }
    std::vector<VkExtensionProperties> extensionProperties(deviceExtensionCount);
    result = vkEnumerateDeviceExtensionProperties(phyDev, 0, &deviceExtensionCount, extensionProperties.data());
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to get devices list. [%s]\n", VkResultToStr(result));
        return false;
    }

    bool hasSwapchainExtension = false;
    for (const auto& extProp : extensionProperties) {
        if (strcmp(extProp.extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME) == 0) {
            hasSwapchainExtension = true;
            break;
        }
    }
    if (!hasSwapchainExtension) {
        dprintf(2, "Chosen physical doesn't support VK_KHR_SWAPCHAIN_EXTENSION_NAME.\n");
        return false;
    }

    familyIndex indices = findQueueFamilies(phyDev, surface);
    if (!isDeviceSuitable(phyDev, surface)) {
        dprintf(2, "Physical device doesn't the right queue families.\n");
        return false;
    }

    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = 0;
    queueCreateInfo.flags = 0;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.queueFamilyIndex = indices.presentFamily;

    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    vkGetPhysicalDeviceFeatures(phyDev, &physicalDeviceFeatures);

    std::vector<const char *> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.flags = 0;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();
    createInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
    createInfo.ppEnabledLayerNames = enabledLayers.data();

    result = vkCreateDevice(phyDev, &createInfo, 0, outDevice);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to create device. [%s]\n", VkResultToStr(result));
        return false;
    }
    vkGetDeviceQueue(*outDevice, indices.presentFamily, 0, outQueue);
    *outQueueFamilyIndex = indices.presentFamily;
    return true;
}