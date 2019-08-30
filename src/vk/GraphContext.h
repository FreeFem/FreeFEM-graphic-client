#ifndef GRAPH_CONTEXT_H
#define GRAPH_CONTEXT_H

#include <vulkan/vulkan.h>
#include <vector>
#include "../util/NonCopyable.h"
#include "../util/utils.h"
#include "Image.h"

namespace FEM {

namespace gr {

class Manager;
class Image;
class VertexBuffer;

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
 * @brief Context used to render object, is reloaded when the window change
 * size. Cannot be copyed.
 */
class Context : public NonCopyable {
   public:
    /**
     * @brief Initialized a context.
     *
     * @param const Manager& grm[in] - Graphic manager used to create the
     * context.
     *
     * @return FORCE_USE_RESULT ErrorValues - Return ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues init(const Manager&);

    /**
     * @brief Destroy the context.
     *
     * @param const Manager& grm[in] - Graphic manager used to create the
     * context.
     */
    void destroy(const Manager&);

    /**
     * @brief Render a frame.
     */
    // ErrorValues render(const Manager& grm);

    VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> SwapImages = {};
    std::vector<VkImageView> SwapImageViews = {};
    std::vector<VkFramebuffer> FrameBuffers = {};
    VkFormat SurfaceFormat;

    VkCommandBuffer InitializerCommandBuffer;

    uint8_t CurrentFrame = 0;
    Frame PerFrame[2];
    VkCommandBuffer PresentCmdBuffer[2];

    Image DepthImage;
    static constexpr VkFormat DepthBufferFormat = VK_FORMAT_D16_UNORM;

   private:
    /**
     * @brief Initialized a context's internal data.
     *
     * @param const Manager& grm[in] - Graphic manager used to create the
     * context.
     *
     * @return FORCE_USE_RESULT ErrorValues - Return ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues initInternal(const Manager&);

    /**
     * @brief Initialized a context's swapchain.
     *
     * @param const Manager& grm[in] - Graphic manager used to create the
     * context.
     *
     * @return FORCE_USE_RESULT ErrorValues - Return ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues initSwapchain(const Manager&);

    /**
     * @brief Fill initialization commandbuffer, posting the depth image on the
     * queue.
     *
     * @param const Manager& grm[in] - Graphic manager used to create the
     * context.
     *
     * @return FORCE_USE_RESULT ErrorValues - Return ErrorValues::NONE if
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues fillInitCmdBuffer(const Manager&);
};

}    // namespace gr
}    // namespace FEM

#endif    // GRAPH_CONTEXT_H