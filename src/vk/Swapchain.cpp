#include <vector>
#include "Swapchain.h"

Error SwapchainCreator::init(VkPhysicalDevice physicalDevice, VkDevice device, GLFWwindow *window, VkSurfaceKHR surface)
{
    m_device = device;
    m_physicalDevice = physicalDevice;
    m_window = window;
    m_surface = surface;
    if (!m_device || !m_physicalDevice || !m_window || !m_surface)
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

Error SwapchainCreator::GetSurfaceFormat(VkFormat& surfaceFormat)
{
    uint32_t surfaceFormatCount;

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &surfaceFormatCount, 0) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    if (surfaceFormatCount < 1)
        return Error::FUNCTION_FAILED;

    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &surfaceFormatCount, surfaceFormats.data()) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;

    if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
        surfaceFormat = VK_FORMAT_B8G8R8_UNORM;
    else
        surfaceFormat = surfaceFormats[0].format;
    return Error::NONE;
}

Error SwapchainCreator::newSwapchain(VkSwapchainKHR oldSwapchain, Swapchain& outSwapchain)
{
    uint32_t surfaceFormatCount;

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &surfaceFormatCount, 0) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    if (surfaceFormatCount < 1)
        return Error::FUNCTION_FAILED;

    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &surfaceFormatCount, surfaceFormats.data()) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;

    VkFormat surfaceFormat;
    if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
        surfaceFormat = VK_FORMAT_B8G8R8_UNORM;
    else
        surfaceFormat = surfaceFormats[0].format;

    VkColorSpaceKHR surfaceColorSpace = surfaceFormats[0].colorSpace;

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &surfaceCapabilities) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;

    uint32_t presentModeCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, 0) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    if (presentModeCount < 1)
        return Error::FUNCTION_FAILED;

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, presentModes.data()) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;

    VkExtent2D swapchainExtent = surfaceCapabilities.currentExtent;

    int windowWidth, windowHeight = 0;
    glfwGetWindowSize(m_window, &windowWidth, &windowHeight);
    if (swapchainExtent.width == (uint32_t)(-1)) {
        swapchainExtent.width = static_cast<uint32_t>(windowWidth);
        swapchainExtent.height = static_cast<uint32_t>(windowHeight);
    } else if (swapchainExtent.width != static_cast<uint32_t>(windowWidth) && swapchainExtent.height != static_cast<uint32_t>(windowHeight)) {
        return Error::FUNCTION_FAILED;
    }

    uint32_t desiredNbOfSwapchainImages = surfaceCapabilities.minImageCount + 1;

    if (surfaceCapabilities.maxImageCount > 0)
        desiredNbOfSwapchainImages = std::min(desiredNbOfSwapchainImages, surfaceCapabilities.maxImageCount);

    VkSurfaceTransformFlagBitsKHR surfaceTransformFlagBits;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        surfaceTransformFlagBits = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else
        surfaceTransformFlagBits = surfaceCapabilities.currentTransform;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;
    createInfo.minImageCount = desiredNbOfSwapchainImages;
    createInfo.imageFormat = surfaceFormat;
    createInfo.imageColorSpace = surfaceColorSpace;
    createInfo.imageExtent = swapchainExtent;
    createInfo.imageUsage = usageFlags;
    createInfo.preTransform = surfaceTransformFlagBits;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.oldSwapchain = oldSwapchain;
    createInfo.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(m_device, &createInfo, 0, &outSwapchain.m_handle) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    if (oldSwapchain != VK_NULL_HANDLE)
        vkDestroySwapchainKHR(m_device, oldSwapchain, 0);
    uint32_t imageCount = 0;
    if (vkGetSwapchainImagesKHR(m_device, outSwapchain.m_handle, &imageCount, 0) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    if (imageCount < 1)
        return Error::FUNCTION_FAILED;
    std::vector<VkImage> swapchainImages(imageCount);
    if (vkGetSwapchainImagesKHR(m_device, outSwapchain.m_handle, &imageCount, swapchainImages.data()) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;

    std::vector<VkImageView> swapchainImageViews(imageCount);

    for (uint32_t i = 0; i < imageCount; i += 1) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = 0;
        createInfo.flags = 0;
        GetSurfaceFormat(createInfo.format);
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

        if (vkCreateImageView(m_device, &createInfo, 0, &swapchainImageViews[i]) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;
    }
    outSwapchain.m_images = std::move(swapchainImages);
    outSwapchain.m_imagesView = std::move(swapchainImageViews);
    return Error::NONE;
}