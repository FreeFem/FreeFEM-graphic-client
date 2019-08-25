#ifndef VK_COMMON_H
#define VK_COMMON_H

#include <vulkan/vulkan.h>

int findMemoryTypeWithProperties(const VkPhysicalDeviceMemoryProperties memoryProperties,
                                 const uint32_t memoryTypeBits,
                                 const VkMemoryPropertyFlags requiredMemoryProperties);

VkResult createFence(const VkDevice device, VkFence &outFence);
VkResult createSemaphore(const VkDevice device, VkSemaphore &outSemaphore);

#endif // VK_COMMON_H