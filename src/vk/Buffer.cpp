#include "Buffer.h"
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace FEM {
namespace VK {

bool newBuffer(const VmaAllocator Allocator, BufferStorage *Storage, Buffer *Buff, const BufferInfos Infos) {
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = Infos.ElementSize * Infos.ElementCount;
    createInfo.usage = Infos.Usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    Buff->CreationInfos = Infos;
    if (vmaCreateBuffer(Allocator, &createInfo, &Infos.AllocInfos, &Buff->Handle, &Buff->Memory, &Buff->MemoryInfos))
        return false;
    Storage->push_back(*Buff);
    return true;
}

bool newVertexBuffer(const VmaAllocator Allocator, BufferStorage *Storage, VertexBuffer *Buff, const BufferInfos Infos, uint32_t AttributeCount,
                     VkVertexInputAttributeDescription *Attributes) {
    if (!newBuffer(Allocator, Storage, &Buff->VulkanData, Infos)) return false;

    Buff->AttributeCount = AttributeCount;
    Buff->Attributes =
        (VkVertexInputAttributeDescription *)malloc(sizeof(VkVertexInputAttributeDescription) * AttributeCount);
    memcpy(Buff->Attributes, Attributes, sizeof(VkVertexInputAttributeDescription) * AttributeCount);
    return true;
}

bool newIndexBuffer(const VmaAllocator Allocator, BufferStorage *Storage, IndexBuffer *Buff, const BufferInfos Infos, VkIndexType IndexType) {
    if (!newBuffer(Allocator, Storage, &Buff->VulkanData, Infos)) return false;

    Buff->IndexType = IndexType;
    return true;
}

void mapBufferMemory(Buffer *Buff, void *data) { memcpy(Buff->MemoryInfos.pMappedData, data, Buff->MemoryInfos.size); }

void destroyBuffer(const VmaAllocator Allocator, Buffer Buff) { vmaDestroyBuffer(Allocator, Buff.Handle, Buff.Memory); }

void destroyVertexBuffer(const VmaAllocator Allocator, VertexBuffer Buff) {
    free(Buff.Attributes);
}

void destroyIndexBuffer(const VmaAllocator Allocator, IndexBuffer Buff) { destroyBuffer(Allocator, Buff.VulkanData); }

void destroyBufferStorage(const VmaAllocator Allocator, BufferStorage Storage)
{
    for (auto ite : Storage) {
        destroyBuffer(Allocator, ite);
    }
}

}    // namespace VK
}    // namespace FEM