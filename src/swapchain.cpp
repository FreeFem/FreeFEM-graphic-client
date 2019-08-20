#include <GLFW/glfw3.h>
#include "instance.h"
#include "utils.h"

bool createSwapchain(const VkPhysicalDevice phyDev,
                     const VkDevice device,
                     const VkSurfaceKHR surface,
                     GLFWwindow *window,
                     VkSwapchainKHR oldSwapchain,
                     VkSwapchainKHR& outSwapchain,
                     const int ownedSwapchainImages,
                     VkFormat& outSurfaceFormat,
                     const VkImageUsageFlags imageUsageFlags
                    )
{
    VkResult result;

    uint32_t surfaceFormatCount;
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(phyDev, surface, &surfaceFormatCount, 0);
    if (result != VK_SUCCESS) {
        return false;
    } else if (surfaceFormatCount == 0) {
        return false;
    }

    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    result = vkGetPhysicalDeviceSurfaceFormatsKHR(phyDev, surface, &surfaceFormatCount, surfaceFormats.data());
    if (result != VK_SUCCESS) {
        return false;
    }
    VkFormat surfaceFormat;

    if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
        surfaceFormat = VK_FORMAT_B8G8R8A8_UNORM;
    else
        surfaceFormat = surfaceFormats[0].format;
    VkColorSpaceKHR surfaceColorSpace = surfaceFormats[0].colorSpace;

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyDev, surface, &surfaceCapabilities);
    if (result != VK_SUCCESS) {
        return false;
    }

    uint32_t presentModeCount = 0;
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(phyDev, surface, &presentModeCount, 0);
    if (result != VK_SUCCESS) {
        return false;
    } else if (presentModeCount == 0) {
        return false;
    }

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    result = vkGetPhysicalDeviceSurfacePresentModesKHR(phyDev, surface, &presentModeCount, presentModes.data());
    if (result != VK_SUCCESS) {
        return false;
    }

    VkExtent2D swapChainExtent = surfaceCapabilities.currentExtent;

    int windowWidth = 0;
    int windowHeight = 0;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    if (swapChainExtent.width == (uint32_t)(-1)) {
        swapChainExtent.width = static_cast<uint32_t>(windowWidth);
        swapChainExtent.height = static_cast<uint32_t>(windowHeight);
    } else {
        if (swapChainExtent.width != static_cast<uint32_t>(windowWidth) && swapChainExtent.height != static_cast<uint32_t>(windowHeight))
            return false;
    }
    uint32_t desiredNumberOfSpawchainImages = surfaceCapabilities.minImageCount + (uint32_t)ownedSwapchainImages;

    if (surfaceCapabilities.maxImageCount > 0) {
        desiredNumberOfSpawchainImages = std::min(desiredNumberOfSpawchainImages, surfaceCapabilities.maxImageCount);
    }

    VkSurfaceTransformFlagBitsKHR surfaceTransformFlagBits;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        surfaceTransformFlagBits = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else
        surfaceTransformFlagBits = surfaceCapabilities.currentTransform;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.pNext = 0;
    createInfo.flags = 0;
    createInfo.surface = surface;
    createInfo.minImageCount = desiredNumberOfSpawchainImages;
    createInfo.imageFormat = surfaceFormat;
    createInfo.imageColorSpace = surfaceColorSpace;
    createInfo.imageExtent = swapChainExtent;
    createInfo.imageUsage = imageUsageFlags;
    createInfo.preTransform = surfaceTransformFlagBits;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = 0;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.oldSwapchain = oldSwapchain;
    createInfo.clipped = VK_TRUE;

    result = vkCreateSwapchainKHR(device, &createInfo, 0, &outSwapchain);
    if (result != VK_SUCCESS) {
        return false;
    }
    outSurfaceFormat = surfaceFormat;

    if (oldSwapchain != VK_NULL_HANDLE) {
        vkDestroySwapchainKHR(device, oldSwapchain, 0);
    }
    return true;
}

bool getSwapchainImagesAndViews(const VkDevice device,
                                const VkSwapchainKHR swapchain,
                                const VkFormat *surfaceFormat,
                                std::vector<VkImage>& outSwapchainImages,
                                std::vector<VkImageView>& outSwapchainImageViews
                                )
{
    VkResult result;

    uint32_t swapchainImageCount = 0;
    result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, 0);
    if (result != VK_SUCCESS) {
        return false;
    } else if (swapchainImageCount == 0) {
        return false;
    }
    std::vector<VkImage> swapchainImages(swapchainImageCount);
    result = vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data());
    if (result != VK_SUCCESS) {
        return false;
    }

    std::vector<VkImageView> swapchainImageViews(swapchainImageCount);

    for (uint32_t i = 0; i < swapchainImageCount; i += 1) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = 0;
        createInfo.flags = 0;
        createInfo.format = *surfaceFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.image = swapchainImages[i];

        result = vkCreateImageView(device, &createInfo, 0, &swapchainImageViews[i]);
        if (result != VK_SUCCESS) {
            return false;
        }
    }
    outSwapchainImages = std::move(swapchainImages);
    outSwapchainImageViews = std::move(swapchainImageViews);
    return true;
}