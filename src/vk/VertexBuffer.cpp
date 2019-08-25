#include <cstring>
#include "VertexBuffer.h"
#include "GraphManager.h"
#include "vkcommon.h"

namespace gr
{

    Error VertexBuffer::init(const Manager& grm, void *data, size_t size, size_t stride, std::vector<AttributeDescription> description, VkPrimitiveTopology topology)
    {
        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = 0;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.size = size;
        bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(grm.getDevice(), &bufferCreateInfo, 0, &m_handle))
            return Error::FUNCTION_FAILED;

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(grm.getDevice(), m_handle, &memoryRequirements);

        int memoryTypeIndex = findMemoryTypeWithProperties(grm.getPhysicalDeviceMemProps(), memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);
        if (memoryTypeIndex < 0)
            return Error::FUNCTION_FAILED;

        VkMemoryAllocateInfo memAllocInfo = {};
        memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memAllocInfo.allocationSize = memoryRequirements.size;
        memAllocInfo.memoryTypeIndex = (uint32_t)memoryTypeIndex;

        if (vkAllocateMemory(grm.getDevice(), &memAllocInfo, 0, &m_memory))
            return Error::FUNCTION_FAILED;
        if (vkBindBufferMemory(grm.getDevice(), m_handle, m_memory, 0))
            return Error::FUNCTION_FAILED;
        {
            vkMapMemory(grm.getDevice(), m_memory, 0, VK_WHOLE_SIZE, 0, &m_mappedMemory);

            memcpy(m_mappedMemory, data, size);
            vkUnmapMemory(grm.getDevice(), m_memory);
        }
        for (auto descriptor : description)
            m_inputAttributeDescriptions.push_back(descriptor);
        m_size = size;
        m_stride = stride;
        m_topology = topology;
        return Error::NONE;
    }

    void VertexBuffer::destroy(const Manager& grm)
    {
        vkFreeMemory(grm.getDevice(), m_memory, 0);
        vkDestroyBuffer(grm.getDevice(), m_handle, 0);
    }

} // namespace gr
