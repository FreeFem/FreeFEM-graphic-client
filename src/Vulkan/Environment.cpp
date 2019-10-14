#include <array>
#include "Environment.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static bool getQueueFamilyIndices(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface, uint32_t* QueueFamilyIndices) {
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, 0);

    std::vector<VkQueueFamilyProperties> QueueFamilyPropertiesArray(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, QueueFamilyPropertiesArray.data( ));

    std::cout << "Nb of Queue : " << queueCount << "\n";
    VkBool32 SurfaceSupported = false;
    int count = 0;
    for (uint32_t i = 0; i < queueCount; i += 1) {
        vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &SurfaceSupported);

        if (SurfaceSupported && QueueFamilyIndices[1] == UINT32_MAX) {
            QueueFamilyIndices[1] = i;
            ++count;
        }
        if (QueueFamilyPropertiesArray[i].queueFlags & VK_QUEUE_GRAPHICS_BIT && QueueFamilyIndices[0] == UINT32_MAX) {
            QueueFamilyIndices[0] = i;
            ++count;
        } else if (QueueFamilyPropertiesArray[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            QueueFamilyIndices[2] = i;
            ++count;
        }
        if (count == 3) return true;
        SurfaceSupported = false;
    }
    return false;
}

static VkSampleCountFlagBits getMaxUsableSampleCount(const VkPhysicalDeviceLimits Limits) {
    VkSampleCountFlags count = std::min(Limits.framebufferColorSampleCounts, Limits.framebufferDepthSampleCounts);
    if (count & VK_SAMPLE_COUNT_64_BIT) {
        return VK_SAMPLE_COUNT_64_BIT;
    }
    if (count & VK_SAMPLE_COUNT_32_BIT) {
        return VK_SAMPLE_COUNT_32_BIT;
    }
    if (count & VK_SAMPLE_COUNT_16_BIT) {
        return VK_SAMPLE_COUNT_16_BIT;
    }
    if (count & VK_SAMPLE_COUNT_8_BIT) {
        return VK_SAMPLE_COUNT_8_BIT;
    }
    if (count & VK_SAMPLE_COUNT_4_BIT) {
        return VK_SAMPLE_COUNT_4_BIT;
    }
    if (count & VK_SAMPLE_COUNT_2_BIT) {
        return VK_SAMPLE_COUNT_2_BIT;
    }

    return VK_SAMPLE_COUNT_1_BIT;
}

static bool NewPhysicalDevice(Environment& Env, const VkInstance& Instance, const VkSurfaceKHR& Surface) {
    uint32_t PhysicalDeviceCount = 0;

    if (vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, 0) != VK_SUCCESS) return false;
    if (PhysicalDeviceCount < 1) return false;
    std::vector<VkPhysicalDevice> PhysicalDevices(PhysicalDeviceCount);
    if (vkEnumeratePhysicalDevices(Instance, &PhysicalDeviceCount, PhysicalDevices.data( )) != VK_SUCCESS) return false;

    uint32_t preffered = 0;
    uint32_t fallback = 0;

    for (uint32_t i = 0; i < PhysicalDeviceCount; ++i) {
        VkPhysicalDeviceProperties Props;
        vkGetPhysicalDeviceProperties(PhysicalDevices[i], &Props);

        if (getQueueFamilyIndices(PhysicalDevices[i], Surface, Env.GPUInfos.QueueIndex) == false) continue;

        if (!preffered && Props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) preffered = i;
        if (!fallback) fallback = i;
    }

    uint32_t result = preffered ? preffered : fallback;

    Env.GPUInfos.PhysicalDevice = PhysicalDevices[result];
    vkGetPhysicalDeviceProperties(Env.GPUInfos.PhysicalDevice, &Env.GPUInfos.Capabilities.Properties);
    vkGetPhysicalDeviceFeatures(Env.GPUInfos.PhysicalDevice, &Env.GPUInfos.Capabilities.Features);
    vkGetPhysicalDeviceMemoryProperties(Env.GPUInfos.PhysicalDevice, &Env.GPUInfos.Capabilities.MemoryProperties);
    Env.GPUInfos.Capabilities.msaaSamples = getMaxUsableSampleCount(Env.GPUInfos.Capabilities.Properties.limits);
    return true;
}

