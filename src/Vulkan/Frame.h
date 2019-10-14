#ifndef FRAME_H_
#define FRAME_H_

#include <vulkan/vulkan.h>

namespace ffGraph {
namespace Vulkan {

struct PerFrame {
    bool FenceInitialized = false;
    VkFence PresentFence;
    VkSemaphore AcquireSemaphore;
    VkSemaphore RenderSemaphore;
    VkCommandBuffer CmdBuffer;
};

bool CreatePerFrameData(const VkDevice& Device, const VkCommandPool& CommandPool, uint32_t Count, PerFrame* FrameData);
void DestroyPerFrameData(const VkDevice& Device, const VkCommandPool& CommandPool, uint32_t Count, PerFrame* FrameData);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // FRAME_H_