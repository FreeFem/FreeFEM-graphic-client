#include <algorithm>
#include "vkcommon.h"

int findMemoryTypeWithProperties(const VkPhysicalDeviceMemoryProperties memoryProperties,
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

VkResult createFence(const VkDevice device, VkFence &outFence)
{
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    return vkCreateFence(device, &fenceCreateInfo, 0, &outFence);
}
VkResult createSemaphore(const VkDevice device, VkSemaphore &outSemaphore)
{
    VkSemaphoreCreateInfo semaphoreCreateInfo = {};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    return vkCreateSemaphore(device, &semaphoreCreateInfo, 0, &outSemaphore);
}
