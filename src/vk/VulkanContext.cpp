#include <cstdlib>
#include "../core/Window.h"
#include "../util/Logger.h"
#include "../util/utils.h"
#include "VulkanContext.h"

namespace FEM {
namespace VK {

#ifdef _DEBUG

VKAPI_ATTR VkBool32 VKAPI_CALL
debugMessengerCallback(UNUSED_PARAM VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       UNUSED_PARAM VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData, UNUSED_PARAM void *pUserData) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::string message(pCallbackData->pMessageIdName);
        message.append(" > ");
        message.append(pCallbackData->pMessage);
        LOGW("Validation Layer Messenger", message);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::string message(pCallbackData->pMessageIdName);
        message.append(" > ");
        message.append(pCallbackData->pMessage);
        LOGW("Validation Layer Messenger", message);
    } else {
        std::string message(pCallbackData->pMessageIdName);
        message.append(" > ");
        message.append(pCallbackData->pMessage);
        LOGI("Validation Layer Messenger", message);
    }
    return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackEXT(UNUSED_PARAM VkDebugReportFlagsEXT flags,
                                                      UNUSED_PARAM VkDebugReportObjectTypeEXT objectType,
                                                      UNUSED_PARAM uint64_t object, UNUSED_PARAM size_t location,
                                                      UNUSED_PARAM int32_t messageCode,
                                                      UNUSED_PARAM const char *pLayerPrefix, const char *pMessage,
                                                      UNUSED_PARAM void *pUserData) {
    dprintf(2, "Validation Layers CallBack : %s.\n", pMessage);
    return VK_FALSE;
}

void DebugMakerBegin(VkCommandBuffer cmdBuffer, const char *name, PFN_vkCmdDebugMarkerBeginEXT CmdDebugMarkerBeginEXT_PFN)
{
    if (CmdDebugMarkerBeginEXT_PFN) {
        VkDebugMarkerMarkerInfoEXT markerInfo = {};
        markerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_MARKER_MARKER_INFO_EXT;
        markerInfo.color[0] = 1.0f;
        markerInfo.pMarkerName = (name == 0 || strlen(name) == 0) ? name : "Unnamed";
        CmdDebugMarkerBeginEXT_PFN(cmdBuffer, &markerInfo);
    }
}

void DebugMakerEnd(VkCommandBuffer cmdBuffer, PFN_vkCmdDebugMarkerEndEXT CmdDebugMarkerEndEXT_PFN)
{
    if (CmdDebugMarkerEndEXT_PFN)
        CmdDebugMarkerEndEXT_PFN(cmdBuffer);
}

#endif

static bool newVkInstance(VulkanContext *vkContext, const Window *Win) {
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "FreeFEM++";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "FreeFEM++ Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    uint32_t ExtensionsCount = Win->VulkanExtensionsCount;
#ifdef _DEBUG
    VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo = {};

    createInfo.enabledLayerCount = LayerCount;
    createInfo.ppEnabledLayerNames = Layers;
    MessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    MessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    MessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    MessengerCreateInfo.pfnUserCallback = debugMessengerCallback;
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&MessengerCreateInfo;
    ExtensionsCount += 2;
    vkContext->InstanceExtensions = (char **)malloc(sizeof(char *) * ExtensionsCount);
    memcpy(vkContext->InstanceExtensions, Win->VulkanExtensions, Win->VulkanExtensionsCount * sizeof(char *));
    vkContext->InstanceExtensions[Win->VulkanExtensionsCount] = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
    vkContext->InstanceExtensions[Win->VulkanExtensionsCount + 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
#else
    vkContext->InstanceExtensions = (char **)malloc(sizeof(char *) * ExtensionsCount);
    memcpy(vkContext->InstanceExtensions, Win->VulkanExtensions, Win->VulkanExtensionsCount * sizeof(char *));
#endif

    vkContext->InstanceExtensionsCount = ExtensionsCount;
    createInfo.enabledExtensionCount = vkContext->InstanceExtensionsCount;
    createInfo.ppEnabledExtensionNames = vkContext->InstanceExtensions;

    if (vkCreateInstance(&createInfo, 0, &vkContext->Instance)) return false;
#ifdef _DEBUG
    VkDebugReportCallbackCreateInfoEXT createInfoDebug = {};
    createInfoDebug.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
    createInfoDebug.pfnCallback = debugReportCallbackEXT;
    createInfoDebug.flags =
        VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
    createInfoDebug.pUserData = vkContext;

    PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
        reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
            vkGetInstanceProcAddr(vkContext->Instance, "vkCreateDebugReportCallbackEXT"));
    if (!vkCreateDebugReportCallbackEXT) return false;
    vkCreateDebugReportCallbackEXT(vkContext->Instance, &createInfoDebug, 0, &vkContext->DebugReportCallback);

    vkContext->CreateDebugUtilsMessengerEXT_PFN = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(vkContext->Instance, "vkCreateDebugUtilsMessengerEXT"));
    if (vkContext->CreateDebugUtilsMessengerEXT_PFN) {
        vkContext->CreateDebugUtilsMessengerEXT_PFN(vkContext->Instance, &MessengerCreateInfo, 0,
                                                    &vkContext->DebugMessenger);
    }
#endif
    return true;
}

