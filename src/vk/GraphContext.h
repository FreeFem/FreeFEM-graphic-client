#ifndef GRAPH_CONTEXT_H
#define GRAPH_CONTEXT_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../util/utils.h"
#include "../util/NonCopyable.h"
#include "Image.h"
#include "Pipeline.h"
#include "Camera.h"

namespace gr
{

    class Manager;

    /**
     * @brief Vulkan data for a frame.
     */
    struct Frame {
        VkSemaphore acquiredSemaphore;
        VkSemaphore renderCompletedSemaphore;
        VkFence presentFence;
        bool fenceInitialized = false;
    };

    /**
     * @brief Context used to render object, is reloaded when the window change size.
     * Cannot be copyed.
     */
    class Context : public NonCopyable {
        public:

            /**
             * @brief Initialized a context.
             *
             * @param const Manager& grm[in] - Graphic manager used to create the context.
             *
             * @return FORCE_USE_RESULT Error - Return Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error init(const Manager&);

            /**
             * @brief Reload the context.
             *
             * @param const Manager& grm[in] - Graphic manager used to create the context.
             *
             * @return FORCE_USE_RESULT Error - Return Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error reload(const Manager&);

            /**
             * @brief Destroy the context.
             */
            void destroy();

            /**
             * @brief Swap rendering buffer.
             */
            inline void swapBuffer() { current_frame = !current_frame; }

            /**
             * @brief SwapchainKHR getter.
             */
            inline VkSwapchainKHR getSwapchainKHR() const { return m_swapchain; }

            /**
             * @brief Surface format getter.
             */
            inline VkFormat getSurfaceFormat() const { return m_surfaceFormat; }

            /**
             * @brief Depth buffer format getter.
             */
            inline VkFormat getDepthBufferFormat() const { return depthBufferFormat; }

            /**
             * @brief Swapchain image views getter.
             */
            inline std::vector<VkImageView> get_swapImageViews() const { return m_swapImageViews; }

            /**
             * @brief Depth image getter.
             */
            inline Image get_depthImage() const { return m_depthImage; }

            /**
             * @brief Render a frame.
             */
            Error render(const Manager& grm);

            /**
             * @brief add a pipeline to the context
             *
             * @param const Manager& grm[in] - Graphic manager used to create the context.
             * @param VertexBuffer& object[in] - Renderable object.
             * @param const char *vertexShaderFilename[in] - Vertex shaders file name.
             * @param const char *fragmentShaderFilename[in] - Fragment shaders file name.
             *
             * @return Error - Return Error::NONE if initialization is successful.
             */
            Error addPipeline(const Manager& grm, VertexBuffer& object, const char *vertexShaderFile, const char *fragmentShaderFile);

            /**
             * @brief Remove all user created pipeline.
             */
            void clearPipeline();

            float m_animTime = 0.5f;

        private:
            VkCommandPool m_commandPool = VK_NULL_HANDLE;

            VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
            std::vector<VkImage> m_swapImages = {};
            std::vector<VkImageView> m_swapImageViews = {};
            VkFormat m_surfaceFormat;

            VkCommandBuffer m_cmdBuffer;

            Camera m_camera;

            uint8_t current_frame = 0;
            Frame m_perFrame[2];
            VkCommandBuffer m_presentCmdBuffer[2];

            std::vector<Pipeline> m_pipelines;

            Image m_depthImage;
            static constexpr VkFormat depthBufferFormat = VK_FORMAT_D16_UNORM;

            /**
             * @brief Initialized a context's internal data.
             *
             * @param const Manager& grm[in] - Graphic manager used to create the context.
             *
             * @return FORCE_USE_RESULT Error - Return Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initInternal(const Manager&);

            /**
             * @brief Initialized a context's swapchain.
             *
             * @param const Manager& grm[in] - Graphic manager used to create the context.
             *
             * @return FORCE_USE_RESULT Error - Return Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error initSwapchain(const Manager&);

            /**
             * @brief Fill initialization commandbuffer, posting the depth image on the queue.
             *
             * @param const Manager& grm[in] - Graphic manager used to create the context.
             *
             * @return FORCE_USE_RESULT Error - Return Error::NONE if initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error fillInitCmdBuffer(const Manager&);

    };

} // namespace gr

#endif // GRAPH_CONTEXT_H