#include <cstring>
#include "GraphManager.h"
#include "Buffers.h"
#include "vkcommon.h"

namespace gr
{

    Error Buffer::initBuffer(const Manager& grm, VkBufferCreateInfo bufferInfos, VmaAllocationCreateInfo allocInfos, VkDeviceSize size)
    {
        if (vmaCreateBuffer(grm.getAllocator(), &bufferInfos, &allocInfos, &m_handle, &m_memory, &m_allocInfo))
            return Error::FUNCTION_FAILED;

        m_size = size;
        return Error::NONE;
    }

    void Buffer::destroy(const Manager& grm)
    {
        vmaDestroyBuffer(grm.getAllocator(), m_handle, m_memory);
    }

    Error VertexBuffer::init(const Manager& grm, void *data, VkDeviceSize count, VkDeviceSize stride, std::vector<AttributeDescription> description, VkPrimitiveTopology topology)
    {
        VkBufferCreateInfo bufferInfos = {};
        bufferInfos.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfos.size = count * stride;
        bufferInfos.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfos.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfos = {};
        allocInfos.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        allocInfos.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        allocInfos.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        allocInfos.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        initBuffer(grm, bufferInfos, allocInfos, count * stride);

        memcpy(m_allocInfo.pMappedData, data, count * stride);

        for (auto descriptor : description)
            m_inputAttributeDescriptions.push_back(descriptor);
        m_count = count;
        m_stride = stride;
        m_topology = topology;
        return Error::NONE;
    }

    Error IndexBuffer::init(const Manager& grm, void *data, VkDeviceSize count, VkDeviceSize stride)
    {
        VkBufferCreateInfo bufferInfos = {};
        bufferInfos.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfos.size = count * stride;
        bufferInfos.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferInfos.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfos = {};
        allocInfos.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        allocInfos.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        allocInfos.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        allocInfos.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        initBuffer(grm, bufferInfos, allocInfos, count * stride);

        memcpy(m_allocInfo.pMappedData, data, count * stride);
        m_count = count;
        m_stride = stride;
        return Error::NONE;
    }

    Error UniformBuffer::init(const Manager& grm, void *data, VkDeviceSize stride, uint32_t binding)
    {
        VkBufferCreateInfo bufferInfos = {};
        bufferInfos.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfos.size = stride;
        bufferInfos.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfos.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfos = {};
        allocInfos.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        allocInfos.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        allocInfos.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
        allocInfos.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        initBuffer(grm, bufferInfos, allocInfos, stride);

        memcpy(m_allocInfo.pMappedData, data, stride);
        m_binding = binding;
        m_stride = stride;
        return Error::NONE;
    }

} // namespace gr
