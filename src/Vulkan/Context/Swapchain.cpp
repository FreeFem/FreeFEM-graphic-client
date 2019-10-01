#include <vector>
#include <string>
#include "Device.h"
#include "../Window/NativeWindow.h"
#include "Swapchain.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

struct SwapchainSupportDetails {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

SwapchainSupportDetails GetSwapchainSupport(const VkPhysicalDevice& PhysicalDevice, const VkSurfaceKHR& Surface)
{
    SwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &details.Capabilities);

    uint32_t FormatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, 0);

    if (FormatCount > 0) {
        details.Formats.resize(FormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &FormatCount, details.Formats.data());
    }

    uint32_t PresentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, 0);

    if (PresentModeCount > 0) {
        details.PresentModes.resize(PresentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &PresentModeCount, details.PresentModes.data());
    }
    return details;
}

VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats)
{
    for (const auto& format : AvailableFormats) {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }
    return AvailableFormats[0];
}

VkPresentModeKHR ChooseSwapchainPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentMode)
{
    for (const auto& PresentMode : AvailablePresentMode) {
        if (PresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            return PresentMode;
    }
    return AvailablePresentMode[0];
}

VkExtent2D chooseSwapchainExtent(const VkSurfaceCapabilitiesKHR& Capabilities, VkExtent2D Extent)
{
    VkExtent2D FinalExtent = {0, 0};

    FinalExtent.width = std::max(Capabilities.minImageExtent.width, std::min(Capabilities.maxImageExtent.width, Extent.width));
    FinalExtent.height = std::max(Capabilities.minImageExtent.height, std::min(Capabilities.maxImageExtent.height, Extent.height));

    return FinalExtent;
}

void CreateSwapchainImageViews(Swapchain& Swapchain, const VkDevice& Device)
{
    Swapchain.Views.resize(Swapchain.Images.size());

    VkImageViewCreateInfo CreateInfos = {};
    CreateInfos.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    CreateInfos.viewType = VK_IMAGE_VIEW_TYPE_2D;
    CreateInfos.format = Swapchain.Format.format;
    CreateInfos.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    CreateInfos.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    CreateInfos.subresourceRange.baseArrayLayer = 0;
    CreateInfos.subresourceRange.levelCount = 1;
    CreateInfos.subresourceRange.baseArrayLayer = 0;
    CreateInfos.subresourceRange.layerCount = 1;

    for (size_t i = 0; i < Swapchain.Images.size(); ++i) {
        CreateInfos.image = Swapchain.Images[i];

        if (vkCreateImageView(Device, &CreateInfos, 0, &Swapchain.Views[i])) {
            LogError(GetCurrentLogLocation(), "Failed to create swapchain VkImageView.");
            return;
        }
    }
}

Swapchain newSwapchain(const Device& Device, const VkSurfaceKHR Surface, VkExtent2D Extent)
{
    Swapchain n;
    SwapchainSupportDetails Details = GetSwapchainSupport(Device.PhysicalHandle, Surface);

    VkSurfaceFormatKHR SurfaceFormat = ChooseSwapchainSurfaceFormat(Details.Formats);
    VkPresentModeKHR PresentMode = ChooseSwapchainPresentMode(Details.PresentModes);
    Extent = chooseSwapchainExtent(Details.Capabilities, Extent);

    uint32_t ImageCount = Details.Capabilities.minImageCount + 1;
    if (Details.Capabilities.maxImageCount > 0 && ImageCount < Details.Capabilities.maxImageCount)
        ImageCount = Details.Capabilities.maxImageCount;

    VkSwapchainCreateInfoKHR SwapchainCreateInfos = {};
    SwapchainCreateInfos.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfos.surface = Surface;

    SwapchainCreateInfos.minImageCount = ImageCount;
    SwapchainCreateInfos.imageFormat = SurfaceFormat.format;
    SwapchainCreateInfos.imageColorSpace = SurfaceFormat.colorSpace;
    SwapchainCreateInfos.imageExtent = Extent;
    SwapchainCreateInfos.imageArrayLayers = 1;
    SwapchainCreateInfos.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (Device.QueueIndex[DEVICE_GRAPH_QUEUE] != Device.QueueIndex[DEVICE_GRAPH_QUEUE]) {
        SwapchainCreateInfos.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        SwapchainCreateInfos.queueFamilyIndexCount = 2;
        SwapchainCreateInfos.pQueueFamilyIndices = Device.QueueIndex;
    } else {
        SwapchainCreateInfos.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    SwapchainCreateInfos.preTransform = Details.Capabilities.currentTransform;
    SwapchainCreateInfos.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfos.presentMode = PresentMode;
    SwapchainCreateInfos.clipped = VK_TRUE;

    if (vkCreateSwapchainKHR(Device.Handle, &SwapchainCreateInfos, 0, &n.Handle))
        return n;
    n.Format = SurfaceFormat;
    ImageCount = 0;
    vkGetSwapchainImagesKHR(Device.Handle, n.Handle, &ImageCount, 0);
    n.Images.resize(ImageCount);
    vkGetSwapchainImagesKHR(Device.Handle, n.Handle, &ImageCount, n.Images.data());
    CreateSwapchainImageViews(n, Device.Handle);
    return n;
}

}
}