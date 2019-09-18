/**
 * @file ffImage.h
 * @brief Declaration of ffImage data type and it's related functions.
 */
#ifndef FF_IMAGE_H_
#define FF_IMAGE_H_

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

namespace ffGraph {
namespace Vulkan {

struct ffImageCreateInfo {
    VkExtent2D Extent;
    VkFormat Format;
    VkBufferUsageFlags Usage;
    VkSharingMode SharingMode;
    bool AsView;
    struct ffImageViewCreateInfo {
        VkImageAspectFlags AspectMask;
    } ViewInfos;
};

struct ffImage {
    VkImage Handle;

    VkImageView View;

    VmaAllocation Memory;

    VmaAllocationInfo AllocationInfos;
};

/**
 * @brief Look if the Image allocation succeeded.
 *
 * @param Image [in] - ffImage on which the test is performed.
 * @return bool - Boolean value (true : ffBuffer is read / false : ffBuffer isn't ready).
 */
inline bool ffIsImageReady(ffImage Image) { return (Image.Handle == VK_NULL_HANDLE) ? false : true; }

/**
 * @brief Create a new ffGraph::Vulkan::ffImage, allocating memory.
 *
 * @param Allocator [in] - VmaAllocator used to allocate memory for the ffGraph::Vulkan::ffImage.
 * @param Device [in] - VkDevice used to create the VkImageView.
 * @param pCreateInfos [in] - Data needed to create a ffGraph::Vulkan::ffImage.
 * @param pAllocateInfos [in] - Data used by VulkanMemoryAllocator to allocate memory to ffGraph::Vulkan::ffImage.
 *
 * @return ffGraph::Vulkan::ffImage - Use ffGraph::Vulkan::ffisImageReady to check return value.
 */
ffImage ffCreateImage(const VmaAllocator& Allocator, const VkDevice& Device, ffImageCreateInfo pCreateInfos,
                      VmaAllocationCreateInfo pAllocateInfo);

/**
 * @brief Destroy a ffGraph::Vulkan::ffImage, releasing memory.
 *
 * @param Allocator [in] - VmaAllocator used to allocate ffGraph::Vulkan::ffImage's memory.
 * @param Device [in] - VkDevice used to create ffGraph::Vulkan::ffImage's VkImageView.
 * @param Image [in] - ffGraph::Vulkan::ffImage to destroy.
 *
 * @return void
 */
void ffDestroyImage(const VmaAllocator& Allocator, const VkDevice& Device, ffImage Image);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // FF_IMAGE_H_