#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "Buffer.h"

namespace FEM {
namespace VK {

bool newBuffer(const VmaAllocator Allocator, Buffer *Buff, const BufferInfos Infos)
{
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = Infos.ElementSize * Infos.ElementCount;
    createInfo.usage = Infos.Usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    Buff->Type = BufferType::PBuffer;
    Buff->CreationInfos = Infos;
    if (vmaCreateBuffer(Allocator, &createInfo, &Infos.AllocInfos, &Buff->Handle, &Buff->Memory, &Buff->MemoryInfos))
        return false;
    return true;
}

bool newVertexBuffer(const VmaAllocator Allocator, VertexBuffer *Buff, const BufferInfos Infos, uint32_t AttributeCount, VkVertexInputAttributeDescription *Attributes)
{
    if (!newBuffer(Allocator, &Buff->VulkanData, Infos))
        return false;

    Buff->VulkanData.Type = BufferType::VBuffer;
    Buff->AttributeCount = AttributeCount;
    Buff->Attributes = (VkVertexInputAttributeDescription *)malloc(sizeof(VkVertexInputAttributeDescription) * AttributeCount);
    memcpy(Buff->Attributes, Attributes, sizeof(VkVertexInputAttributeDescription) * AttributeCount);
    return true;
}

bool newIndexBuffer(const VmaAllocator Allocator, IndexBuffer *Buff, const BufferInfos Infos, VkIndexType IndexType)
{
    if (!newBuffer(Allocator, &Buff->VulkanData, Infos))
        return false;

    Buff->VulkanData.Type = BufferType::IBuffer;
    Buff->IndexType = IndexType;
    return true;
}

void mapBufferMemory(Buffer *Buff, void *data)
{
    memcpy(Buff->MemoryInfos.pMappedData, data, Buff->MemoryInfos.size);
}

void destroyBuffer(const VmaAllocator Allocator, Buffer Buff)
{
    vmaDestroyBuffer(Allocator, Buff.Handle, Buff.Memory);
}

void destroyVertexBuffer(const VmaAllocator Allocator, VertexBuffer Buff)
{
    destroyBuffer(Allocator, Buff.VulkanData);
    free(Buff.Attributes);
}

void destroyIndexBuffer(const VmaAllocator Allocator, IndexBuffer Buff)
{
    destroyBuffer(Allocator, Buff.VulkanData);
}

}
}