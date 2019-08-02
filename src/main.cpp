#include <cstdlib>
#include <cstdio>
#include <vector>
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"
#include "layers.h"
#include "utils.h"
#include "instance.h"

bool fillPresent(const VkCommandBuffer commandBuffer, const VkImage currentSwapchainImage, const float clearColorR, const float clearColorG, const float clearColorB)
{
    VkResult result;

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext = 0;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo = 0;

    result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to start command buffer. [%s]\n", VkResultToStr(result));
        return false;
    }
    VkImageMemoryBarrier memBarrier = {};
    memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memBarrier.pNext = 0;
    memBarrier.srcAccessMask = 0;
    memBarrier.dstAccessMask = 0;
    memBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    memBarrier.subresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};
    memBarrier.image = currentSwapchainImage;

    memBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    memBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    memBarrier.srcAccessMask = 0;
    memBarrier.dstAccessMask = 0;

    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                        0, 0, 0, 0, 0, 1, &memBarrier);

    VkClearColorValue clearColorValue;
    VkImageSubresourceRange imageSubresourceRange = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

    clearColorValue.float32[0] = clearColorR;
    clearColorValue.float32[1] = clearColorG;
    clearColorValue.float32[2] = clearColorB;
    clearColorValue.float32[3] = 1.0f;
    vkCmdClearColorImage(commandBuffer, currentSwapchainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColorValue, 1, &imageSubresourceRange);

    memBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    memBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    memBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    memBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                        0, 0, 0, 0, 0, 1, &memBarrier);

    result = vkEndCommandBuffer(commandBuffer);
    return true;
}

bool render(const VkDevice device,
            const VkQueue queue,
            const VkSwapchainKHR swapchain,
            const VkCommandBuffer presentCmdBuffer,
            const std::vector<VkImage> & swapchainImagesVector,
            const float clearColorR, const float clearColorG, const float clearColorB)
{
    VkResult result;
    VkSemaphore imageAcquiredSemaphore, renderingCompleteSemaphore;

    if (createSemaphore(device, &imageAcquiredSemaphore) != VK_SUCCESS) {
        dprintf(2, "Failed to create semaphore.\n");
        return false;
    }
    if (createSemaphore(device, &renderingCompleteSemaphore) != VK_SUCCESS) {
        dprintf(2, "Failed to create semaphore.\n");
        return false;
    }
    uint32_t imageIndex = UINT32_MAX;

    result = vkAcquireNextImageKHR(device, swapchain, UINT64_MAX, imageAcquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        dprintf(2, "We don't support yet out-of-date swapchains.\n");
        return false;
    } else if (result == VK_SUBOPTIMAL_KHR) {
        printf("suboptimal.\n");
    } else if (result != VK_SUCCESS) {
        dprintf(2, "Couldn't acquire next image.\n");
        return false;
    }

    if (!fillPresent(presentCmdBuffer, swapchainImagesVector[imageIndex], clearColorR, clearColorG, clearColorB)) {
        dprintf(2, "Failed to fill presentation command buffer.\n");
        return false;
    }

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = 0;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &imageAcquiredSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &presentCmdBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &renderingCompleteSemaphore;

    result = vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to submit work to queue. [%s]\n", VkResultToStr(result));
        return false;
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = 0;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderingCompleteSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;
    presentInfo.pResults = 0;

    result = vkQueuePresentKHR(queue, &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        dprintf(2, "We don't support yet out-of-date swapchains.\n");
        return false;
    } else if (result == VK_SUBOPTIMAL_KHR) {
        printf("suboptimal2.\n");
    } else if (result != VK_SUCCESS) {
        dprintf(2, "Couldn't present queue.\n");
        return false;
    }

    vkQueueWaitIdle(queue);
    vkDestroySemaphore(device, imageAcquiredSemaphore, 0);
    vkDestroySemaphore(device, renderingCompleteSemaphore, 0);
    return true;
}

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

    createInstance(&instance, GetRequiredExtensions(), debugLayers);
    setupDebugMessenger(instance, &debugMessenger);

    glfwGetWindow("FreeFEM++", 1280, 769, &window);

    VkSurfaceKHR surface;
    createSurfaceKHR(window, instance, &surface);

    chooseVkPhysicalDevice(instance, &physicalDevice, surface);

    createDeviceAndQueue(physicalDevice, surface, debugLayers, &device, &queue, &queueFamily);

    VkSwapchainKHR swapchain;
    VkFormat surfaceFormat;
    if (!createSwapchain(physicalDevice, device, surface, window, VK_NULL_HANDLE, &swapchain, 1, &surfaceFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT))
        return EXIT_FAILURE;

	std::vector<VkImage> swapchainImagesVector;
	std::vector<VkImageView> swapchainImageViewsVector;
    getSwapchainImagesAndViews(device, swapchain, &surfaceFormat, swapchainImagesVector, swapchainImageViewsVector);

    VkCommandPool commandPool;
    createCommandPool(device, queueFamily, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, &commandPool);

    VkCommandBuffer cmdBufferInit;
    allocateCommandBuffer(device, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &cmdBufferInit);

    VkCommandBuffer cmdBufferPresent;
    allocateCommandBuffer(device, commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &cmdBufferPresent);

    long frameNumber = 0;
    constexpr int MAX_COLORS = 4;
    constexpr int FRAMES_PER_COLOR = 120;
	static float screenColors[MAX_COLORS][3] = {
	    {1.0f, 0.2f, 0.2f},
	    {0.0f, 0.9f, 0.2f},
	    {0.0f, 0.2f, 1.0f},
	    {1.0f, 0.9f, 0.2f},
	};

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

		float colR = screenColors[(frameNumber/FRAMES_PER_COLOR) % MAX_COLORS][0];
		float colG = screenColors[(frameNumber/FRAMES_PER_COLOR) % MAX_COLORS][1];
		float colB = screenColors[(frameNumber/FRAMES_PER_COLOR) % MAX_COLORS][2];

        if (!render(device, queue, swapchain, cmdBufferPresent, swapchainImagesVector, colR, colG, colB))
            glfwSetWindowShouldClose(window, true);
        frameNumber += 1;
    }
    return EXIT_SUCCESS;
}