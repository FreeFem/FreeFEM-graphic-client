#include <cstdio>
#include "utils.h"

bool createAndAllocateImage(const VkDevice device,
                            const VkPhysicalDeviceMemoryProperties memProps,
                            const VkBufferUsageFlags imageUsage,
                            const VkMemoryPropertyFlags requiredMemoryProps,
                            const VkFormat imageFormat,
                            const int width,
                            const int height,
                            VkImage& outImage,
                            VkDeviceMemory& outImageMem,
                            VkImageView *outImageViewPtr,
                            VkImageAspectFlags viewSubresoucesAspectMask)
{
    VkResult result;
    VkImage image;
    VkImageView imageView;
    VkDeviceMemory imageMemory;

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

    result = vkCreateImage(device, &imageCreateInfo, 0, &image);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to create image. [%s]\n", VkResultToStr(result));
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetImageMemoryRequirements(device, image, &memoryRequirements);

    int memoryTypeIndex = findMemoryTypeWithProperties(memProps, memoryRequirements.memoryTypeBits, requiredMemoryProps);
    if (memoryTypeIndex < 0) {
        dprintf(2, "Failed to find the memory type to hold the image.\n");
        return false;
    }

    VkMemoryAllocateInfo memAllocInfo = {};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memAllocInfo.pNext = 0;
    memAllocInfo.allocationSize = memoryRequirements.size;
    memAllocInfo.memoryTypeIndex = (uint32_t)memoryTypeIndex;

    result = vkAllocateMemory(device, &memAllocInfo, 0, &imageMemory);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to allocate memory for the image. [%s]\n", VkResultToStr(result));
        return false;
    }

    result = vkBindImageMemory(device, image, imageMemory, 0);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to bind a image to her memory. [%s]\n", VkResultToStr(result));
        return false;
    }
    outImage = image;
    outImageMem = imageMemory;

    if (outImageViewPtr != 0) {
        VkImageViewCreateInfo imageViewCreateInfo = {};
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = 0;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = image;
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

        result = vkCreateImageView(device, &imageViewCreateInfo, 0, &imageView);
        if (result != VK_SUCCESS) {
            dprintf(2, "Failed to create image view. [%s]\n", VkResultToStr(result));
            return false;
        }

        *outImageViewPtr = imageView;
    }
    return true;
}