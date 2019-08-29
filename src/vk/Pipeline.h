#ifndef PIPELINE_H
#define PIPELINE_H

#include <vulkan/vulkan.h>
#include <list>
#include <vector>
#include "../util/utils.h"
#include "Buffers.h"
#include "Shader.h"

namespace gr
{

    class Manager;
    class Context;

    /**
     * @brief Graphic pipeline, rendering a batch of buffers of the same time using the same shaders.
     * Hard coded Shaders loading for test purpose. (Will be changed later)
     */
    class Pipeline {
        public:
            /**
             * @brief Initialize a new pipeline.
             *
             * @param const Manager& grm - Graphic manager used to create the pipeline.
             * @param const Context& grm - Graphic context used to create the pipeline.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initilization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error init(const Manager&, const Context&);

            /**
             * @brief Initialize the pipeline's shaders
             *
             * @param const char *vertexShaderFilename[in] - Vertex shaders file name.
             * @param const char *fragmentShaderFilename[in] - Fragment shaders file name.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initilization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initShaders(const VkDevice& device, const char *vertexShaderFilename, const char *fragmentShaderFilename);

            /**
             * @brief Reload the pipeline.
             *
             * @param const Manager& grm - Graphic manager used to create the pipeline.
             * @param const Context& grm - Graphic context used to create the pipeline.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initilization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error reload(const Manager&, const Context&);

            /**
             * @brief Destroy the pipeline.
             */
            void destroy();

            /**
             * WIP
             */
            inline void addData(VertexBuffer vBuffer) { m_vertexBuffers.push_back(vBuffer); }

            inline uint32_t getVerticesCount() const {
                uint32_t ret = 0;
                for (const auto vBuffer : m_vertexBuffers) {
                    ret += vBuffer.getVerticesCount();
                }
                return ret;
            }

            // @brief Vulkan pipeline getter.
            inline VkPipeline getPipeline() const { return m_handle; }
            // @brief Vulkan pipeline layout getter.
            inline VkPipelineLayout getPipelineLayout() const { return m_layout; }
            // @brief Vulkan renderpass getter.
            inline VkRenderPass getRenderpass() const { return m_renderpass; }
            // @brief Vulkan framebuffers getter.
            inline std::vector<VkFramebuffer> getFramebuffers() const { return m_framebuffers; }
            // @brief Vulkan framebuffer getter.
            inline VertexBuffer getBuffer(uint32_t idx) const { return m_vertexBuffers[idx]; }
            // @brief VertexBuffer getter.
            inline std::vector<VertexBuffer> getBuffers() const { return m_vertexBuffers; }

            inline std::vector<VkDescriptorSet> getDescriptorSets() const { return m_descriptorSet; }

        private:
            VkPipeline m_handle;
            VkPipelineLayout m_layout;
            VkRenderPass m_renderpass;
            std::vector<VkFramebuffer> m_framebuffers = {};
            std::vector<VertexBuffer> m_vertexBuffers = {};
            VkDescriptorSetLayout m_descriptorSetLayout;
            std::vector<VkDescriptorSet> m_descriptorSet = {};
            std::vector<UniformBuffer> m_uniformBuffers = {};
            Shader m_shaders;

            /**
             * @brief Initialize pipelines's renderpass.
             *
             * @param const Manager& grm - Graphic manager used to create the pipeline.
             * @param const Context& grm - Graphic context used to create the pipeline.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initilization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initRenderpass(const Manager&, const Context&);

            /**
             * @brief Initialize pipeline's framebuffers.
             *
             * @param const Manager& grm - Graphic manager used to create the pipeline.
             * @param const Context& grm - Graphic context used to create the pipeline.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initilization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initFramebuffers(const Manager&, const Context&);

            /**
             * @brief Initialize pipeline's vulkan pipeline.
             *
             * @param const Manager& grm - Graphic manager used to create the pipeline.
             * @param const Context& grm - Graphic context used to create the pipeline.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initilization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initPipeline(const Manager&, UNUSED_PARAM const Context&);

            /**
             * @brief Initialize pipeline's vulkan descriptor set layout.
             *
             * @param const Manager& grm - Graphic manager used to create the pipeline.
             * @param const Context& grm - Graphic context used to create the pipeline.
             *
             * @return FORCE_USE_RESULT Error - Returns Error::NONE if initilization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initDescriptorSetLayout(const Manager&, UNUSED_PARAM const Context&);
    };

} // namespace gr

#endif // PIPELINE_H