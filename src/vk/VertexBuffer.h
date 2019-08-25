#ifndef VERTEX_BUFFER_H
#define VERTEX_BUFFER_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../util/utils.h"


namespace gr
{
    class Manager;

    struct AttributeDescription {
        uint32_t location;
        VkFormat format;
        size_t offset;
    };

    class VertexBuffer {
        public:
            Error init(const Manager& grm, void *data, size_t size, size_t stride, std::vector<AttributeDescription> description, VkPrimitiveTopology topology);

            void destroy(const Manager& grm);

            inline VkBuffer getBuffer() const { return m_handle; }
            inline VkDeviceMemory getMemory() const { return m_memory; }
            inline size_t getAttributeNumber() const { return m_inputAttributeDescriptions.size(); }
            inline AttributeDescription getAttribute(uint32_t idx) const { return m_inputAttributeDescriptions[idx]; }
            inline std::vector<AttributeDescription> getAttributes() const { return m_inputAttributeDescriptions; }

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