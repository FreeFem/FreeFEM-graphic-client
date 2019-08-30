#ifndef VK_COMMON_H
#define VK_COMMON_H

#include <vulkan/vulkan.h>

namespace FEM {
namespace gr {
/**
 * @brief Find the memory type based on the device memory properties.
 *
 * @param const VkPhysicalDeviceMemoryProperties memoryProperties[in] - Physical
 * device memory properties.
 * @param const uint32_t memoryTypeBits[in] - Memory type.
 * @param const VkMemoryPropertyFlags requiredMemoryProperties[in] - Required
 * memory properties.
 *
 * @return int - returns -1 if function fails else returns the value of the
 * memory type.
 */
int findMemoryTypeWithProperties(const VkPhysicalDeviceMemoryProperties memoryProperties, const uint32_t memoryTypeBits,
                                 const VkMemoryPropertyFlags requiredMemoryProperties);

/**
 * @brief Create a vulkan fence
 *
 * @param const VkDevice device[in] - Device which the fence will be used on.
 * @param VkFence &outFence[out] - Fence to be created.
 *
 * @return VkResult - Returns VK_SUCCESS if the creation is successful.
 */
VkResult createFence(const VkDevice device, VkFence &outFence);

/**
 * @brief Create a vulkan semaphore
 *
 * @param const VkDevice device[in] - Device which the fence will be used on.
 * @param VkFence &outFence[out] - Semaphore to be created.
 *
 * @return VkResult - Returns VK_SUCCESS if the creation is successful.
 */
VkResult createSemaphore(const VkDevice device, VkSemaphore &outSemaphore);
}    // namespace gr
}    // namespace FEM

#endif    // VK_COMMON_H