static uint32_t getGraphicsFamilyIndex(VkPhysicalDevice PhysicalDevice, VkSurfaceKHR Surface) {
    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, 0);

    VkQueueFamilyProperties *QueueFamilyPropertiesArray =
        (VkQueueFamilyProperties *)malloc(sizeof(VkQueueFamilyProperties) * queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, QueueFamilyPropertiesArray);

    VkBool32 SurfaceSupported = false;
    for (uint32_t i = 0; i < queueCount; i += 1) {
        vkGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &SurfaceSupported);
        if (SurfaceSupported && QueueFamilyPropertiesArray[i].queueFlags & VK_QUEUE_GRAPHICS_BIT &&
            QueueFamilyPropertiesArray[i].queueFlags & VK_QUEUE_TRANSFER_BIT)
            return i;
        SurfaceSupported = false;
    }
    return VK_QUEUE_FAMILY_IGNORED;
}

static bool newVkSurfaceKHR(VulkanContext *vkContext, const Window *Win) {
    if (glfwCreateWindowSurface(vkContext->Instance, Win->Handle, 0, &vkContext->Surface)) return false;
    return true;
}

static bool newVkPhysicalDevice(VulkanContext *vkContext) {
    uint32_t PhysicalDeviceCount = 0;
    if (vkEnumeratePhysicalDevices(vkContext->Instance, &PhysicalDeviceCount, 0)) return false;
    if (PhysicalDeviceCount < 1) return false;

    VkPhysicalDevice *PhysicalDeviceArray = (VkPhysicalDevice *)malloc(sizeof(VkPhysicalDevice) * PhysicalDeviceCount);
    if (vkEnumeratePhysicalDevices(vkContext->Instance, &PhysicalDeviceCount, PhysicalDeviceArray)) return false;

    uint32_t preferred = 0;
    uint32_t fallback = 0;
    for (uint32_t i = 0; i < PhysicalDeviceCount; i += 1) {
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(PhysicalDeviceArray[i], &props);

        if (getGraphicsFamilyIndex(PhysicalDeviceArray[i], vkContext->Surface) == VK_QUEUE_FAMILY_IGNORED) continue;
        if (!preferred && props.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) preferred = i;
        if (!fallback) fallback = i;
    }
    uint32_t result = preferred ? preferred : fallback;

    vkContext->PhysicalDevice = PhysicalDeviceArray[result];
    vkGetPhysicalDeviceProperties(vkContext->PhysicalDevice, &vkContext->PhysicalDeviceInfos.Properties);
    vkGetPhysicalDeviceMemoryProperties(vkContext->PhysicalDevice, &vkContext->PhysicalDeviceInfos.MemoryProperties);
    vkGetPhysicalDeviceFeatures(vkContext->PhysicalDevice, &vkContext->PhysicalDeviceInfos.Features);
    vkContext->PhysicalDeviceInfos.Limits = vkContext->PhysicalDeviceInfos.Properties.limits;
    free(PhysicalDeviceArray);

    VkFormat format;
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext->PhysicalDevice, vkContext->Surface, &formatCount, 0);
    VkSurfaceFormatKHR *SurfaceFormatArray = (VkSurfaceFormatKHR *)malloc(sizeof(VkSurfaceFormatKHR) * formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext->PhysicalDevice, vkContext->Surface, &formatCount,
                                         SurfaceFormatArray);

    if (formatCount == 1 && SurfaceFormatArray[0].format == VK_FORMAT_UNDEFINED) {
        format = VK_FORMAT_R8G8B8A8_UNORM;
    }

    for (uint32_t i = 0; i < formatCount; i += 1)
        if (SurfaceFormatArray[i].format == VK_FORMAT_R8G8B8A8_UNORM ||
            SurfaceFormatArray[i].format == VK_FORMAT_B8G8R8A8_UNORM)
            format = SurfaceFormatArray[i].format;

    VkSurfaceFormatKHR finalSurfaceformat;
    finalSurfaceformat.format = format;
    finalSurfaceformat.colorSpace = SurfaceFormatArray[0].colorSpace;
    vkContext->SurfaceFormat = finalSurfaceformat;
    free(SurfaceFormatArray);
    return true;
}

