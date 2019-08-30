#include "GraphContext.h"
#include "../util/Logger.h"
#include "Buffers.h"
#include "Camera.h"
#include "GraphManager.h"
#include "vkcommon.h"

namespace FEM {
namespace gr {
static bool allocateCommandBuffer(const VkDevice device, const VkCommandPool commandPool,
                                  const VkCommandBufferLevel commandBufferLevel, VkCommandBuffer& outCommandBuffer) {
    VkResult res;

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.pNext = 0;
    allocInfo.commandPool = commandPool;
    allocInfo.level = commandBufferLevel;
    allocInfo.commandBufferCount = 1;

    res = vkAllocateCommandBuffers(device, &allocInfo, &outCommandBuffer);
    if (res != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to allocate memory for VkCommandBuffer.");
        return ErrorValues::FUNCTION_FAILED;
    }
    return true;
}

ErrorValues Context::init(const Manager& grm) {
    if (initInternal(grm)) {
        return ErrorValues::FUNCTION_FAILED;
    }
    return ErrorValues::NONE;
}

ErrorValues Context::initInternal(const Manager& grm) {
    if (!allocateCommandBuffer(grm.Device, grm.CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                               InitializerCommandBuffer)) {
        LOGE(FILE_LOCATION( ), "Failed to allocate gr::Context VkCommandBuffer InitializerCommandBuffer.");
        return ErrorValues::FUNCTION_FAILED;
    }
    if (initSwapchain(grm)) {
        LOGE(FILE_LOCATION( ), "Failed to create gr::Context VkSwapchainKHR Swapchain.");
        return ErrorValues::FUNCTION_FAILED;
    }

    if (DepthImage.init(grm, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, DepthBufferFormat,
                        VK_IMAGE_ASPECT_DEPTH_BIT)) {
        LOGE(FILE_LOCATION( ), "Failed to create gr::Context Image DepthImage.");
        return ErrorValues::FUNCTION_FAILED;
    }
    if (fillInitCmdBuffer(grm)) {
        LOGE(FILE_LOCATION( ), "Failed to fill gr::Context VkCommandBuffer InitializerCommandBuffer.");
        return ErrorValues::FUNCTION_FAILED;
    }

    for (int i = 0; i < 2; i += 1) {
        allocateCommandBuffer(grm.Device, grm.CommandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, PresentCmdBuffer[i]);
        createFence(grm.Device, PerFrame[i].presentFence);
        createSemaphore(grm.Device, PerFrame[i].acquiredSemaphore);
        createSemaphore(grm.Device, PerFrame[i].renderCompletedSemaphore);
        PerFrame[i].fenceInitialized = false;
    }

    return ErrorValues::NONE;
}

ErrorValues Context::initSwapchain(const Manager& grm) {
    uint32_t surfaceFormatCount;

    VkSwapchainKHR oldSwapchain = (Swapchain) ? Swapchain : VK_NULL_HANDLE;

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(grm.PhysicalDevice, grm.Surface, &surfaceFormatCount, 0) != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to get surface format.");
        return ErrorValues::FUNCTION_FAILED;
    }
    if (surfaceFormatCount < 1) {
        LOGE(FILE_LOCATION( ), "Failed to get surface format.");
        return ErrorValues::FUNCTION_FAILED;
    }

    std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(grm.PhysicalDevice, grm.Surface, &surfaceFormatCount,
                                             surfaceFormats.data( )) != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to get surface format.");
        return ErrorValues::FUNCTION_FAILED;
    }

    VkFormat surfaceFormat;
    if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
        surfaceFormat = VK_FORMAT_B8G8R8_UNORM;
    else
        surfaceFormat = surfaceFormats[0].format;

    SurfaceFormat = surfaceFormat;

