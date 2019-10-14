#include "Frame.h"

namespace ffGraph {
namespace Vulkan {

static bool newCommandBuffers(const VkDevice Device, const VkCommandPool CommandPool, uint32_t Count,
                              VkCommandBuffer* CmdBuffers, const VkCommandBufferLevel CmdBufferLevels) {
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = CommandPool;
    allocInfo.level = CmdBufferLevels;
    allocInfo.commandBufferCount = Count;

    if (vkAllocateCommandBuffers(Device, &allocInfo, CmdBuffers)) return false;
    return true;
}

static VkFence newFence(const VkDevice& Device) {
    VkFenceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkFence Fence;
    if (vkCreateFence(Device, &createInfo, 0, &Fence) != VK_SUCCESS) return VK_NULL_HANDLE;
    return Fence;
}

static VkSemaphore newSemaphore(const VkDevice Device) {
    VkSemaphoreCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore Semaphore = VK_NULL_HANDLE;
    if (vkCreateSemaphore(Device, &createInfo, 0, &Semaphore)) return VK_NULL_HANDLE;
    return Semaphore;
}

bool CreatePerFrameData(const VkDevice& Device, const VkCommandPool& CommandPool, uint32_t Count, PerFrame* FrameData) {
    for (uint32_t i = 0; i < Count; ++i) {
        FrameData[i].PresentFence = newFence(Device);
        FrameData[i].AcquireSemaphore = newSemaphore(Device);
        FrameData[i].RenderSemaphore = newSemaphore(Device);
        if (newCommandBuffers(Device, CommandPool, 1, &FrameData[i].CmdBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY) ==
            false)
            return false;
        if (FrameData[i].PresentFence == VK_NULL_HANDLE || FrameData[i].AcquireSemaphore == VK_NULL_HANDLE ||
            FrameData[i].RenderSemaphore == VK_NULL_HANDLE)
            return false;
    }
    return true;
}

void DestroyPerFrameData(const VkDevice& Device, const VkCommandPool& CommandPool, uint32_t Count,
                         PerFrame* FrameData) {
    for (uint32_t i = 0; i < Count; ++i) {
        vkDestroyFence(Device, FrameData[i].PresentFence, 0);
        vkDestroySemaphore(Device, FrameData[i].AcquireSemaphore, 0);
        vkDestroySemaphore(Device, FrameData[i].RenderSemaphore, 0);
        vkFreeCommandBuffers(Device, CommandPool, 1, &FrameData[i].CmdBuffer);
    }
}

}    // namespace Vulkan
}    // namespace ffGraph