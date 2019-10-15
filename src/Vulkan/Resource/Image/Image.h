/**
 * @file Image.h
 * @brief Declaration of Image data type and it's related functions.
 */
#ifndef IMAGE_H_
#define IMAGE_H_

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

namespace ffGraph {
namespace Vulkan {

struct ImageCreateInfo {
    bool AsView;
    VkExtent2D Extent;
    VkFormat Format;
    VkSampleCountFlagBits SampleCount;
    VkBufferUsageFlags Usage;
    VkSharingMode SharingMode;
    uint32_t QueueIndexCount;
    uint32_t QueueIndices[3];
    struct ImageViewCreateInfo {
        VkImageAspectFlags AspectMask;
    } ViewInfos;
};

struct Image {
    VkImage Handle;

    VkImageView View;

    VmaAllocation Memory;

    VmaAllocationInfo AllocationInfos;
};

/**
 * @brief Look if the Image allocation succeeded.
 *
 * @param Image [in] - Image on which the test is performed.
 * @return bool - Boolean value (true : ffBuffer is read / false : ffBuffer isn't ready).
 */
inline bool IsImageReady(Image Image) { return (Image.Handle == VK_NULL_HANDLE) ? false : true; }

/**
 * @brief Create a new ffGraph::Vulkan::Image, allocating memory.
 *
 * @param Allocator [in] - VmaAllocator used to allocate memory for the ffGraph::Vulkan::Image.
 * @param Device [in] - VkDevice used to create the VkImageView.
 * @param pCreateInfos [in] - Data needed to create a ffGraph::Vulkan::Image.
 * @param pAllocateInfos [in] - Data used by VulkanMemoryAllocator to allocate memory to ffGraph::Vulkan::Image.
 *
 * @return ffGraph::Vulkan::Image - Use ffGraph::Vulkan::ffisImageReady to check return value.
 */
Image CreateImage(const VmaAllocator& Allocator, const VkDevice& Device, ImageCreateInfo pCreateInfos,
                  VmaAllocationCreateInfo pAllocateInfo);

/**
 * @brief Destroy a ffGraph::Vulkan::Image, releasing memory.
 *
 * @param Allocator [in] - VmaAllocator used to allocate ffGraph::Vulkan::Image's memory.
 * @param Device [in] - VkDevice used to create ffGraph::Vulkan::Image's VkImageView.
 * @param Image [in] - ffGraph::Vulkan::Image to destroy.
 *
 * @return void
 */
void DestroyImage(const VmaAllocator& Allocator, const VkDevice& Device, Image Image);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // IMAGE_H_