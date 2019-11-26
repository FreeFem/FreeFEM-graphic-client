#include <imgui.h>
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

    if (!RenderGraph.Pipelines.empty()) {
        for (size_t i = 0; i < RenderGraph.RenderedGeometries.size(); ++i) {
            const Pipeline p = RenderGraph.Pipelines[RenderGraph.Geometries[RenderGraph.RenderedGeometries[i]].Geo.Description.PipelineID];
            vkCmdBindPipeline(CurrentFrame.CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, p.Handle);

            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)m_Window.WindowSize.width;
            viewport.height = (float)m_Window.WindowSize.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            vkCmdSetViewport(CurrentFrame.CmdBuffer, 0, 1, &viewport);

            RenderGraph.CamUniform.ViewProj = RenderGraph.Cam.Handle.ViewProjMatrix;
            vkCmdPushConstants(CurrentFrame.CmdBuffer, p.Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(CameraUniform),
                           &RenderGraph.CamUniform);

            VkRect2D scissor = {};

            scissor.offset.x = 0;
            scissor.offset.y = 0;
            scissor.extent.width = m_Window.WindowSize.width;
            scissor.extent.height = m_Window.WindowSize.height;
            vkCmdSetScissor(CurrentFrame.CmdBuffer, 0, 1, &scissor);

            vkCmdBindVertexBuffers(CurrentFrame.CmdBuffer, 0, 1, &RenderGraph.RenderBuffer.Handle,
                    &RenderGraph.Geometries[RenderGraph.RenderedGeometries[i]].Geo.BufferOffset);

            vkCmdDraw(CurrentFrame.CmdBuffer, RenderGraph.Geometries[RenderGraph.RenderedGeometries[i]].Geo.count(), 1, 0, 0);
        }
    }

    renderUI( );

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

void Instance::renderUI( ) {
    PerFrame& CurrentFrame = FrameData[CurrentFrameData];

    vkCmdBindDescriptorSets(CurrentFrame.CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Ui.Layout, 0, 1,
                            &Ui.DescriptorSet, 0, 0);
    vkCmdBindPipeline(CurrentFrame.CmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Ui.Handle);

    Ui.ImGuiPushConstant.Scale =
        glm::vec2(2.0f / (float)m_Window.WindowSize.width, 2.0f / (float)m_Window.WindowSize.height);
    Ui.ImGuiPushConstant.Translate = glm::vec2(-1.f);

    vkCmdPushConstants(CurrentFrame.CmdBuffer, Ui.Layout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(UiPipeline::ImGuiPushConst), &Ui.ImGuiPushConstant);

    ImDrawData* imDraw = ImGui::GetDrawData( );
    int32_t vertexOffset = 0;
    uint32_t indexOffset = 0;

    if (imDraw == 0)
        return;
    if (imDraw->CmdListsCount > 0) {
        VkDeviceSize offset = {0};

        vkCmdBindVertexBuffers(CurrentFrame.CmdBuffer, 0, 1, &Ui.ImGuiVertices.Handle, &offset);
        vkCmdBindIndexBuffer(CurrentFrame.CmdBuffer, Ui.ImGuiIndices.Handle, 0, VK_INDEX_TYPE_UINT16);
        for (uint32_t i = 0; i < imDraw->CmdListsCount; ++i) {
            const ImDrawList* cmd_list = imDraw->CmdLists[i];
            for (uint32_t j = 0; j < cmd_list->CmdBuffer.Size; ++j) {
                const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];
                VkRect2D scissorRect;
                scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
                scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
                scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
                scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
                vkCmdSetScissor(CurrentFrame.CmdBuffer, 0, 1, &scissorRect);

                VkViewport viewport = {};
                viewport.x = 0.0f;
                viewport.y = 0.0f;
                viewport.width = (float)m_Window.WindowSize.width;
                viewport.height = (float)m_Window.WindowSize.height;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;

                vkCmdSetViewport(CurrentFrame.CmdBuffer, 0, 1, &viewport);

                vkCmdDrawIndexed(CurrentFrame.CmdBuffer, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
                indexOffset += pcmd->ElemCount;
            }
            vertexOffset += cmd_list->VtxBuffer.Size;
        }
    }
}

}    // namespace Vulkan
}    // namespace ffGraph