static VkSurfaceFormatKHR GetSurfaceFormat(const VkPhysicalDevice& PhysicalDevice, const VkSurfaceKHR& Surface) {
    uint32_t count = 0;
    VkResult res;

    res = vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &count, 0);
    if (count == 0 || res) return {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR};
    std::vector<VkSurfaceFormatKHR> SurfaceFormats(count);
    vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &count, SurfaceFormats.data( ));

    for (uint32_t i = 0; i < SurfaceFormats.size( ); ++i) {
        if (SurfaceFormats[i].format == VK_FORMAT_B8G8R8A8_UNORM &&
            SurfaceFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return SurfaceFormats[i];
    }
    return SurfaceFormats[0];
}

static VkPresentModeKHR GetPresentMode(const VkPhysicalDevice& PhysicalDevice, const VkSurfaceKHR& Surface) {
    uint32_t count = 0;
    VkResult res;

    res = vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &count, 0);
    if (count == 0 || res) return {VK_PRESENT_MODE_MAX_ENUM_KHR};
    std::vector<VkPresentModeKHR> PresentModes(count);
    vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &count, PresentModes.data( ));

    for (uint32_t i = 0; i < PresentModes.size( ); ++i) {
        if (PresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) return PresentModes[i];
    }
    return PresentModes[0];
}

static bool GetSurfaceCapabilities(const VkPhysicalDevice& PhysicalDevice, const VkSurfaceKHR& Surface,
                                   VkSurfaceCapabilitiesKHR& Capabilities) {
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &Capabilities)) return false;
    return true;
}