    VkColorSpaceKHR surfaceColorSpace = surfaceFormats[0].colorSpace;

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(grm.PhysicalDevice, grm.Surface, &surfaceCapabilities) !=
        VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to get surface capabilities.");
        return ErrorValues::FUNCTION_FAILED;
    }

    uint32_t presentModeCount = 0;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(grm.PhysicalDevice, grm.Surface, &presentModeCount, 0) !=
        VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to get surface present mode.");
        return ErrorValues::FUNCTION_FAILED;
    }
    if (presentModeCount < 1) {
        LOGE(FILE_LOCATION( ), "Failed to get surface present mode.");
        return ErrorValues::FUNCTION_FAILED;
    }

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(grm.PhysicalDevice, grm.Surface, &presentModeCount,
                                                  presentModes.data( )) != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to get surface present mode.");
        return ErrorValues::FUNCTION_FAILED;
    }

    VkExtent2D swapchainExtent = surfaceCapabilities.currentExtent;

    int windowWidth, windowHeight = 0;
    glfwGetWindowSize(grm.Window->getNativeWindow( ), &windowWidth, &windowHeight);
    if (swapchainExtent.width == (uint32_t)(-1)) {
        swapchainExtent.width = static_cast<uint32_t>(windowWidth);
        swapchainExtent.height = static_cast<uint32_t>(windowHeight);
    } else if (swapchainExtent.width != static_cast<uint32_t>(windowWidth) &&
               swapchainExtent.height != static_cast<uint32_t>(windowHeight)) {
        LOGE(FILE_LOCATION( ), "Swapchain extent doesn't fit with Window size.");
        return ErrorValues::FUNCTION_FAILED;
    }

    uint32_t desiredNbOfSwapchainImages = surfaceCapabilities.minImageCount + 1;

    if (surfaceCapabilities.maxImageCount > 0)
        desiredNbOfSwapchainImages = std::min(desiredNbOfSwapchainImages, surfaceCapabilities.maxImageCount);

    VkSurfaceTransformFlagBitsKHR surfaceTransformFlagBits;
    if (surfaceCapabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
        surfaceTransformFlagBits = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    else
        surfaceTransformFlagBits = surfaceCapabilities.currentTransform;

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = grm.Surface;
    createInfo.minImageCount = desiredNbOfSwapchainImages;
    createInfo.imageFormat = surfaceFormat;
    createInfo.imageColorSpace = surfaceColorSpace;
    createInfo.imageExtent = swapchainExtent;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.preTransform = surfaceTransformFlagBits;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.imageArrayLayers = 1;
    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
    createInfo.oldSwapchain = oldSwapchain;
    createInfo.clipped = VK_TRUE;

    VkSwapchainKHR swapchain;
    if (vkCreateSwapchainKHR(grm.Device, &createInfo, 0, &swapchain) != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to create VkSwapchainKHR.");
        return ErrorValues::FUNCTION_FAILED;
    }
    Swapchain = swapchain;
    uint32_t imageCount = 0;
    if (vkGetSwapchainImagesKHR(grm.Device, Swapchain, &imageCount, 0) != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to get swapchain VkImage.");
        return ErrorValues::FUNCTION_FAILED;
    }
    if (imageCount < 1) {
        LOGE(FILE_LOCATION( ), "Failed to get swapchain VkImage.");
        return ErrorValues::FUNCTION_FAILED;
    }
    std::vector<VkImage> swapchainImages(imageCount);
    if (vkGetSwapchainImagesKHR(grm.Device, Swapchain, &imageCount, swapchainImages.data( )) != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to get swapchain VkImage.");
        return ErrorValues::FUNCTION_FAILED;
    }

    std::vector<VkImageView> swapchainImageViews(imageCount);

    for (uint32_t i = 0; i < imageCount; i += 1) {
        VkImageViewCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.pNext = 0;
        createInfo.flags = 0;
        createInfo.format = surfaceFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_R;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_G;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_B;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_A;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.image = swapchainImages[i];

        if (vkCreateImageView(grm.Device, &createInfo, 0, &swapchainImageViews[i]) != VK_SUCCESS) {
            LOGE(FILE_LOCATION( ), "Failed to create swapchain VkImageView.");
            return ErrorValues::FUNCTION_FAILED;
        }
    }
    SwapImages = std::move(swapchainImages);
    SwapImageViews = std::move(swapchainImageViews);
    return ErrorValues::NONE;
}

