#ifndef FF_BUFFER_H_
#define FF_BUFFER_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_mem_alloc.h"
#include "util/Array.h"

namespace ffGraph {
namespace Vulkan {

/**
 * @brief Data used to create a new ffBuffer.
 */
struct ffBufferCreateInfo {
    // @brief Number of Elements.
    VkDeviceSize ElementCount;
    // @brief Size of one Element.
    VkDeviceSize ElementSize;
    // @brief Vulkan buffer usage.
    VkBufferUsageFlags Usage;
    // @brief Vulkan buffer sharing mode.
    VkSharingMode SharingMode;
};

/**
 * @brief Store data on a buffer from Vulkan and VulkanMemoryAllocator.
 */
struct ffBuffer {

    // @brief Vulkan buffer handle.
    VkBuffer Handle;

    // @brief VulkanMemoryAllocator allocation handle.
    VmaAllocation Memory;

    // @brief VulkanMemoryAllocator allocation informations.
    VmaAllocationInfo AllocationInfos;
};

/**
 * @brief Look if the buffer allocation succeeded.
 *
 * @param ffBuffer [in] - ffBuffer on which the test is performed.
 * @return bool - Boolean value (true : ffBuffer is read / false : ffBuffer isn't ready).
 */
inline bool ffIsBufferReady(ffBuffer Buffer) { return (Buffer.Handle == VK_NULL_HANDLE) ? false : true; }

/**
 * @brief Create a new ffBuffer.
 */
ffBuffer ffCreateBuffer(VmaAllocator, ffBufferCreateInfo, VmaAllocationCreateInfo);

/**
 * @brief Map ffGraph::Array to ffGraph::Vulkan::ffBuffer
 */
bool ffMapArrayToBuffer(ffBuffer, Array);

/**
 * @brief Map array to ffGraph::Vulkan::ffBuffer
 */
bool ffMapDataToBuffer(ffBuffer, void *);

/**
 * @brief Destroy a ffBuffer.
 */
void ffDestroyBuffer(VmaAllocator, ffBuffer);

}
}

#endif // FF_BUFFER_H_