#include "vkcommon.h"
#include "GraphManager.h"

Error gr::Image::init(const Manager& grm, const VkBufferUsageFlags imageUsage,
                    const VkMemoryPropertyFlags requiredMemProps, const VkFormat imageFormat,
                    VkImageAspectFlags viewSubresoucesAspectMask)
{
    int width = grm.getNativeWindow().getWidth();
    int height = grm.getNativeWindow().getHeight();

    VkImageCreateInfo imageCreateInfo = {};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCreateInfo.pNext = 0;
    imageCreateInfo.flags = 0;
    imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imageCreateInfo.format = imageFormat;
    imageCreateInfo.extent = {(uint32_t)width, (uint32_t)height, 1};
    imageCreateInfo.mipLevels = 1;
    imageCreateInfo.arrayLayers = 1;
    imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageCreateInfo.usage = imageUsage;
    imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageCreateInfo.queueFamilyIndexCount = 0;
    imageCreateInfo.pQueueFamilyIndices = 0;
    imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VmaAllocationCreateInfo allocInfo = {};
    allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if (vmaCreateImage(grm.getAllocator(), &imageCreateInfo, &allocInfo, &m_handle, &m_memory, 0))
        return Error::FUNCTION_FAILED;

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(grm.getDevice(), m_handle, &memoryRequirements);

    VkImageViewCreateInfo imageViewCreateInfo = {};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.pNext = 0;
    imageViewCreateInfo.flags = 0;
    imageViewCreateInfo.image = m_handle;
    imageViewCreateInfo.format = imageFormat;
    imageViewCreateInfo.subresourceRange.aspectMask = viewSubresoucesAspectMask;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;
    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

    if (vkCreateImageView(grm.getDevice(), &imageViewCreateInfo, 0, &m_view) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

void gr::Image::destroy(const Manager& grm)
{
    vkDestroyImageView(grm.getDevice(), m_view, 0);
    vmaDestroyImage(grm.getAllocator(), m_handle, m_memory);
}