static bool newVkDevice(VulkanContext *vkContext) {
    uint32_t DesiredQueue = getGraphicsFamilyIndex(vkContext->PhysicalDevice, vkContext->Surface);

    vkContext->QueueInfos.Index = DesiredQueue;
    vkContext->QueueInfos.TypeFlags = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT;

    float QueuePriority = 1.0f;
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = DesiredQueue;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &QueuePriority;

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.pEnabledFeatures = &vkContext->PhysicalDeviceInfos.Features;

    const char *extensions[] = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#ifdef _DEBUG
        VK_EXT_DEBUG_MARKER_EXTENSION_NAME,
#endif
    };

    deviceCreateInfo.enabledExtensionCount = (uint32_t)(sizeof(extensions) / sizeof(extensions[0]));
    deviceCreateInfo.ppEnabledExtensionNames = extensions;

#ifdef _DEBUG
    deviceCreateInfo.enabledLayerCount = LayerCount;
    deviceCreateInfo.ppEnabledLayerNames = Layers;
#endif
    if (vkCreateDevice(vkContext->PhysicalDevice, &deviceCreateInfo, 0, &vkContext->Device)) return false;
    vkGetDeviceQueue(vkContext->Device, vkContext->QueueInfos.Index, 0, &vkContext->Queue);
    return true;
}

static bool newVmaAllocator(VulkanContext *vkContext) {
    VmaAllocatorCreateInfo createInfo = {};
    createInfo.device = vkContext->Device;
    createInfo.physicalDevice = vkContext->PhysicalDevice;

    if (vmaCreateAllocator(&createInfo, &vkContext->Allocator)) return false;
    return true;
}

static bool newVkSwapchainKHR(VulkanContext *vkContext, const Window *Win) {
    VkColorSpaceKHR SurfaceColorSpace = vkContext->SurfaceFormat.colorSpace;

    VkSurfaceCapabilitiesKHR SurfaceCapabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkContext->PhysicalDevice, vkContext->Surface, &SurfaceCapabilities))
        return false;
    uint32_t presentModeCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(vkContext->PhysicalDevice, vkContext->Surface, &presentModeCount, 0))
        return false;
    if (presentModeCount < 1) return false;
    VkPresentModeKHR *PresentModeArray = (VkPresentModeKHR *)malloc(sizeof(VkPresentModeKHR) * presentModeCount);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(vkContext->PhysicalDevice, vkContext->Surface, &presentModeCount,
                                                  PresentModeArray))
        return false;
    VkExtent2D swapchainExtent = SurfaceCapabilities.currentExtent;

    if (swapchainExtent.width == UINT32_MAX) {
        swapchainExtent.width = Win->ScreenWidth;
        swapchainExtent.height = Win->ScreenHeight;
    } else if (swapchainExtent.width != Win->ScreenWidth || swapchainExtent.height != Win->ScreenHeight)
        return false;

    uint32_t desiredNbOfSwapchainImages = SurfaceCapabilities.minImageCount + 1;

    if (SurfaceCapabilities.maxImageCount > 0)
        desiredNbOfSwapchainImages = std::min(desiredNbOfSwapchainImages, SurfaceCapabilities.maxImageCount);

    VkSurfaceTransformFlagBitsKHR SurfacetransformFlagBits;
    if (SurfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        SurfacetransformFlagBits = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else
        SurfacetransformFlagBits = SurfaceCapabilities.currentTransform;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = vkContext->Surface;
    createInfo.minImageCount = desiredNbOfSwapchainImages;
    createInfo.imageFormat = vkContext->SurfaceFormat.format;
    createInfo.imageColorSpace = SurfaceColorSpace;
    createInfo.imageExtent = swapchainExtent;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = SurfacetransformFlagBits;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(vkContext->Device, &createInfo, 0, &vkContext->Swapchain)) return false;

    if (vkGetSwapchainImagesKHR(vkContext->Device, vkContext->Swapchain, &vkContext->SwapchainImageCount, 0))
        return false;
    if (vkContext->SwapchainImageCount < 1) return false;
    vkContext->SwapchainImages = (VkImage *)malloc(sizeof(VkImage) * vkContext->SwapchainImageCount);
    vkContext->SwapchainImageViews = (VkImageView *)malloc(sizeof(VkImageView) * vkContext->SwapchainImageCount);
    if (vkGetSwapchainImagesKHR(vkContext->Device, vkContext->Swapchain, &vkContext->SwapchainImageCount,
                                vkContext->SwapchainImages))
        return false;

    for (uint32_t i = 0; i < vkContext->SwapchainImageCount; i += 1) {
        VkImageViewCreateInfo ImageViewCreateInfo = {};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.format = vkContext->SurfaceFormat.format;
        ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        ImageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        ImageViewCreateInfo.subresourceRange.levelCount = 1;
        ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        ImageViewCreateInfo.subresourceRange.layerCount = 1;
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        ImageViewCreateInfo.image = vkContext->SwapchainImages[i];

        if (vkCreateImageView(vkContext->Device, &ImageViewCreateInfo, 0, &vkContext->SwapchainImageViews[i]))
            return false;
    }
    return true;
}