ErrorValues Context::fillInitCmdBuffer(const Manager& grm) {
    VkResult res;
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
        imageMemBarrier.image = DepthImage.getImage( );

        memBarriers.push_back(imageMemBarrier);
    }

    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    res = vkBeginCommandBuffer(InitializerCommandBuffer, &commandBufferBeginInfo);
    if (res != VK_SUCCESS) {
        LOGE(FILE_LOCATION( ), "Failed to begin VkCommandBuffer operation.");
        return ErrorValues::FUNCTION_FAILED;
    }

    vkCmdPipelineBarrier(InitializerCommandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         0, 0, 0, 0, 0, (uint32_t)memBarriers.size( ), memBarriers.data( ));

    res = vkEndCommandBuffer(InitializerCommandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &InitializerCommandBuffer;

    vkQueueSubmit(grm.Queue, 1, &submitInfo, VK_NULL_HANDLE);

    vkQueueWaitIdle(grm.Queue);
    return ErrorValues::NONE;
}

// ErrorValues Context::render(const Manager& grm) {
//     VkResult res;
//     if (PerFrame[CurrentFrame].fenceInitialized) {
//         vkWaitForFences(grm.Device, 1,
//                         &PerFrame[CurrentFrame].presentFence, VK_TRUE,
//                         UINT64_MAX);
//         vkResetFences(grm.Device, 1,
//                       &PerFrame[CurrentFrame].presentFence);
//     }

//     uint32_t imageIndex = UINT32_MAX;
//     res = vkAcquireNextImageKHR(grm.Device, Swapchain, UINT64_MAX,
//                                 PerFrame[CurrentFrame].acquiredSemaphore,
//                                 VK_NULL_HANDLE, &imageIndex);

//     PerFrame[CurrentFrame].fenceInitialized = true;
//     if (res == VK_ERROR_OUT_OF_DATE_KHR) {
//         dprintf(2, "we don't support out-of-date swapchain yet.\n");
//         return ErrorValues::FUNCTION_FAILED;
//     } else if (res == VK_SUBOPTIMAL_KHR)
//         dprintf(1, "Swapchain is suboptimal.\n");
//     else if (res != VK_SUCCESS)
//         return ErrorValues::FUNCTION_FAILED;

//     for (const auto pipeline : m_pipelines) {
//         VkCommandBufferBeginInfo commandbufferBeginInfo = {};
//         commandbufferBeginInfo.sType =
//             VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//         commandbufferBeginInfo.flags =
//             VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

//         res = vkBeginCommandBuffer(m_presentCmdBuffer[CurrentFrame],
//                                    &commandbufferBeginInfo);
//         grm.beginDebugMaker(m_presentCmdBuffer[CurrentFrame],
//                             "PresentCmdBuffer");
//         if (res != VK_SUCCESS) return ErrorValues::FUNCTION_FAILED;

//         VkClearValue clearValues[2];
//         clearValues[0].color.float32[0] = 1.0f;
//         clearValues[0].color.float32[1] = 1.0f;
//         clearValues[0].color.float32[2] = 1.0f;
//         clearValues[0].color.float32[3] = 1.0f;
//         clearValues[1].depthStencil.depth = 1.0f;
//         clearValues[1].depthStencil.stencil = 0.0f;

//         VkRenderPassBeginInfo renderPassBeginInfo = {};
//         renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//         renderPassBeginInfo.renderPass = pipeline.getRenderpass( );
//         renderPassBeginInfo.framebuffer =
//             pipeline.getFramebuffers( )[imageIndex];
//         renderPassBeginInfo.renderArea.offset = {0, 0};
//         renderPassBeginInfo.renderArea.extent = {
//             grm.Window->getWidth( ),
//             grm.Window->getHeight( )};
//         renderPassBeginInfo.clearValueCount = 2;
//         renderPassBeginInfo.pClearValues = clearValues;

//         vkCmdBeginRenderPass(m_presentCmdBuffer[CurrentFrame],
//                              &renderPassBeginInfo,
//                              VK_SUBPASS_CONTENTS_INLINE);

//         vkCmdBindPipeline(m_presentCmdBuffer[CurrentFrame],
//                           VK_PIPELINE_BIND_POINT_GRAPHICS,
//                           pipeline.getPipeline( ));