static bool CreateDeviceInfos(Environment& Env, const VkInstance& Instance) {
    if (NewPhysicalDevice(Env, Instance, Env.ScreenInfos.Surface) == false) return false;
    Env.ScreenInfos.SurfaceFormat = GetSurfaceFormat(Env.GPUInfos.PhysicalDevice, Env.ScreenInfos.Surface);
    if (Env.ScreenInfos.SurfaceFormat.format == VK_FORMAT_UNDEFINED &&
        Env.ScreenInfos.SurfaceFormat.colorSpace == VK_COLOR_SPACE_MAX_ENUM_KHR)
        return false;
    if (GetSurfaceCapabilities(Env.GPUInfos.PhysicalDevice, Env.ScreenInfos.Surface,
                               Env.ScreenInfos.SurfaceCapabilities) == false)
        return false;
    Env.ScreenInfos.PresentMode = GetPresentMode(Env.GPUInfos.PhysicalDevice, Env.ScreenInfos.Surface);
    if (Env.ScreenInfos.PresentMode == VK_PRESENT_MODE_MAX_ENUM_KHR) return false;

    float QueuePriority = 1.0f;
    uint32_t Count = 0;
    std::array<VkDeviceQueueCreateInfo, 3> QueueCreateInfo = {};

    QueueCreateInfo[0].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueCreateInfo[0].queueFamilyIndex = Env.GPUInfos.QueueIndex[Env.GPUInfos.GraphicQueueIndex];
    QueueCreateInfo[0].queueCount = 1;
    QueueCreateInfo[0].pQueuePriorities = &QueuePriority;
    Count += 1;
    if (Env.GPUInfos.QueueIndex[Env.GPUInfos.GraphicQueueIndex] !=
        Env.GPUInfos.QueueIndex[Env.GPUInfos.PresentQueueIndex] &&
        Env.GPUInfos.QueueIndex[Env.GPUInfos.PresentQueueIndex] != UINT32_MAX) {
        QueueCreateInfo[1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueCreateInfo[1].queueFamilyIndex = Env.GPUInfos.QueueIndex[Env.GPUInfos.PresentQueueIndex];
        QueueCreateInfo[1].queueCount = 1;
        QueueCreateInfo[1].pQueuePriorities = &QueuePriority;
        Count += 1;
    }

    if (Env.GPUInfos.QueueIndex[Env.GPUInfos.TransferQueueIndex] !=
            Env.GPUInfos.QueueIndex[Env.GPUInfos.PresentQueueIndex] &&
        Env.GPUInfos.QueueIndex[Env.GPUInfos.TransferQueueIndex] !=
            Env.GPUInfos.QueueIndex[Env.GPUInfos.GraphicQueueIndex] &&
        Env.GPUInfos.QueueIndex[Env.GPUInfos.TransferQueueIndex] != UINT32_MAX) {
        QueueCreateInfo[2].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueCreateInfo[2].queueFamilyIndex = Env.GPUInfos.QueueIndex[Env.GPUInfos.TransferQueueIndex];
        QueueCreateInfo[2].queueCount = 1;
        QueueCreateInfo[2].pQueuePriorities = &QueuePriority;
        Count += 1;
    } else {
        Env.GPUInfos.QueueIndex[Env.GPUInfos.TransferQueueIndex] = Env.GPUInfos.QueueIndex[Env.GPUInfos.GraphicQueueIndex];
    }
    VkDeviceCreateInfo DeviceCreateInfo = {};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.pEnabledFeatures = &Env.GPUInfos.Capabilities.Features;
    DeviceCreateInfo.queueCreateInfoCount = Count;
    DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfo.data( );

    std::vector<const char*> Extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };
    DeviceCreateInfo.enabledExtensionCount = Extensions.size( );
    DeviceCreateInfo.ppEnabledExtensionNames = Extensions.data( );

    std::vector<const char*> enabledLayers = {
#ifdef _DEBUG
        "VK_LAYER_KHRONOS_Validation",
#endif
    };

    DeviceCreateInfo.enabledLayerCount = enabledLayers.size( );
    DeviceCreateInfo.ppEnabledLayerNames = enabledLayers.data( );

    if (vkCreateDevice(Env.GPUInfos.PhysicalDevice, &DeviceCreateInfo, 0, &Env.GPUInfos.Device)) return false;

    vkGetDeviceQueue(Env.GPUInfos.Device, Env.GPUInfos.QueueIndex[Env.GPUInfos.GraphicQueueIndex], 0,
                     &Env.GPUInfos.Queues[Env.GPUInfos.GraphicQueueIndex]);
    vkGetDeviceQueue(Env.GPUInfos.Device, Env.GPUInfos.QueueIndex[Env.GPUInfos.PresentQueueIndex], 0,
                     &Env.GPUInfos.Queues[Env.GPUInfos.PresentQueueIndex]);
    vkGetDeviceQueue(Env.GPUInfos.Device, Env.GPUInfos.QueueIndex[Env.GPUInfos.TransferQueueIndex], 0,
                     &Env.GPUInfos.Queues[Env.GPUInfos.TransferQueueIndex]);
    return true;
}

VkSurfaceKHR CreateSurface(const VkInstance& Instance, const NativeWindow& Window) {
    return ffGetSurface(Instance, Window);
}

static bool CreateSwapchainImageView(std::vector<VkImageView>& Views, const std::vector<VkImage>& Images,
                                     const VkDevice& Device, const VkFormat format) {
    Views.resize(Images.size( ));

    VkImageViewCreateInfo CreateInfos = {};
    CreateInfos.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    CreateInfos.viewType = VK_IMAGE_VIEW_TYPE_2D;
    CreateInfos.format = format;
    CreateInfos.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    CreateInfos.subresourceRange.baseArrayLayer = 0;
    CreateInfos.subresourceRange.levelCount = 1;
    CreateInfos.subresourceRange.baseArrayLayer = 0;
    CreateInfos.subresourceRange.layerCount = 1;

    for (size_t i = 0; i < Images.size( ); ++i) {
        CreateInfos.image = Images[i];

        if (vkCreateImageView(Device, &CreateInfos, 0, &Views[i])) {
            LogError(GetCurrentLogLocation( ), "Failed to create swapchain VkImageView.");
            return false;
        }
    }
    return true;
}

