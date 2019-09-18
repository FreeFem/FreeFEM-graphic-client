/**
 * @file ffBuffer.h
 * @brief Declaration of ffBuffer data type and it's related functions.
 */
#ifndef FF_BUFFER_H_
#define FF_BUFFER_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_mem_alloc.h"
#include "Array.h"

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
 * @brief Create a new ffBuffer, allocating GPU memory.
 *
 * @param Allocator [in] - VmaAllocator used to allocate ffGraph::Vulkan::ffBuffer's memory.
 * @param pCreateInfos [in] - Data used to create a ffGraph::Vulkan::ffBuffer.
 * @param pAllocateInfos [in] - Data used by VulkanMemoryAllocator to allocate memory to ffGraph::Vulkan::ffBuffer.
 */
ffBuffer ffCreateBuffer(VmaAllocator Allocator, ffBufferCreateInfo pCreateInfos,
                        VmaAllocationCreateInfo pAllocateInfos);

/**
 * @brief Map ffGraph::Array to ffGraph::Vulkan::ffBuffer
 *
 * @param Buffer [in] - ffGraph::Vulkan::ffBuffer to map data to.
 * @param Data [in] - ffGraph::Array containing the bytes to map.
 *
 * @return bool
 */
bool ffMapArrayToBuffer(ffBuffer Buffer, Array Data);

/**
 * @brief Map bytes to ffGraph::Vulkan::ffBuffer
 *
 * @param Buffer [in] - ffGraph::Vulkan::ffBuffer to map data to.
 * @param Data [in] - Pointer on bytes to map.
 *
 * @return bool
 */
bool ffMapDataToBuffer(ffBuffer Buffer, void *Data);

/**
 * @brief Destroy a ffBuffer.
 *
 * @param Allocator [in] - VmaAllocator used to allocate ffGraph::Vulkan::ffBuffer.
 * @param Buffer [in] - ffGraph::Vulkan::ffBuffer to destroy.
 *
 * @return void
 */
void ffDestroyBuffer(VmaAllocator Allocator, ffBuffer Buffer);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // FF_BUFFER_H_