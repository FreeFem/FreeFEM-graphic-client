#ifndef BUFFER_H_
#define BUFFER_H_

#include <vulkan/vulkan.h>
#include <vector>
#include "vk_mem_alloc.h"

namespace FEM {
namespace VK {

struct BufferInfos {
    VkDeviceSize ElementCount;
    VkDeviceSize ElementSize;
    VkBufferUsageFlags Usage;
    VmaAllocationCreateInfo AllocInfos;
};

struct Buffer {
    char *BufferName;
    VkBuffer Handle;
    VmaAllocation Memory;
    VmaAllocationInfo MemoryInfos;
    BufferInfos CreationInfos;
};

typedef std::vector<Buffer> BufferStorage;

struct VertexBuffer {
    Buffer VulkanData;
    uint32_t AttributeCount;
    VkVertexInputAttributeDescription *Attributes;
};

struct IndexBuffer {
    Buffer VulkanData;
    VkIndexType IndexType;
};

bool newBuffer(const VmaAllocator Allocator, BufferStorage *Storage, Buffer *Buff, const BufferInfos Infos);

bool newIndexBuffer(const VmaAllocator Allocator, BufferStorage *Storage, IndexBuffer *Buff, const BufferInfos Infos, VkIndexType IndexType);

bool newVertexBuffer(const VmaAllocator Allocator, BufferStorage *Storage, VertexBuffer *Buff, const BufferInfos Infos, uint32_t AttributeCount,
                     VkVertexInputAttributeDescription *Attributes);

void mapBufferMemory(Buffer *Buff, void *data);

void destroyBuffer(const VmaAllocator Allocator, Buffer Buff);

void destroyVertexBuffer(const VmaAllocator Allocator, VertexBuffer Buff);

void destroyIndexBuffer(const VmaAllocator Allocator, Buffer Buff);

void destroyBufferStorage(const VmaAllocator Allocator, BufferStorage Storage);

}    // namespace VK
}    // namespace FEM

#endif    // BUFFER_H_