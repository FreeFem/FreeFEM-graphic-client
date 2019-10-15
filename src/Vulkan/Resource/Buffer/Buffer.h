#ifndef BUFFER_H_
#define BUFFER_H_

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

namespace ffGraph {
namespace Vulkan {

struct BufferCreateInfo {
    struct {
        VkDeviceSize Size = 0;
        VkBufferUsageFlags Usage;
        VkSharingMode SharingMode = VK_SHARING_MODE_MAX_ENUM;
        uint32_t queueFamilyIndexCount = 0;    // Optional
        uint32_t *pQueueFamilyIndices = 0;     // Optional
    } vkData;

    struct {
        VmaMemoryUsage Usage;
        VmaAllocationCreateFlags flags;
        VkMemoryPropertyFlags requiredFlags;
        VkMemoryPropertyFlags preferredFlags;
    } vmaData;
};

struct Buffer {
    VkBuffer Handle = VK_NULL_HANDLE;
    VmaAllocation Memory;
    VmaAllocationInfo Infos;
};

Buffer CreateBuffer(VmaAllocator Allocator, BufferCreateInfo pCreateInfo);
void DestroyBuffer(VmaAllocator Allocator, Buffer toDestroy);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // BUFFER_H_