static bool newVkCommandBuffer(const VkDevice Device, const VkCommandPool CommandPool, uint32_t Count,
                               const VkCommandBufferLevel CmdBufferLevels, VkCommandBuffer *CommandBuffers) {
    VkResult res;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = CommandPool;
    allocInfo.level = CmdBufferLevels;
    allocInfo.commandBufferCount = Count;

    if (vkAllocateCommandBuffers(Device, &allocInfo, CommandBuffers)) return false;
    return true;
}

static bool newVkFence(const VkDevice Device, VkFence *Fence) {
    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    return vkCreateFence(Device, &createInfo, 0, Fence);
}

static bool newVkSemaphore(const VkDevice Device, VkSemaphore *Semaphore) {
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    return vkCreateSemaphore(Device, &createInfo, 0, Semaphore);
}

static bool pushInitCmdBuffer(const VkDevice Device, const VkQueue Queue, const Image DepthImage,
                              VkCommandBuffer cmdBuffer) {
    VkImageMemoryBarrier memBarrier = {};

    memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
    memBarrier.image = DepthImage.Handle;

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(cmdBuffer, &BeginInfo)) return false;
    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, 0, 0, 0,
                         1, &memBarrier);

    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    vkQueueSubmit(Queue, 1, &submitInfo, VK_NULL_HANDLE);

    vkQueueWaitIdle(Queue);
    return true;
}

