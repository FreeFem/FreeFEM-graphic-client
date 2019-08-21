#ifndef GRAPH_CONTEXT_H
#define GRAPH_CONTEXT_H

#include <vulkan/vulkan.h>
#include <list>
#include <vector>
#include "../util/utils.h"
#include "../util/NonCopyable.h"
#include "Image.h"

namespace gr
{

    class Manager;

    class Context : public NonCopyable {
        public:

            FORCE_USE_RESULT Error init(const Manager&);
            FORCE_USE_RESULT Error reload(const Manager&);

            void destroy();

            inline VkSwapchainKHR getSwapchainKHR() const { return m_swapchain; }
            inline VkFormat getSurfaceFormat() const { return m_surfaceFormat; }
            inline VkFormat getDepthBufferFormat() const { return depthBufferFormat; }

        private:

            VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
            std::vector<VkImage> m_swapImages;
            std::vector<VkImageView> m_swapImageViews;
            VkFormat m_surfaceFormat;

            VkCommandBuffer m_cmdBuffer;

            Image m_depthImage;
            static constexpr VkFormat depthBufferFormat = VK_FORMAT_D16_UNORM;

            std::list<VkFramebuffer> m_framebuffers;

            FORCE_USE_RESULT Error initInternal(const Manager&);
            FORCE_USE_RESULT Error initSwapchain(const Manager&);
    };

} // namespace gr

#endif // GRAPH_CONTEXT_H