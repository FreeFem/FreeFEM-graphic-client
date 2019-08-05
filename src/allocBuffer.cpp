#include <cstdio>
#include "utils.h"

bool createAndAllocateBuffer(const VkDevice device,
                             const VkPhysicalDeviceMemoryProperties memProps,
                             const VkBufferUsageFlags bufferUsage,
                             const VkMemoryPropertyFlags requiredMemProps,
                             const VkDeviceSize bufferSize,
                             VkBuffer& outBuffer,
                             VkDeviceMemory& outBufferMem)
{
    VkResult result;
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.pNext = 0;
    bufferCreateInfo.flags = 0;
    bufferCreateInfo.size = bufferSize;
    bufferCreateInfo.usage = bufferUsage;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferCreateInfo.queueFamilyIndexCount = 0;
    bufferCreateInfo.pQueueFamilyIndices = 0;

    result = vkCreateBuffer(device, &bufferCreateInfo, 0, &buffer);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to create buffer. [%s]\n", VkResultToStr(result));
        return false;
    }

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

    int memoryTypeIndex = findMemoryTypeWithProperties(memProps, memoryRequirements.memoryTypeBits, requiredMemProps);
    if (memoryTypeIndex < 0) {
        dprintf(2, "Failed to find the memory type to hold the image.\n");
        return false;
    }

    VkMemoryAllocateInfo memoryAllocateInfo = {};
    memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memoryAllocateInfo.pNext = 0;
    memoryAllocateInfo.allocationSize = memoryRequirements.size;
    memoryAllocateInfo.memoryTypeIndex = (uint32_t)memoryTypeIndex;

    result = vkAllocateMemory(device, &memoryAllocateInfo, 0, &bufferMemory);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to allocate buffer memory. [%s]\n", VkResultToStr(result));
        return false;
    }

    result = vkBindBufferMemory(device, buffer, bufferMemory, 0);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to bind a buffer to his memory. [%s]\n", VkResultToStr(result));
        return false;
    }

    outBuffer = buffer;
    outBufferMem = bufferMemory;

    return true;
}