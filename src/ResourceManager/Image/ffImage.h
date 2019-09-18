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

ffImage ffCreateImage(const VmaAllocator&, const VkDevice&, ffImageCreateInfo, VmaAllocationCreateInfo);

void ffDestroyImage(const VmaAllocator& Allocator, const VkDevice& Device, ffImage Image);

} // namespace Vulkan
} // namespace ffGraph


#endif // FF_IMAGE_H_