#ifndef BUFFER_H
#define BUFFER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../util/utils.h"
#include "vk_mem_alloc.h"

namespace FEM {
namespace gr {
class Manager;

/**
 * @brief Describe data layout in the buffer.
 */
struct AttributeDescription {
    uint32_t location;
    VkFormat format;
    size_t offset;
};

class Buffer {
   public:
    void destroy(const Manager& grm);

    // @brief Vulkan buffer handle getter.
    inline VkBuffer getHandle( ) const { return m_handle; }
    // @brief Vulkan buffer memory handle getter.
    inline VmaAllocation getMemory( ) const { return m_memory; }
    // @brief Buffer size getter.
    inline VkDeviceSize getSize( ) const { return m_size; }

   protected:
    ErrorValues initBuffer(const Manager& grm, VkBufferCreateInfo bufferInfos, VmaAllocationCreateInfo allocInfos,
                           VkDeviceSize size);

    VkBuffer m_handle = VK_NULL_HANDLE;
    VmaAllocation m_memory;
    VmaAllocationInfo m_allocInfo;
    VkDeviceSize m_size;
};

class VertexBuffer : public Buffer {
   public:
    /**
     * @brief Initialize the VertexBuffer.
     *
     * @param const Manager& grm[in] - Manager used to create the VertexBuffer.
     * @param void *data[in] - Pointer on the data used by the VertexBuffer.
     * @param VkDeviceSize size[in] - Complete size of data.
     * @param VkDeviceSize stride[in] - Size of one vertex.
     * @param std::vector<AttributeDescription> description[in] - Layout of the
     * VertexBuffer.
     * @param VkPrimitiveTopology topology[in] - Rendering primitive.
     *
     * @return ErrorValues - Returns ErrorValues::NONE if initialization is
     * successful.
     */
    ErrorValues init(const Manager& grm, void* data, VkDeviceSize elementCount, VkDeviceSize elementStride,
                     std::vector<AttributeDescription> description, VkPrimitiveTopology topology);

    // @brief Attribute number getter.
    inline size_t getAttributeNumber( ) const { return m_inputAttributeDescriptions.size( ); }
    // @brief Attribute getter.
    inline AttributeDescription getAttribute(uint32_t idx) const { return m_inputAttributeDescriptions[idx]; }
    // @brief Attributes getter.
    inline std::vector<AttributeDescription> getAttributes( ) const { return m_inputAttributeDescriptions; }
    // @brief Buffer stride getter.
    inline VkDeviceSize getStride( ) const { return m_stride; }
    // @brief Buffer topology getter.
    inline VkDeviceSize getVerticesCount( ) const { return m_count; }
    inline VkPrimitiveTopology getTopology( ) const { return m_topology; }

   private:
    size_t m_count = 0;
    size_t m_stride = 0;
    std::vector<AttributeDescription> m_inputAttributeDescriptions = {};
    VkPrimitiveTopology m_topology;
};

class IndexBuffer : public Buffer {
   public:
    /**
     * @brief Initialize the IndexBuffer.
     *
     * @param const Manager& grm[in] - Manager used to create the VertexBuffer.
     * @param void *data[in] - Pointer on the data used by the VertexBuffer.
     * @param VkDeviceSize size[in] - Complete size of data.
     * @param VkDeviceSize stride[in] - Size of one vertex.
     *
     * @return ErrorValues - Returns ErrorValues::NONE if initialization is
     * successful.
     */
    ErrorValues init(const Manager& grm, void* data, VkDeviceSize elementCount, VkDeviceSize elementStride);

    // @brief Buffer stride getter.
    inline VkDeviceSize getStride( ) const { return m_stride; };

   private:
    VkDeviceSize m_count = 0;
    VkDeviceSize m_stride = 0;
};

class UniformBuffer : public Buffer {
   public:
    /**
     * @brief Initialize the UniformBuffer.
     *
     * @param const Manager& grm[in] - Manager used to create the VertexBuffer.
     * @param void *data[in] - Pointer on the data used by the VertexBuffer.
     * @param VkDeviceSize stride[in] - Size of one vertex.
     * @param uitn32_t binding[in] - Shader uniform binding value.
     *
     * @return ErrorValues - Returns ErrorValues::NONE if initialization is
     * successful.
     */
    ErrorValues init(const Manager& grm, void* data, VkDeviceSize elementStride, uint32_t binding);

    // @brief Buffer stride getter.
    inline VkDeviceSize getStride( ) const { return m_stride; };
    // @brief Buffer binding index.
    inline uint32_t getBindingIdx( ) const { return m_binding; };

   private:
    VkDeviceSize m_stride = 0;
    uint32_t m_binding = -1;
};
}    // namespace gr
}    // namespace FEM
#endif    // BUFFER_H