//         VkViewport viewport = {};
//         viewport.x = 0.0f;
//         viewport.y = 0.0f;
//         viewport.width = (float)grm.Window->getWidth( );
//         viewport.height = (float)grm.Window->getHeight( );
//         viewport.minDepth = 0.0f;
//         viewport.maxDepth = 1.0f;

//         vkCmdSetViewport(m_presentCmdBuffer[CurrentFrame], 0, 1, &viewport);

//         VkRect2D scissor = {};
//         scissor.offset.x = 0;
//         scissor.offset.y = 0;
//         scissor.extent.width = grm.Window->getWidth( );
//         scissor.extent.height = grm.Window->getHeight( );

//         vkCmdSetScissor(m_presentCmdBuffer[CurrentFrame], 0, 1, &scissor);

//         VkDeviceSize bufferOffsets = 0;
//         std::vector<VertexBuffer> vertexBuffers = pipeline.getBuffers( );
//         for (size_t i = 0; i < vertexBuffers.size( ); i += 1) {
//             VkBuffer tmp = vertexBuffers[i].getHandle( );
//             vkCmdBindVertexBuffers(m_presentCmdBuffer[CurrentFrame], i,
//                                    vertexBuffers.size( ), &tmp,
//                                    &bufferOffsets);
//         }

//         vkCmdBindDescriptorSets(m_presentCmdBuffer[CurrentFrame],
//                                 VK_PIPELINE_BIND_POINT_GRAPHICS,
//                                 pipeline.getPipelineLayout( ), 0, 1,
//                                 &pipeline.getDescriptorSets( )[0], 0, 0);

//         vkCmdDraw(m_presentCmdBuffer[CurrentFrame],
//                   pipeline.getVerticesCount( ), 1, 0, 0);

//         vkCmdEndRenderPass(m_presentCmdBuffer[CurrentFrame]);

//         grm.endDebugMaker(m_presentCmdBuffer[CurrentFrame]);
//         res = vkEndCommandBuffer(m_presentCmdBuffer[CurrentFrame]);

//         VkPipelineStageFlags pipelineStageFlags =
//             VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//         VkSubmitInfo submitInfo = {};
//         submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//         submitInfo.waitSemaphoreCount = 1;
//         submitInfo.pWaitSemaphores =
//             &PerFrame[CurrentFrame].acquiredSemaphore;
//         submitInfo.pWaitDstStageMask = &pipelineStageFlags;
//         submitInfo.commandBufferCount = 1;
//         submitInfo.pCommandBuffers = &m_presentCmdBuffer[CurrentFrame];
//         submitInfo.signalSemaphoreCount = 1;
//         submitInfo.pSignalSemaphores =
//             &PerFrame[CurrentFrame].renderCompletedSemaphore;

//         res = vkQueueSubmit(grm.Queue, 1, &submitInfo,
//                             PerFrame[CurrentFrame].presentFence);
//         if (res != VK_SUCCESS) return ErrorValues::FUNCTION_FAILED;

//         VkPresentInfoKHR presentInfo = {};
//         presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//         presentInfo.waitSemaphoreCount = 1;
//         presentInfo.pWaitSemaphores =
//             &PerFrame[CurrentFrame].renderCompletedSemaphore;
//         presentInfo.swapchainCount = 1;
//         presentInfo.pSwapchains = &Swapchain;
//         presentInfo.pImageIndices = &imageIndex;

//         res = vkQueuePresentKHR(grm.Queue, &presentInfo);

//         if (res == VK_ERROR_OUT_OF_DATE_KHR) {
//             dprintf(2, "we don't support out-of-date swapchain yet.\n");
//             return ErrorValues::FUNCTION_FAILED;
//         } else if (res == VK_SUBOPTIMAL_KHR)
//             dprintf(1, "Swapchain is suboptimal.\n");
//         else if (res != VK_SUCCESS)
//             return ErrorValues::FUNCTION_FAILED;
//     }
//     return ErrorValues::NONE;
// }

}    // namespace gr
}    // namespace FEM