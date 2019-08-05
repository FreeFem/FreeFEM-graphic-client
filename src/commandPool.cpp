#include "instance.h"
#include "utils.h"

bool createCommandPool(const VkDevice device,
                       const uint32_t queueFamilyIndex,
                       const VkCommandPoolCreateFlagBits createFlagBits,
                       VkCommandPool& outCommandPool
                      )
{
    VkResult result;

    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.flags = createFlagBits;
    createInfo.queueFamilyIndex = queueFamilyIndex;

    result = vkCreateCommandPool(device, &createInfo, 0, &outCommandPool);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to create command pool. [%s]\n", VkResultToStr(result));
        return false;
    }
    return true;
}

bool allocateCommandBuffer(const VkDevice device,
                           const VkCommandPool commandPool,
                           const VkCommandBufferLevel commandBufferLevel,
                           VkCommandBuffer& outCommandBuffer
                          )
{
    VkResult result;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = 0;
    allocInfo.commandPool = commandPool;
    allocInfo.level = commandBufferLevel;
    allocInfo.commandBufferCount = 1;

    result = vkAllocateCommandBuffers(device, &allocInfo, &outCommandBuffer);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to create command buffer. [%s]\n", VkResultToStr(result));
        return false;
    }
    return true;
}