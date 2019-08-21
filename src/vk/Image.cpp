#include <algorithm>
#include "Image.h"

static int findMemoryTypeWithProperties(const VkPhysicalDeviceMemoryProperties memoryProperties,
                                 const uint32_t memoryTypeBits,
                                 const VkMemoryPropertyFlags requiredMemoryProperties)
{
    uint32_t typeBits = memoryTypeBits;

    uint32_t len = std::min(memoryProperties.memoryTypeCount, 32u);
    for (uint32_t i = 0; i < len; i += 1) {
        if ((typeBits & 1) == 1) {
            if ((memoryProperties.memoryTypes[i].propertyFlags & requiredMemoryProperties) == requiredMemoryProperties)
                return (int)i;
        }
        typeBits >>= 1;
    }
    return -1;
}

Error gr::Image::init(const VkDevice device, const VkPhysicalDeviceMemoryProperties memProps, const VkBufferUsageFlags imageUsage,
                    const VkMemoryPropertyFlags requiredMemProps, const VkFormat imageFormat, const int width, const int height,
                    VkImageAspectFlags viewSubresoucesAspectMask)
{
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

    if (vkCreateImage(device, &imageCreateInfo, 0, &m_handle) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, m_handle, &memoryRequirements);

    int memoryTypeIndex = findMemoryTypeWithProperties(memProps, memoryRequirements.memoryTypeBits, requiredMemProps);
    if (memoryTypeIndex < 0)
        return Error::FUNCTION_FAILED;
    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext = 0;
    memAllocInfo.allocationSize = memoryRequirements.size;
    memAllocInfo.memoryTypeIndex = (uint32_t)memoryTypeIndex;

    if (vkAllocateMemory(device, &memAllocInfo, 0, &m_memory) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;

    if (vkBindImageMemory(device, m_handle, m_memory, 0) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;

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

    if (vkCreateImageView(device, &imageViewCreateInfo, 0, &m_view) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

void gr::Image::destroy(const VkDevice& device)
{
    vkFreeMemory(device, m_memory, 0);
    vkDestroyImageView(device, m_view, 0);
    vkDestroyImage(device, m_handle, 0);
}