#include <cstring>
#include "ffImage.h"

namespace ffGraph {
namespace Vulkan {

ffImage ffCreateImage(const VmaAllocator& Allocator, const VkDevice& Device, ffImageCreateInfo pCreateInfo,
                      VmaAllocationCreateInfo pAllocationInfos) {
    ffImage n = {VK_NULL_HANDLE, VK_NULL_HANDLE, 0, 0};

    VkImageCreateInfo vkCreateInfo = {};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    vkCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    vkCreateInfo.format = pCreateInfo.Format;
    vkCreateInfo.extent = {pCreateInfo.Extent.width, pCreateInfo.Extent.height, 1};
    vkCreateInfo.mipLevels = 1;
    vkCreateInfo.arrayLayers = 1;
    vkCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    vkCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    vkCreateInfo.usage = pCreateInfo.Usage;
    vkCreateInfo.sharingMode = pCreateInfo.SharingMode;
    vkCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (vmaCreateImage(Allocator, &vkCreateInfo, &pAllocationInfos, &n.Handle, &n.Memory, &n.AllocationInfos)) return n;

    if (pCreateInfo.AsView) {
        VkImageViewCreateInfo ImageViewCreateInfo = {};
        ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        ImageViewCreateInfo.image = n.Handle;
        ImageViewCreateInfo.format = pCreateInfo.Format;
        ImageViewCreateInfo.subresourceRange = {pCreateInfo.ViewInfos.AspectMask, 0, 1, 0, 1};
        ImageViewCreateInfo.components = {VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY,
                                          VK_COMPONENT_SWIZZLE_IDENTITY, VK_COMPONENT_SWIZZLE_IDENTITY};
        ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

        vkCreateImageView(Device, &ImageViewCreateInfo, 0, &n.View);
    }
    return n;
}

void ffDestroyImage(const VmaAllocator& Allocator, const VkDevice& Device, ffImage Image) {
    vkDestroyImageView(Device, Image.View, 0);
    vmaDestroyImage(Allocator, Image.Handle, Image.Memory);
    memset(&Image, 0, sizeof(ffImage));
}

}    // namespace Vulkan
}    // namespace ffGraph
