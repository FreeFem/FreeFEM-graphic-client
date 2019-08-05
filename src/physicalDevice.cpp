#include <vulkan/vulkan.h>
#include <cstdio>
#include "utils.h"

static void showPhysicalDeviceInfos(VkPhysicalDeviceProperties phyDevProp, int i)
{
    printf("- Found physical device: %s (N %d)\n", phyDevProp.deviceName, i);
}

familyIndex findQueueFamilies(VkPhysicalDevice phyDev, const VkSurfaceKHR surface)
{
    familyIndex f = {false, 0, false, 0};
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &queueFamilyCount, 0);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(phyDev, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            f.has_graphicFamily = true;
            f.graphicFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(phyDev, i, surface, &presentSupport);

        if (queueFamily.queueCount > 0 && presentSupport) {
            f.has_presentFamily = true;
            f.presentFamily = i;
        }
        if (f.has_graphicFamily == true && f.has_presentFamily == true)
            break;
    }
    return f;
}

bool isDeviceSuitable(VkPhysicalDevice phyDev, const VkSurfaceKHR surface)
{
    familyIndex indices = findQueueFamilies(phyDev, surface);

    return indices.has_graphicFamily && indices.has_presentFamily;
}

bool chooseVkPhysicalDevice(const VkInstance instance, VkPhysicalDevice &outPhysicalDevice, const VkSurfaceKHR surface)
{
    VkResult result;
    uint32_t physicalDevicesCount = 0;
    result = vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, 0);

    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to get number of physical devices. [%s]\n", VkResultToStr(result));
        return false;
    } else if (physicalDevicesCount == 0) {
        dprintf(2, "Didn't find any physical devices with Vulkan support.\n");
        return false;
    }

    std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
    vkEnumeratePhysicalDevices(instance, &physicalDevicesCount, physicalDevices.data());
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to get physical devices list. [%s]\n", VkResultToStr(result));
        return false;
    }

    int deviceIndex = 0;
    for (const auto &phyDev : physicalDevices) {
        VkPhysicalDeviceProperties phyDevProp;
        vkGetPhysicalDeviceProperties(phyDev, &phyDevProp);

        showPhysicalDeviceInfos(phyDevProp, deviceIndex);

        if (isDeviceSuitable(phyDev, surface)) {
            outPhysicalDevice = phyDev;
            return true;
        }
        if (phyDev == VK_NULL_HANDLE) {
            dprintf(2, "Failed to find suitable GPU.\n");
            return false;
        }
    }
    dprintf(2, "Failed to find suitable GPU.\n");
    return false;
}