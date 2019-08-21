#ifndef GRAPH_CONTEXT_H
#define GRAPH_CONTEXT_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../util/utils.h"
#include "../util/NonCopyable.h"
#include "Image.h"

namespace gr
{

    class Manager;

    struct Frame {
        VkSemaphore acquiredSemaphore;
        VkSemaphore renderCompletedSemaphore;
        VkFence presentFence;
        bool fenceInitialized = false;
    };

    class Context : public NonCopyable {
        public:

            FORCE_USE_RESULT Error init(const Manager&);
            FORCE_USE_RESULT Error reload(const Manager&);

            void destroy();

            inline void swapBuffer() { current_frame = !current_frame; }
            inline VkSwapchainKHR getSwapchainKHR() const { return m_swapchain; }
            inline VkFormat getSurfaceFormat() const { return m_surfaceFormat; }
            inline VkFormat getDepthBufferFormat() const { return depthBufferFormat; }

        private:

            VkCommandPool m_commandPool = VK_NULL_HANDLE;

            VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
            std::vector<VkImage> m_swapImages = {};
            std::vector<VkImageView> m_swapImageViews = {};
            VkFormat m_surfaceFormat;

            VkRenderPass m_renderpass;

            VkCommandBuffer m_cmdBuffer;

            uint8_t current_frame = 0;
            Frame m_perFrame[2];
            VkCommandBuffer m_presentCmdBuffer[2];

            Image m_depthImage;
            static constexpr VkFormat depthBufferFormat = VK_FORMAT_D16_UNORM;

            std::vector<VkFramebuffer> m_framebuffers;

            FORCE_USE_RESULT Error initInternal(const Manager&);
            FORCE_USE_RESULT Error initSwapchain(const Manager&);
            FORCE_USE_RESULT Error initRenderpass(const Manager&);
            FORCE_USE_RESULT Error initFramebuffer(const Manager&);
    };

} // namespace gr

#endif // GRAPH_CONTEXT_H