#include <cstring>
#include <vector>
#include "Renderer.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static bool pushDepthImage(const Device& D, const Image DepthImage, const VkCommandPool& Pool) {
    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandPool = Pool;
    AllocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    if (vkAllocateCommandBuffers(D.Handle, &AllocInfo, &cmdBuffer)) return false;
    VkImageMemoryBarrier memBarrier = {};
    memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
    memBarrier.image = DepthImage.Handle;

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(cmdBuffer, &BeginInfo)) return false;
    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, 0, 0, 0, 0, 0,
                         1, &memBarrier);

    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    vkQueueSubmit(D.Queue[DEVICE_GRAPH_QUEUE], 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(D.Queue[DEVICE_GRAPH_QUEUE]);

    vkFreeCommandBuffers(D.Handle, Pool, 1, &cmdBuffer);
    return true;
}

static bool pushColorImage(const Device& D, const Image ColorImage, const VkCommandPool& Pool) {
    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandPool = Pool;
    AllocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    if (vkAllocateCommandBuffers(D.Handle, &AllocInfo, &cmdBuffer)) return false;
    VkImageMemoryBarrier memBarrier = {};
    memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    memBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    memBarrier.image = ColorImage.Handle;

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(cmdBuffer, &BeginInfo)) return false;
    vkCmdPipelineBarrier(cmdBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, 0,
                         0, 0, 0, 0, 1, &memBarrier);

    vkEndCommandBuffer(cmdBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffer;

    vkQueueSubmit(D.Queue[DEVICE_GRAPH_QUEUE], 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(D.Queue[DEVICE_GRAPH_QUEUE]);

    vkFreeCommandBuffers(D.Handle, Pool, 1, &cmdBuffer);
    return true;
}

bool pushInitCmdBuffer(const Device& D, const Image DepthImage, const Image& ColorImage, const VkCommandPool& Pool) {
    if (!pushDepthImage(D, DepthImage, Pool)) return false;
    if (!pushColorImage(D, ColorImage, Pool)) return false;
    return true;
}

static std::vector<VkCommandBuffer> newCommandBuffers(const VkDevice Device, const VkCommandPool CommandPool,
                                                      uint32_t Count, const VkCommandBufferLevel CmdBufferLevels) {
    VkResult res;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = CommandPool;
    allocInfo.level = CmdBufferLevels;
    allocInfo.commandBufferCount = Count;

    std::vector<VkCommandBuffer> Buffers(Count);
    if (vkAllocateCommandBuffers(Device, &allocInfo, Buffers.data( ))) return {};
    return Buffers;
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

Renderer NewRenderer(const VkDevice& Device, VkQueue* Queue, uint32_t QueueIndex, const VkSurfaceKHR& Surface,
                     VkExtent2D Extent) {
    Renderer n;
    memset(&n, 0, sizeof(Renderer));

    n.Queue_REF = Queue;
    VkCommandPoolCreateInfo CmdPoolCreateInfos = {};
    CmdPoolCreateInfos.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CmdPoolCreateInfos.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CmdPoolCreateInfos.queueFamilyIndex = QueueIndex;

    if (vkCreateCommandPool(Device, &CmdPoolCreateInfos, 0, &n.CommandPool)) {
        LogError(GetCurrentLogLocation( ), "Failed to create VkCommandPool.");
        return n;
    }
    for (uint i = 0; i < 2; ++i) {
        n.Frames[i].PresentFence = newFence(Device);
        n.Frames[i].Render = newSemaphore(Device);
        n.Frames[i].Acquire = newSemaphore(Device);
        n.Frames[i].CmdBuffer = newCommandBuffers(Device, n.CommandPool, 1, VK_COMMAND_BUFFER_LEVEL_PRIMARY)[0];
        if (n.Frames[i].PresentFence == VK_NULL_HANDLE || n.Frames[i].Render == VK_NULL_HANDLE ||
            n.Frames[i].Acquire == VK_NULL_HANDLE || n.Frames[i].CmdBuffer == VK_NULL_HANDLE)
            LogError(GetCurrentLogLocation( ), "Failed to create VkFence or VkSemaphore or VkCommandBuffer.");
    }
    return n;
}

void DestroyRenderer(const VkDevice& Device, Renderer& vkRenderer) {
    for (uint i = 0; i < 2; ++i) {
        vkDestroyFence(Device, vkRenderer.Frames[i].PresentFence, 0);
        vkDestroySemaphore(Device, vkRenderer.Frames[i].Acquire, 0);
        vkDestroySemaphore(Device, vkRenderer.Frames[i].Render, 0);
    }
    vkDestroyCommandPool(Device, vkRenderer.CommandPool, 0);
}

void Render(const Context& vkContext, const VkRenderPass RenderPass, std::vector<VkFramebuffer> Framebuffers,
            Renderer& vkRenderer, const RenderGraph& Graph, const VkExtent2D Extent) {
    if (Graph.Nodes.empty( )) return;
    PerFrame& currentFrame = vkRenderer.Frames[vkRenderer.frame];
    if (currentFrame.FenceInitialized) {
        vkWaitForFences(vkContext.vkDevice.Handle, 1, &currentFrame.PresentFence, VK_TRUE, UINT64_MAX);
        vkResetFences(vkContext.vkDevice.Handle, 1, &currentFrame.PresentFence);
    }

    VkResult res;
    uint32_t imageIndex = UINT32_MAX;

    res = vkAcquireNextImageKHR(vkContext.vkDevice.Handle, vkContext.vkSwapchain.Handle, UINT64_MAX,
                                currentFrame.Acquire, VK_NULL_HANDLE, &imageIndex);

    currentFrame.FenceInitialized = true;
    if (res != VK_SUCCESS) return;

    VkCommandBufferBeginInfo CmdBufferBeginInfo = {};
    CmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(currentFrame.CmdBuffer, &CmdBufferBeginInfo)) return;

    VkClearValue clearValues[3];
    clearValues[0].color.float32[0] = 1.0f;
    clearValues[0].color.float32[1] = 1.0f;
    clearValues[0].color.float32[2] = 1.0f;
    clearValues[0].color.float32[3] = 1.0f;

    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0.0f;

    clearValues[2].color.float32[0] = 1.0f;
    clearValues[2].color.float32[1] = 1.0f;
    clearValues[2].color.float32[2] = 1.0f;
    clearValues[2].color.float32[3] = 1.0f;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = RenderPass;
    renderPassBeginInfo.framebuffer = Framebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = {Extent.width, Extent.height};
    renderPassBeginInfo.clearValueCount = 3;
    renderPassBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(currentFrame.CmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    for (auto& Node : Graph.Nodes) {
        vkCmdBindPipeline(currentFrame.CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Node.Handle);

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)Extent.width;
        viewport.height = (float)Extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(currentFrame.CmdBuffer, 0, 1, &viewport);

        VkRect2D scissor = {};

        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = Extent.width;
        scissor.extent.height = Extent.height;
        vkCmdSetScissor(currentFrame.CmdBuffer, 0, 1, &scissor);

        vkCmdPushConstants(currentFrame.CmdBuffer, Node.Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CameraUniform),
                           &Graph.PushCamera);
        VkDeviceSize Offset = 0;
        for (uint32_t i = 0; i < Node.GPUBuffers.size( ); ++i) {
            vkCmdBindVertexBuffers(currentFrame.CmdBuffer, i, 1, &Node.GPUBuffers[i], &Offset);
        }
        uint32_t VerticesCount = 0;
        for (uint32_t i = 0; i < Node.GPUBuffers.size( ); ++i) {
            VerticesCount += Node.Meshes[i].CPUBuffer.ElementCount;
        }
        vkCmdDraw(currentFrame.CmdBuffer, VerticesCount, 1, 0, 0);
    }

    vkCmdEndRenderPass(currentFrame.CmdBuffer);

    res = vkEndCommandBuffer(currentFrame.CmdBuffer);

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &currentFrame.Acquire;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentFrame.CmdBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &currentFrame.Render;

    res = vkQueueSubmit(vkContext.vkDevice.Queue[DEVICE_GRAPH_QUEUE], 1, &submitInfo, currentFrame.PresentFence);
    if (res != VK_SUCCESS) return;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &currentFrame.Render;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkContext.vkSwapchain.Handle;
    presentInfo.pImageIndices = &imageIndex;

    res = vkQueuePresentKHR(vkContext.vkDevice.Queue[DEVICE_GRAPH_QUEUE], &presentInfo);
    if (res != VK_SUCCESS) return;
}

}    // namespace Vulkan
}    // namespace ffGraph