bool CreateScreenInfos(Environment& Env, const NativeWindow& Window) {
    uint32_t ImageCount = Env.ScreenInfos.SurfaceCapabilities.minImageCount + 1;
    if (Env.ScreenInfos.SurfaceCapabilities.maxImageCount > 0 &&
        ImageCount < Env.ScreenInfos.SurfaceCapabilities.maxImageCount)
        ImageCount = Env.ScreenInfos.SurfaceCapabilities.maxImageCount;

    VkSwapchainCreateInfoKHR SwapchainCreateInfos = {};
    SwapchainCreateInfos.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfos.surface = Env.ScreenInfos.Surface;
    SwapchainCreateInfos.minImageCount = ImageCount;
    SwapchainCreateInfos.imageFormat = Env.ScreenInfos.SurfaceFormat.format;
    SwapchainCreateInfos.imageColorSpace = Env.ScreenInfos.SurfaceFormat.colorSpace;
    SwapchainCreateInfos.imageExtent = Window.WindowSize;
    SwapchainCreateInfos.imageArrayLayers = 1;
    SwapchainCreateInfos.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (Env.GPUInfos.QueueIndex[Env.GPUInfos.GraphicQueueIndex] !=
        Env.GPUInfos.QueueIndex[Env.GPUInfos.PresentQueueIndex]) {
        SwapchainCreateInfos.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        SwapchainCreateInfos.queueFamilyIndexCount = 2;
        SwapchainCreateInfos.pQueueFamilyIndices = Env.GPUInfos.QueueIndex;
    } else {
        SwapchainCreateInfos.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    SwapchainCreateInfos.preTransform = Env.ScreenInfos.SurfaceCapabilities.currentTransform;
    SwapchainCreateInfos.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfos.presentMode = Env.ScreenInfos.PresentMode;
    SwapchainCreateInfos.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(Env.GPUInfos.Device, &SwapchainCreateInfos, 0, &Env.ScreenInfos.Swapchain)) return false;
    ImageCount = 0;
    vkGetSwapchainImagesKHR(Env.GPUInfos.Device, Env.ScreenInfos.Swapchain, &ImageCount, 0);
    Env.ScreenInfos.Images.resize(ImageCount);
    if (vkGetSwapchainImagesKHR(Env.GPUInfos.Device, Env.ScreenInfos.Swapchain, &ImageCount,
                                Env.ScreenInfos.Images.data( )))
        return false;
    CreateSwapchainImageView(Env.ScreenInfos.Views, Env.ScreenInfos.Images, Env.GPUInfos.Device,
                             Env.ScreenInfos.SurfaceFormat.format);
    return true;
}

bool CreateEnvironment(Environment& Env, const VkInstance& Instance, const NativeWindow& Window) {
    LogInfo("CreateEnvironment", "Creating Vulkan environment.");

    Env.ScreenInfos.Surface = CreateSurface(Instance, Window);
    if (Env.ScreenInfos.Surface == VK_FALSE) {
        LogError("CreateEnvironment", "Failed to create VkSurfaceKHR.");
        return false;
    }
    if (CreateDeviceInfos(Env, Instance) == false) return false;
    if (CreateScreenInfos(Env, Window) == false) return false;
    VmaAllocatorCreateInfo AllocatorCreateInfo = {};
    AllocatorCreateInfo.device = Env.GPUInfos.Device;
    AllocatorCreateInfo.physicalDevice = Env.GPUInfos.PhysicalDevice;
    if (vmaCreateAllocator(&AllocatorCreateInfo, &Env.Allocator)) return false;
    if (CreateGraphicInformations(Env.GraphManager, Env, Window) == false) return false;
    LogInfo("CreateEnvironment", "Done creating Vulkan environment.");
    return true;
}

void DestroyEnvironment(Environment& Env, const VkInstance& Instance) {
    DestroyGraphicManager(Env.GraphManager, Env);

    for (uint32_t i = 0; i < Env.ScreenInfos.Views.size( ); ++i) {
        vkDestroyImageView(Env.GPUInfos.Device, Env.ScreenInfos.Views[i], 0);
    }
    vmaDestroyAllocator(Env.Allocator);
    vkDestroySwapchainKHR(Env.GPUInfos.Device, Env.ScreenInfos.Swapchain, 0);
    vkDestroySurfaceKHR(Instance, Env.ScreenInfos.Surface, 0);
    vkDestroyDevice(Env.GPUInfos.Device, 0);
}

}    // namespace Vulkan
}    // namespace ffGraph