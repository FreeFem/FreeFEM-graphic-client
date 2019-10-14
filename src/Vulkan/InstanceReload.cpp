#include "Instance.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static void cleanup_for_reload(Instance* Handle) {
    Environment& Env = Handle->Env;
    DestroyImage(Env.Allocator, Env.GPUInfos.Device, Env.GraphManager.DepthImage);
    DestroyImage(Env.Allocator, Env.GPUInfos.Device, Env.GraphManager.ColorImage);

    for (auto& framebuffer : Env.GraphManager.Framebuffers) {
        vkDestroyFramebuffer(Env.GPUInfos.Device, framebuffer, 0);
    }
    Env.GraphManager.Framebuffers.clear( );

    for (size_t i = 0; i < Handle->Graphs.size( ); ++i) {
        for (size_t j = 0; j < Handle->Graphs[i].Nodes.size( ); ++j) {
            vkDestroyPipelineLayout(Env.GPUInfos.Device, Handle->Graphs[i].Nodes[j].Layout, 0);
            vkDestroyPipeline(Env.GPUInfos.Device, Handle->Graphs[i].Nodes[j].Handle, 0);
            Handle->Graphs[i].Nodes[j].Layout = VK_NULL_HANDLE;
            Handle->Graphs[i].Nodes[j].Handle = VK_NULL_HANDLE;
        }
    }

    vkDestroyRenderPass(Env.GPUInfos.Device, Env.GraphManager.RenderPass, 0);

    for (auto& view : Env.ScreenInfos.Views) {
        vkDestroyImageView(Env.GPUInfos.Device, view, 0);
    }
    Env.ScreenInfos.Views.clear( );

    vkDestroySwapchainKHR(Env.GPUInfos.Device, Env.ScreenInfos.Swapchain, 0);
    Env.ScreenInfos.Images.clear( );
}

void Instance::reload( ) {
    vkDeviceWaitIdle(Env.GPUInfos.Device);

    cleanup_for_reload(this);

    CreateScreenInfos(Env, m_Window);
    CreateGraphicInformations(Env.GraphManager, Env, m_Window);
    RenderGraphCreateInfos CreateInfo = {Env.GPUInfos.Device, Env.GraphManager.RenderPass,
                                         Env.GPUInfos.Capabilities.msaaSamples};
    for (auto& Graph : Graphs) {
        ReloadRenderGraph(CreateInfo, Graph);
    }
}

void Instance::render( ) {
    PerFrame& CurrentFrame = FrameData[CurrentFrameData];
    if (CurrentFrame.FenceInitialized) {
        vkWaitForFences(Env.GPUInfos.Device, 1, &CurrentFrame.PresentFence, VK_TRUE, UINT64_MAX);
        vkResetFences(Env.GPUInfos.Device, 1, &CurrentFrame.PresentFence);
    }

    uint32_t imageIndex = UINT32_MAX;
    VkResult res;

    res = vkAcquireNextImageKHR(Env.GPUInfos.Device, Env.ScreenInfos.Swapchain, UINT64_MAX,
                                CurrentFrame.AcquireSemaphore, VK_NULL_HANDLE, &imageIndex);
    CurrentFrame.FenceInitialized = true;

    if (res != VK_SUCCESS) return;

    VkCommandBufferBeginInfo CmdBufferBeginInfo = {};
    CmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    CmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    if (vkBeginCommandBuffer(CurrentFrame.CmdBuffer, &CmdBufferBeginInfo)) return;

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
    renderPassBeginInfo.renderPass = Env.GraphManager.RenderPass;
    renderPassBeginInfo.framebuffer = Env.GraphManager.Framebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = m_Window.WindowSize;
    renderPassBeginInfo.clearValueCount = 3;
    renderPassBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(CurrentFrame.CmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    for (auto& Node : Graphs[CurrentRenderGraph].Nodes) {
        if (!Node.to_render) continue;
        memcpy(Graphs[CurrentRenderGraph].PushBuffer.Infos.pMappedData, Node.CPUMeshData.BatchedMeshes.Data,
               Node.CPUMeshData.BatchedMeshes.ElementCount * Node.CPUMeshData.BatchedMeshes.ElementSize);

        vkCmdBindPipeline(CurrentFrame.CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Node.Handle);

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)m_Window.WindowSize.width;
        viewport.height = (float)m_Window.WindowSize.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(CurrentFrame.CmdBuffer, 0, 1, &viewport);

        VkRect2D scissor = {};

        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = m_Window.WindowSize.width;
        scissor.extent.height = m_Window.WindowSize.height;
        vkCmdSetScissor(CurrentFrame.CmdBuffer, 0, 1, &scissor);

        vkCmdPushConstants(CurrentFrame.CmdBuffer, Node.Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CameraUniform),
                           &Graphs[CurrentRenderGraph].PushCamera);
        std::vector<VkDeviceSize> Offsets = {};
        for (uint32_t i = 0; i < Node.CPUMeshData.Layouts.size( ); ++i)
            Offsets.push_back(Node.CPUMeshData.Layouts[i].offset);
        vkCmdBindVertexBuffers(CurrentFrame.CmdBuffer, 0, 1, &Graphs[CurrentRenderGraph].PushBuffer.Handle,
                               Offsets.data( ));

        uint32_t VerticesCount = Node.CPUMeshData.BatchedMeshes.ElementCount;
        vkCmdDraw(CurrentFrame.CmdBuffer, VerticesCount, 1, 0, 0);
    }

    vkCmdEndRenderPass(CurrentFrame.CmdBuffer);

    res = vkEndCommandBuffer(CurrentFrame.CmdBuffer);

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &CurrentFrame.AcquireSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &CurrentFrame.CmdBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &CurrentFrame.RenderSemaphore;

    res = vkQueueSubmit(Env.GPUInfos.Queues[Env.GPUInfos.GraphicQueueIndex], 1, &submitInfo, CurrentFrame.PresentFence);
    if (res != VK_SUCCESS) return;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &CurrentFrame.RenderSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &Env.ScreenInfos.Swapchain;
    presentInfo.pImageIndices = &imageIndex;

    res = vkQueuePresentKHR(Env.GPUInfos.Queues[Env.GPUInfos.PresentQueueIndex], &presentInfo);
    if (res != VK_SUCCESS) return;
}

}    // namespace Vulkan
}    // namespace ffGraph