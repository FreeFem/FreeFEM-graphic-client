#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../util/utils.h"


namespace gr
{
    class Manager;

    /**
     * @brief Describe data layout in the buffer.
     */
    struct AttributeDescription {
        uint32_t location;
        VkFormat format;
        size_t offset;
    };

    /**
     * @brief Renderable object.
     */
    class VertexBuffer {
        public:
            /**
             * @brief Initialize the VertexBuffer.
             *
             * @param const Manager& grm[in] - Manager used to create the VertexBuffer.
             * @param void *data[in] - Pointer on the data used by the VertexBuffer.
             * @param size_t size[in] - Complete size of data.
             * @param size_t stride[in] - Size of one vertex.
             * @param std::vector<AttributeDescription> description[in] - Layout of the VertexBuffer.
             * @param VkPrimitiveTopology topology[in] - Rendering primitive.
             *
             * @return Error - Returns Error::NONE if initialization is successful.
             */
            Error init(const Manager& grm, void *data, size_t size, size_t stride, std::vector<AttributeDescription> description, VkPrimitiveTopology topology);

            /**
             * @brief Destroy the VertexBuffer.
             */
            void destroy(const Manager& grm);

            // @brief Vulkan buffer handle getter.
            inline VkBuffer getBuffer() const { return m_handle; }
            // @brief Vulkan buffer memory handle getter.
            inline VkDeviceMemory getMemory() const { return m_memory; }
            // @brief Attribute number getter.
            inline size_t getAttributeNumber() const { return m_inputAttributeDescriptions.size(); }
            // @brief Attribute getter.
            inline AttributeDescription getAttribute(uint32_t idx) const { return m_inputAttributeDescriptions[idx]; }
            // @brief Attributes getter.
            inline std::vector<AttributeDescription> getAttributes() const { return m_inputAttributeDescriptions; }
            // @brief Buffer stride getter.
            inline size_t getStride() const { return m_stride; }
            // @brief Buffer size getter.
            inline size_t getSize() const { return m_size; }
            // @brief Buffer topology getter.
            inline VkPrimitiveTopology getTopology() const { return m_topology; }
        private:

            VkBuffer m_handle = VK_NULL_HANDLE;
            VkDeviceMemory m_memory = VK_NULL_HANDLE;
            void *m_mappedMemory = NULL;
            size_t m_size = 0;
            size_t m_stride = 0;
            std::vector<AttributeDescription> m_inputAttributeDescriptions = {};
            VkPrimitiveTopology m_topology;
    };

} // namespace gr

#endif // VERTEX_BUFFER_H