bool newVulkanContext(VulkanContext *vkContext, const Window *Win) {
    memset(vkContext, 0, sizeof(VulkanContext));
    if (!newVkInstance(vkContext, Win)) {
        LOGE(FILE_LOCATION( ), "Failed to create VkInstance.");
        return false;
    }
    if (!newVkSurfaceKHR(vkContext, Win)) {
        LOGE(FILE_LOCATION( ), "Failed to create VkSurfaceKHR.");
        return false;
    }
    if (!newVkPhysicalDevice(vkContext)) {
        LOGE(FILE_LOCATION( ), "Failed to create VkPhysicalDevice.");
        return false;
    }
    if (!newVkDevice(vkContext)) {
        LOGE(FILE_LOCATION( ), "Failed to create VkDevice.");
        return false;
    }
    if (!newVmaAllocator(vkContext)) {
        LOGE(FILE_LOCATION( ), "Failed to create VmaAllocator.");
        return false;
    }

    ImageInfos DepthImageInfos = {};
    DepthImageInfos.Width = Win->ScreenWidth;
    DepthImageInfos.Height = Win->ScreenHeight;
    DepthImageInfos.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    DepthImageInfos.AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    DepthImageInfos.AllocInfos.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    DepthImageInfos.Format = VK_FORMAT_D16_UNORM;
    if (!newImage({&vkContext->Device, &vkContext->Allocator}, &vkContext->DepthImage, true, DepthImageInfos)) {
        LOGE("newVulkanContext", "Failed to initialize DepthImage.");
        return false;
    }

    ImageInfos ColorImageInfos = {};
    ColorImageInfos.Width = Win->ScreenWidth;
    ColorImageInfos.Height = Win->ScreenHeight;
    ColorImageInfos.Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    ColorImageInfos.AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    ColorImageInfos.AllocInfos.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    ColorImageInfos.Format = VK_FORMAT_B8G8R8A8_UNORM;
    if (!newImage({&vkContext->Device, &vkContext->Allocator}, &vkContext->ColorImage, true, ColorImageInfos)) {
        LOGE("newVulkanContext", "Failed to initialize ColorImage.");
        return false;
    }

    VkCommandPoolCreateInfo CmdPoolCreateInfos = {};
    CmdPoolCreateInfos.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CmdPoolCreateInfos.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CmdPoolCreateInfos.queueFamilyIndex = vkContext->QueueInfos.Index;

    if (vkCreateCommandPool(vkContext->Device, &CmdPoolCreateInfos, 0, &vkContext->CommandPool)) {
        LOGE("newVulkanContext", "Failed to initialize CommandPool.");
        return false;
    }
    if (!newVkCommandBuffer(vkContext->Device, vkContext->CommandPool, SCREENBUFFER_NB + 1,
                            VK_COMMAND_BUFFER_LEVEL_PRIMARY, vkContext->CommandBuffers)) {
        LOGE("newVulkanContext", "Failed to initialize CommandBuffers");
        return false;
    }
    for (uint8_t i = 0; i < SCREENBUFFER_NB; i += 1) {
        newVkFence(vkContext->Device, &vkContext->FrameInfos[i].Fence);
        newVkSemaphore(vkContext->Device, &vkContext->FrameInfos[i].Semaphores[0]);
        newVkSemaphore(vkContext->Device, &vkContext->FrameInfos[i].Semaphores[1]);
        vkContext->FrameInfos[i].initialize = false;
    }
    if (!newVkSwapchainKHR(vkContext, Win)) {
        LOGE(FILE_LOCATION( ), "Failed to create VkSwapchainKHR.");
        return false;
    }
    if (!pushInitCmdBuffer(vkContext->Device, vkContext->Queue, vkContext->DepthImage,
                           vkContext->CommandBuffers[SCREENBUFFER_NB])) {
        LOGE("newVulkanContext", "Failed to push initialization infos on Queue.");
        return false;
    }
    return true;
}

void destroyVulkanContext(VulkanContext *vkContext) {
    vkFreeCommandBuffers(vkContext->Device, vkContext->CommandPool, SCREENBUFFER_NB + 1, vkContext->CommandBuffers);
    for (uint32_t i = 0; i < vkContext->SwapchainImageCount; i += 1) {
        vkDestroyImageView(vkContext->Device, vkContext->SwapchainImageViews[i], 0);
    }
    free(vkContext->SwapchainImageViews);
    vkDestroySwapchainKHR(vkContext->Device, vkContext->Swapchain, 0);

    for (uint8_t i = 0; i < SCREENBUFFER_NB; i += 1) {
        vkDestroyFence(vkContext->Device, vkContext->FrameInfos[i].Fence, 0);
        vkDestroySemaphore(vkContext->Device, vkContext->FrameInfos[i].Semaphores[0], 0);
        vkDestroySemaphore(vkContext->Device, vkContext->FrameInfos[i].Semaphores[1], 0);
    }
    vkDestroyCommandPool(vkContext->Device, vkContext->CommandPool, 0);

    destroyImage({&vkContext->Device, &vkContext->Allocator}, vkContext->DepthImage);
    destroyImage({&vkContext->Device, &vkContext->Allocator}, vkContext->ColorImage);

    vmaDestroyAllocator(vkContext->Allocator);
    vkDestroyDevice(vkContext->Device, 0);
    free(vkContext->SwapchainImages);

#ifdef _DEBUG
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(vkContext->Instance,
                                                                           "vkDestroyDebugUtilsMessengerEXT");
    if (func) func(vkContext->Instance, vkContext->DebugMessenger, 0);
    auto func1 = (PFN_vkDestroyDebugReportCallbackEXT)vkGetInstanceProcAddr(vkContext->Instance,
                                                                            "vkDestroyDebugReportCallbackEXT");
    if (func1) func1(vkContext->Instance, vkContext->DebugReportCallback, 0);
#endif
    vkDestroySurfaceKHR(vkContext->Instance, vkContext->Surface, 0);
    vkDestroyInstance(vkContext->Instance, 0);

    free(vkContext->InstanceExtensions);
}

}    // namespace VK
}    // namespace FEM
