#ifndef BUFFER_H_
#define BUFFER_H_

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

namespace FEM {
namespace VK {

enum BufferType { Unknow, PBuffer, VBuffer, IBuffer };

struct BufferInfos {
    VkDeviceSize ElementCount;
    VkDeviceSize ElementSize;
    VkBufferUsageFlags Usage;
    VmaAllocationCreateInfo AllocInfos;
};

struct Buffer {
    uint8_t Type;
    VkBuffer Handle;
    VmaAllocation Memory;
    VmaAllocationInfo MemoryInfos;
    BufferInfos CreationInfos;
};

struct VertexBuffer {
    Buffer VulkanData;
    uint32_t AttributeCount;
    VkVertexInputAttributeDescription *Attributes;
};

struct IndexBuffer {
    Buffer VulkanData;
    VkIndexType IndexType;
};

bool newBuffer(const VmaAllocator Allocator, Buffer *Buff, const BufferInfos Infos);

bool newIndexBuffer(const VmaAllocator Allocator, IndexBuffer *Buff, const BufferInfos Infos, VkIndexType IndexType);

bool newVertexBuffer(const VmaAllocator Allocator, VertexBuffer *Buff, const BufferInfos Infos, uint32_t AttributeCount,
                     VkVertexInputAttributeDescription *Attributes);

void mapBufferMemory(Buffer *Buff, void *data);

void destroyBuffer(const VmaAllocator Allocator, Buffer Buff);

void destroyVertexBuffer(const VmaAllocator Allocator, Buffer Buff);

void destroyIndexBuffer(const VmaAllocator Allocator, Buffer Buff);

}    // namespace VK
}    // namespace FEM

#endif    // BUFFER_H_