#include <cstdlib>
#include <cstdio>
#include <vector>
#include <chrono>
#include <climits>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "layers.h"
#include "utils.h"
#include "instance.h"
#include "test_data.h"
#include "doublebuffer.h"

bool fillInitCmdBuffer(const VkCommandBuffer commandBuffer, const VkImage depthImage)
{
    VkResult result;
    std::vector<VkImageMemoryBarrier> memBarriers;

    {
        VkImageMemoryBarrier imageMemBarrier = {};
        imageMemBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        imageMemBarrier.srcAccessMask = 0;
        imageMemBarrier.dstAccessMask = 0;
        imageMemBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageMemBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        imageMemBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        imageMemBarrier.subresourceRange = {VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1};
        imageMemBarrier.image = depthImage;

        memBarriers.push_back(imageMemBarrier);
    }

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    result = vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to begin command buffer. [%s]\n", VkResultToStr(result));
        return false;
    }

    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         0,
                         0,
                         0,
                         0,
                         0,
                         (uint32_t)memBarriers.size(),
                         memBarriers.data());

    result = vkEndCommandBuffer(commandBuffer);
    return true;
}

bool fillRenderingCmdBuffer(const VkCommandBuffer commandBuffer,
                            const VkFramebuffer currentFramebuffer,
                            const VkRenderPass renderPass,
                            const VkPipeline pipeline,
                            const VkBuffer vertexBuffer,
                            const uint32_t vertexInputBinding,
                            const int width,
                            const int height)
{
    VkResult result;

    VkCommandBufferBeginInfo commandbufferBeginInfo = {};
    commandbufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandbufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    result = vkBeginCommandBuffer(commandBuffer, &commandbufferBeginInfo);
    if (result != VK_SUCCESS) return false;

    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = 0.0f;
    clearValues[0].color.float32[1] = 0.0f;
    clearValues[0].color.float32[2] = 0.0f;
    clearValues[0].color.float32[3] = 1.0f;
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0.0f;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = renderPass;
    renderPassBeginInfo.framebuffer = currentFramebuffer;
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = {(uint32_t)width, (uint32_t)height};
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)width;
    viewport.height = (float)height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = (uint32_t)width;
    scissor.extent.height = (uint32_t)height;

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkDeviceSize bufferOffsets = 0;
    vkCmdBindVertexBuffers(commandBuffer, vertexInputBinding, 1, &vertexBuffer, &bufferOffsets);

    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    result = vkEndCommandBuffer(commandBuffer);
    return true;
}


bool render(const VkDevice device, const VkQueue queue, const VkSwapchainKHR swapchain,
            const std::vector<VkFramebuffer> &frameBuffers, const VkRenderPass renderPass,
            const VkPipeline pipeline, const VkBuffer vertexBuffer, const uint32_t vertexInputBinding,
            perFrameData& currentFrameData, const int width, const int height)
{
    VkResult result;

    if (currentFrameData.fenceInitialized) {
        vkWaitForFences(device, 1, &currentFrameData.presentFence, VK_TRUE, UINT64_MAX);
        vkResetFences(device, 1, &currentFrameData.presentFence);
    }
    uint32_t imageIndex = UINT32_MAX;
    result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, currentFrameData.imageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

    currentFrameData.fenceInitialized = true;

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        dprintf(2, "we don't support out-of-date swapchain yet.\n");
        return false;
    } else if (result == VK_SUBOPTIMAL_KHR)
        dprintf(1, "Swapchain is suboptimal.\n");
    else if (result != VK_SUCCESS)
        return false;

    if (!fillRenderingCmdBuffer(currentFrameData.presentCmdBuffer, frameBuffers[imageIndex], renderPass, pipeline, vertexBuffer, vertexInputBinding, width, height))
        return false;

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &currentFrameData.imageAcquiredSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentFrameData.presentCmdBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &currentFrameData.renderingCompletedSemaphore;

    result = vkQueueSubmit(queue, 1, &submitInfo, currentFrameData.presentFence);
    if (result != VK_SUCCESS) return false;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &currentFrameData.renderingCompletedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;

    result = vkQueuePresentKHR(queue, &presentInfo);


    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        dprintf(2, "we don't support out-of-date swapchain yet.\n");
        return false;
    } else if (result == VK_SUBOPTIMAL_KHR)
        dprintf(1, "Swapchain is suboptimal.\n");
    else if (result != VK_SUCCESS)
        return false;
    return true;
}

#include "vkContext.hpp"

int main()
{
    vkContext c;
    vkContextInitInfo init = {1280, 769, "NAME"};

    if (c.init(init) != ReturnError::NONE)
        return EXIT_FAILURE;

    while (!glfwWindowShouldClose(c.window)) {
        glfwPollEvents();
    }
    return EXIT_SUCCESS;
}