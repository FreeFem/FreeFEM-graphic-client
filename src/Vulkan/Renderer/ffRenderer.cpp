#include "ffRenderer.h"

namespace ffGraph {
namespace Vulkan {

static int ffFindMemoryType(const VkPhysicalDeviceMemoryProperties MemProps,
                            const uint32_t MemTypeBits,
                            const VkMemoryPropertyFlags RequiredMemProperties)
{
    uint32_t typeBits = MemTypeBits;

    uint32_t len = std::min(MemProps.memoryTypeCount, 32u);

    for (uint32_t i = 0; i < len; ++i) {
        if ((typeBits & 1) == 1) {
            if ((MemProps.memoryTypes[i].propertyFlags & RequiredMemProperties) == RequiredMemProperties)
                return (int)i;
            typeBits >>= 1;
        }
    }
    return -1;
}

static void ffNewDepthImage(const ffDevice& Device, VkExtent2D Extent, ffRenderer& Renderer)
{
    Renderer.DepthImage = VK_NULL_HANDLE;
    Renderer.DepthImageView = VK_NULL_HANDLE;
    Renderer.DepthImageMemory = VK_NULL_HANDLE;

    VkImageCreateInfo CreateInfos = {};
    CreateInfos.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    CreateInfos.imageType = VK_IMAGE_TYPE_2D;
    CreateInfos.format = VK_FORMAT_D16_UNORM;
    CreateInfos.extent = {Extent.width, Extent.height, 1};
    CreateInfos.mipLevels = 1;
    CreateInfos.arrayLayers = 1;
    CreateInfos.samples = VK_SAMPLE_COUNT_1_BIT;
    CreateInfos.tiling = VK_IMAGE_TILING_OPTIMAL;
    CreateInfos.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
    CreateInfos.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    CreateInfos.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vkCreateImage(Device.Handle, &CreateInfos, 0, &Renderer.DepthImage))
        return;

    VkMemoryRequirements MemRequired;
    vkGetImageMemoryRequirements(Device.Handle, Renderer.DepthImage, &MemRequired);

    int MemTypeIndex = ffFindMemoryType(Device.PhysicalHandleCapabilities.MemoryProperties, MemRequired.memoryTypeBits, 0);

    VkMemoryAllocateInfo MemAllocInfos = {};
    MemAllocInfos.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    MemAllocInfos.allocationSize = MemRequired.size;
    MemAllocInfos.memoryTypeIndex = (uint32_t)MemTypeIndex;

    if (vkAllocateMemory(Device.Handle, &MemAllocInfos, 0, &Renderer.DepthImageMemory))
        return;

    if (vkBindImageMemory(Device.Handle, Renderer.DepthImage, Renderer.DepthImageMemory, 0))
        return;

    VkImageViewCreateInfo DepthImageViewCreateInfos = {};
    DepthImageViewCreateInfos.sType= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    DepthImageViewCreateInfos.image = Renderer.DepthImage;
    DepthImageViewCreateInfos.format = VK_FORMAT_D16_UNORM;
    DepthImageViewCreateInfos.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
    DepthImageViewCreateInfos.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
    DepthImageViewCreateInfos.viewType = VK_IMAGE_VIEW_TYPE_2D;

    if (vkCreateImageView(Device.Handle, &DepthImageViewCreateInfos, 0, &Renderer.DepthImageView))
        return;
}

ffRenderer ffNewRenderer(ffDevice& Device, const VkSurfaceKHR& Surface, VkExtent2D Extent)
{
    ffRenderer n;
    memset(&n, 0, sizeof(ffRenderer));

    n.Queue_REF = &Device.Queue;
    n.Swapchain = ffNewSwapchain(Device.PhysicalHandle, Device.Handle, Surface, Extent);
    if (ffIsSwapchainReady(n.Swapchain) == false)
        return n;
    VkCommandPoolCreateInfo CmdPoolCreateInfos = {};
    CmdPoolCreateInfos.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CmdPoolCreateInfos.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CmdPoolCreateInfos.queueFamilyIndex = Device.QueueIndex;

    if (vkCreateCommandPool(Device.Handle, &CmdPoolCreateInfos, 0, &n.CommandPool))
        return n;
    ffNewDepthImage(Device, );
    return n;
}

} // namespace Vulkan
}