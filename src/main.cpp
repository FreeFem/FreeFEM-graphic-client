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
            const VkCommandBuffer presentCmdBuffer, const std::vector<VkFramebuffer> &frameBuffers,
            const VkRenderPass renderPass, const VkPipeline pipeline, const VkBuffer vertexBuffer,
            const uint32_t vertexInputBinding, const int width, const int height)
{
    VkResult result;
    VkSemaphore imageAcquiredSemaphore, renderingCompletedSemaphore;

    result = createSemaphore(device, imageAcquiredSemaphore);
    if (result != VK_SUCCESS) return false;
    result = createSemaphore(device, renderingCompletedSemaphore);
    if (result != VK_SUCCESS) return false;

    uint32_t imageIndex = UINT32_MAX;
    result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        dprintf(2, "we don't support out-of-date swapchain yet.\n");
        return false;
    } else if (result == VK_SUBOPTIMAL_KHR)
        dprintf(1, "Swapchain is suboptimal.\n");
    else if (result != VK_SUCCESS)
        return false;

    if (!fillRenderingCmdBuffer(presentCmdBuffer, frameBuffers[imageIndex], renderPass, pipeline, vertexBuffer, vertexInputBinding, width, height))
        return false;

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAcquiredSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &presentCmdBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderingCompletedSemaphore;

    result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (result != VK_SUCCESS) return false;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderingCompletedSemaphore;
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

    vkQueueWaitIdle(queue);

    vkDestroySemaphore(device, imageAcquiredSemaphore, 0);
    vkDestroySemaphore(device, renderingCompletedSemaphore, 0);
    return true;

}

#include <fstream>

int main(int ac, char *av[])
{
    if (glfwInit() != GLFW_TRUE) {
        dprintf(2, "%s: Failed to init GLFW.\n", av[0]);
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    GLFWwindow *window;
    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue queue;
    uint32_t queueFamily;

    createInstance(instance, GetRequiredExtensions(), debugLayers);
    setupDebugMessenger(instance, &debugMessenger);

    glfwGetWindow("FreeFEM++", 1280, 769, window);

    VkSurfaceKHR surface;
    createSurfaceKHR(window, instance, surface);

    chooseVkPhysicalDevice(instance, physicalDevice, surface);

    createDeviceAndQueue(physicalDevice, surface, debugLayers, device, queue, queueFamily);

    VkSwapchainKHR swapchain;
    VkFormat surfaceFormat;
    if (!createSwapchain(physicalDevice, device, surface, window, VK_NULL_HANDLE, swapchain, 1, surfaceFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT)) {
        return EXIT_FAILURE;
    }

	std::vector<VkImage> swapchainImagesVector;
	std::vector<VkImageView> swapchainImageViewsVector;
    getSwapchainImagesAndViews(device, swapchain, &surfaceFormat, swapchainImagesVector, swapchainImageViewsVector);

    VkCommandPool commandPool;
    createCommandPool(device, queueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, commandPool);

    VkCommandBuffer cmdBufferInit;
    allocateCommandBuffer(device, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, cmdBufferInit);

    VkCommandBuffer cmdBufferPresent;
    allocateCommandBuffer(device, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, cmdBufferPresent);

    VkPhysicalDeviceMemoryProperties memoryProps;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProps);

    const VkFormat depthBufferFormat = VK_FORMAT_D16_UNORM;

    VkImage depthImage;
    VkImageView depthImageView;
    VkDeviceMemory depthMemory;
    createAndAllocateImage(device,
                           memoryProps,
                           VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                           0,
                           depthBufferFormat,
                           1280, 769,
                           depthImage,
                           depthMemory,
                           &depthImageView,
                           VK_IMAGE_ASPECT_DEPTH_BIT);

    VkRenderPass renderPass;
    createRenderPass(device, surfaceFormat, depthBufferFormat, renderPass);

    std::vector<VkFramebuffer> framebufferVector;
    framebufferVector.reserve(swapchainImageViewsVector.size());

    for (const auto view : swapchainImageViewsVector) {
        VkFramebuffer fb;
        createFramebuffer(device, renderPass, {view, depthImageView}, 1280, 769, fb);
        framebufferVector.push_back(fb);
    }

    const size_t vertexBufferSize = sizeof(TriangleDemoVertex) * NUM_DEMO_VERTICES;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMem;
    createAndAllocateBuffer(device, memoryProps,
                            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
                            vertexBufferSize,
                            vertexBuffer,
                            vertexBufferMem);
    {
        void *mappedBuffer;
        vkMapMemory(device, vertexBufferMem, 0, VK_WHOLE_SIZE, 0, &mappedBuffer);

        memcpy(mappedBuffer, vertices, vertexBufferSize);
        vkUnmapMemory(device, vertexBufferMem);
    }

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = 0;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = 0;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = 0;

    VkPipelineLayout pipelineLayout;
    vkCreatePipelineLayout(device, &pipelineLayoutCreateInfo, 0, &pipelineLayout);

    VkPipeline graphicPipeline;
    if (!createPipeline(device, renderPass, pipelineLayout, VERTEX_SHADER_FILENAME, FRAGMENT_SHADER_FILENAME, VERTEX_INPUT_BINDING, graphicPipeline)) {
        dprintf(2, "Pipeline call failed.\n");
        return EXIT_FAILURE;
    }

    fillInitCmdBuffer(cmdBufferInit, depthImage);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBufferInit;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);

    vkQueueWaitIdle(queue);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (!render(device, queue, swapchain, cmdBufferPresent, framebufferVector, renderPass, graphicPipeline, vertexBuffer, VERTEX_INPUT_BINDING, 1280, 769))
            break;
    }

    return EXIT_SUCCESS;
}