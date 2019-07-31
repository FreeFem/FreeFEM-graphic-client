#include <cstring>
#include <vector>
#include <stdexcept>
#include "context.h"

static bool createVkInstance(VulkanContext *context)
{
    VkApplicationInfo appInfo;

    memset(&appInfo, 0, sizeof(VkApplicationInfo));
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(VkInstanceCreateInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

#ifdef _DEBUG
    const char *debugLayers[] = {
        "VK_LAYER_LUNARG_standard_validation"
    };
    createInfo.ppEnabledLayerNames = debugLayers;
    createInfo.enabledLayerCount = sizeof(debugLayers) / sizeof(debugLayers[0]);
#endif
    const char *extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef VK_USE_PLATFORM_WIN32_KHR
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
#ifdef _DEBUG
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
    };
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);

    if (vkCreateInstance(&createInfo, 0, &context->Instance) != VK_SUCCESS)
        return false;
    volkLoadInstance(context->Instance);
    return true;
}

static uint32_t getGraphicsFamilyIndex(VkPhysicalDevice physicalDevice)
{
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, 0);

    std::vector<VkQueueFamilyProperties> queues(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queues.data());

    for (uint32_t i = 0; i < queueCount; ++i) {
        if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            return i;
    }
    return VK_QUEUE_FAMILY_IGNORED;
}

static bool supportsPresentation(VkPhysicalDevice physicalDevice, uint32_t familyIndex)
{
#ifdef defined(VK_USE_PLATFORM_WIN32_KHR)
    return !!vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, familyIndex);
#else
    return true;
#endif
}

static bool pickPhysicalDevice(VulkanContext *context)
{
    uint32_t physicalDevicesCount = 0;
    vkEnumeratePhysicalDevices(context->Instance, &physicalDevicesCount, 0);

    if (physicalDevicesCount == 0)
        throw std::runtime_error("Error: failed to find GPUs with Vulkan support.");
    std::vector<VkPhysicalDevice> physicalDevices(physicalDevicesCount);
    vkEnumeratePhysicalDevices(context->Instance, &physicalDevicesCount, physicalDevices.data());

    VkPhysicalDevice preffered = 0;
    VkPhysicalDevice fallback = 0;
    for (uint32_t i = 0; i < physicalDevicesCount; ++i) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physicalDevices[i], &props);

        uint32_t familyIndex = getGraphicsFamilyIndex(physicalDevices[i]);
        if (familyIndex == VK_QUEUE_FAMILY_IGNORED)
            continue;
        if (!supportsPresentation(physicalDevices[i], familyIndex))
            continue;
        if (!preffered && props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
            preffered = physicalDevices[i];
        if (!fallback)
            fallback = physicalDevices[i];
    }
    VkPhysicalDevice result = preffered ? preffered : fallback;

    if (result) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(result, &props);

        context->PhysicalDevice = result;
        context->FamilyIndex = getGraphicsFamilyIndex(result);
        printf("Selected GPU %s\n", props.deviceName);
    } else {
        throw std::runtime_error("Error: No suitable GPUs found.");
    }
    return true;
}

static bool createDeviceAndQueue(VulkanContext *context)
{
    VkDeviceQueueCreateInfo queueCreateInfo;
    memset(&queueCreateInfo, 0, sizeof(VkDeviceQueueCreateInfo));
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = context->FamilyIndex;
    queueCreateInfo.queueCount = 1;

    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures;
    memset(&deviceFeatures, 0, sizeof(VkPhysicalDeviceFeatures));

    VkDeviceCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(VkDeviceCreateInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;

#ifdef _DEBUG
    const char *debugLayers[] = {
        "VK_LAYER_KHRONOS_validation"
    };
    createInfo.ppEnabledLayerNames = debugLayers;
    createInfo.enabledLayerCount = sizeof(debugLayers) / sizeof(debugLayers[0]);
#endif

    if (vkCreateDevice(context->PhysicalDevice, &createInfo, 0, &context->Device) != VK_SUCCESS)
        return false;
    vkGetDeviceQueue(context->Device, context->FamilyIndex, 0, &context->Queue);
    return true;
}

bool createVulkanContext(VulkanContext *context)
{
    if (context == 0)
        return false;
    if (!createVkInstance(context))
        return false;
    if (!pickPhysicalDevice(context))
        return false;
    if (!createDeviceAndQueue(context))
        return false;
    return true;
}

void destroyVulkanContext(VulkanContext context)
{
    vkDestroyDevice(context.Device, 0);
    vkDestroyInstance(context.Instance, 0);
}