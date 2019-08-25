#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>
#include <list>
#include <vector>
#include "../util/utils.h"
#include "VertexBuffer.h"

namespace gr
{

    class Manager;
    class Context;

    /**
     * Hard coded Shaders loading for test purpose. (Will be changed later)
     * Hard coded the vertex buffer aswell
     */
    class Pipeline {
        public:
            FORCE_USE_RESULT Error init(const Manager&, const Context&);
            FORCE_USE_RESULT Error reload(const Manager&, const Context&);

            void destroy();

            Error addData();

            inline VkPipeline getPipeline() const { return m_handle; }
            inline VkPipelineLayout getPipelineLayout() const { return m_layout; }
            inline VkRenderPass getRenderpass() const { return m_renderpass; }
            inline std::vector<VkFramebuffer> getFramebuffers() const { return m_framebuffers; }
            inline VertexBuffer getBuffer(uint32_t idx) const { return m_vertexBuffers[idx]; }
            inline std::vector<VertexBuffer> getBuffers() const { return m_vertexBuffers; }

        private:
            VkPipeline m_handle;
            VkPipelineLayout m_layout;
            VkRenderPass m_renderpass;
            std::vector<VkFramebuffer> m_framebuffers = {};
            std::vector<VertexBuffer> m_vertexBuffers = {};

            FORCE_USE_RESULT Error initRenderpass(const Manager&, const Context&);
            FORCE_USE_RESULT Error initFramebuffers(const Manager&, const Context&);
            FORCE_USE_RESULT Error initPipeline(const Manager&, const Context&);
    };

} // namespace gr

#endif // PIPELINE_H