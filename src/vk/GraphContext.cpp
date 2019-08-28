#include "GraphContext.h"
#include "GraphManager.h"
#include "Buffers.h"
#include "vkcommon.h"

namespace gr
{
    static bool allocateCommandBuffer(const VkDevice device,
                           const VkCommandPool commandPool,
                           const VkCommandBufferLevel commandBufferLevel,
                           VkCommandBuffer& outCommandBuffer
                          )
    {
        VkResult res;

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.pNext = 0;
        allocInfo.commandPool = commandPool;
        allocInfo.level = commandBufferLevel;
        allocInfo.commandBufferCount = 1;

        res = vkAllocateCommandBuffers(device, &allocInfo, &outCommandBuffer);
        if (res != VK_SUCCESS) {
            return Error::FUNCTION_FAILED;
        }
        return true;
    }

    Error Context::init(const Manager& grm)
    {
        if (initInternal(grm))
            return Error::FUNCTION_FAILED;
        return Error::NONE;
    }

    Error Context::initInternal(const Manager& grm)
    {

        VkCommandPoolCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        createInfo.pNext = 0;
        createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        createInfo.queueFamilyIndex = grm.getGraphicsQueueFamily();

        if (vkCreateCommandPool(grm.getDevice(), &createInfo, 0, &m_commandPool) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;
        if (!allocateCommandBuffer(grm.getDevice(), m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_cmdBuffer))
            return Error::FUNCTION_FAILED;
        if (initSwapchain(grm))
            return Error::FUNCTION_FAILED;
        if (m_depthImage.init(grm, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 0, depthBufferFormat, VK_IMAGE_ASPECT_DEPTH_BIT))
            return Error::FUNCTION_FAILED;
        if (fillInitCmdBuffer(grm))
            return Error::FUNCTION_FAILED;

        for (int i = 0; i < 2; i += 1) {
            allocateCommandBuffer(grm.getDevice(), m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_presentCmdBuffer[i]);
            createFence(grm.getDevice(), m_perFrame[i].presentFence);
            createSemaphore(grm.getDevice(), m_perFrame[i].acquiredSemaphore);
            createSemaphore(grm.getDevice(), m_perFrame[i].renderCompletedSemaphore);
            m_perFrame[i].fenceInitialized = false;
        }

        return Error::NONE;
    }

    Error Context::reload(const Manager& grm)
    {
        vkDeviceWaitIdle(grm.getDevice());
        for (const auto pipeline : m_pipelines) {
            for (size_t i = 0; i < pipeline.getFramebuffers().size(); i += 1) {
                vkDestroyFramebuffer(grm.getDevice(), pipeline.getFramebuffers()[i], 0);
            }
            pipeline.getFramebuffers().clear();
        }

        vkFreeCommandBuffers(grm.getDevice(), m_commandPool, 1, &m_cmdBuffer);
        vkFreeCommandBuffers(grm.getDevice(), m_commandPool, 2, m_presentCmdBuffer);
        for (const auto pipeline : m_pipelines) {
            vkDestroyPipeline(grm.getDevice(), pipeline.getPipeline(), 0);
            vkDestroyPipelineLayout(grm.getDevice(), pipeline.getPipelineLayout(), 0);
            vkDestroyRenderPass(grm.getDevice(), pipeline.getRenderpass(), 0);
        }

        for (size_t i = 0; i < m_swapImages.size(); i += 1) {
            vkDestroyImage(grm.getDevice(), m_swapImages[i], 0);
        }
        m_swapImages.clear();
        m_depthImage.destroy(grm);

        for (size_t i = 0; i < m_swapImageViews.size(); i += 1) {
            vkDestroyImageView(grm.getDevice(), m_swapImageViews[i], 0);
        }
        m_swapImageViews.clear();
        if (initSwapchain(grm))
            return Error::FUNCTION_FAILED;
        if (m_depthImage.init(grm, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 0, depthBufferFormat, VK_IMAGE_ASPECT_DEPTH_BIT))
            return Error::FUNCTION_FAILED;
        if (!allocateCommandBuffer(grm.getDevice(), m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_cmdBuffer))
            return Error::FUNCTION_FAILED;
        if (fillInitCmdBuffer(grm))
            return Error::FUNCTION_FAILED;
        // if (m_pipeline.reload(grm, *this))
        //     return Error::FUNCTION_FAILED;
        for (int i = 0; i < 2; i += 1) {
            allocateCommandBuffer(grm.getDevice(), m_commandPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, m_presentCmdBuffer[i]);
            m_perFrame[i].fenceInitialized = false;
        }
        return Error::NONE;
    }

    Error Context::initSwapchain(const Manager& grm)
    {
        uint32_t surfaceFormatCount;

        VkSwapchainKHR oldSwapchain = (m_swapchain) ? m_swapchain : VK_NULL_HANDLE;

        if (vkGetPhysicalDeviceSurfaceFormatsKHR(grm.getPhysicalDevice(), grm.getSurface(), &surfaceFormatCount, 0) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;
        if (surfaceFormatCount < 1)
            return Error::FUNCTION_FAILED;

        std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
        if (vkGetPhysicalDeviceSurfaceFormatsKHR(grm.getPhysicalDevice(), grm.getSurface(), &surfaceFormatCount, surfaceFormats.data()) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;

        VkFormat surfaceFormat;
        if (surfaceFormatCount == 1 && surfaceFormats[0].format == VK_FORMAT_UNDEFINED)
            surfaceFormat = VK_FORMAT_B8G8R8_UNORM;
        else
            surfaceFormat = surfaceFormats[0].format;

        m_surfaceFormat = surfaceFormat;

        VkColorSpaceKHR surfaceColorSpace = surfaceFormats[0].colorSpace;

        VkSurfaceCapabilitiesKHR surfaceCapabilities;
        if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(grm.getPhysicalDevice(), grm.getSurface(), &surfaceCapabilities) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;

        uint32_t presentModeCount = 0;
        if (vkGetPhysicalDeviceSurfacePresentModesKHR(grm.getPhysicalDevice(), grm.getSurface(), &presentModeCount, 0) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;
        if (presentModeCount < 1)
            return Error::FUNCTION_FAILED;

        std::vector<VkPresentModeKHR> presentModes(presentModeCount);
        if (vkGetPhysicalDeviceSurfacePresentModesKHR(grm.getPhysicalDevice(), grm.getSurface(), &presentModeCount, presentModes.data()) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;

        VkExtent2D swapchainExtent = surfaceCapabilities.currentExtent;

        int windowWidth, windowHeight = 0;
        glfwGetWindowSize(grm.getNativeWindow().getNativeWindow(), &windowWidth, &windowHeight);
        if (swapchainExtent.width == (uint32_t)(-1)) {
            swapchainExtent.width = static_cast<uint32_t>(windowWidth);
            swapchainExtent.height = static_cast<uint32_t>(windowHeight);
        } else if (swapchainExtent.width != static_cast<uint32_t>(windowWidth) && swapchainExtent.height != static_cast<uint32_t>(windowHeight)) {
            return Error::FUNCTION_FAILED;
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
        createInfo.surface = grm.getSurface();
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
        if (vkCreateSwapchainKHR(grm.getDevice(), &createInfo, 0, &swapchain) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;
        m_swapchain = swapchain;
        // if (oldSwapchain != VK_NULL_HANDLE)
        //     vkDestroySwapchainKHR(grm.getDevice(), oldSwapchain, 0);
        uint32_t imageCount = 0;
        if (vkGetSwapchainImagesKHR(grm.getDevice(), m_swapchain, &imageCount, 0) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;
        if (imageCount < 1)
            return Error::FUNCTION_FAILED;
        std::vector<VkImage> swapchainImages(imageCount);
        if (vkGetSwapchainImagesKHR(grm.getDevice(), m_swapchain, &imageCount, swapchainImages.data()) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;

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

            if (vkCreateImageView(grm.getDevice(), &createInfo, 0, &swapchainImageViews[i]) != VK_SUCCESS)
                return Error::FUNCTION_FAILED;
        }
        m_swapImages = std::move(swapchainImages);
        m_swapImageViews = std::move(swapchainImageViews);
        return Error::NONE;
    }

    Error Context::fillInitCmdBuffer(const Manager& grm)
    {
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
            imageMemBarrier.image = m_depthImage.getImage();

            memBarriers.push_back(imageMemBarrier);
        }

        VkCommandBufferBeginInfo commandBufferBeginInfo = {};
        commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        res = vkBeginCommandBuffer(m_cmdBuffer, &commandBufferBeginInfo);
        if (res != VK_SUCCESS) {
            return Error::FUNCTION_FAILED;
        }

        vkCmdPipelineBarrier(m_cmdBuffer,
                            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                            VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                            0,
                            0,
                            0,
                            0,
                            0,
                            (uint32_t)memBarriers.size(),
                            memBarriers.data());

        res = vkEndCommandBuffer(m_cmdBuffer);

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_cmdBuffer;

        vkQueueSubmit(grm.getQueue(), 1, &submitInfo, VK_NULL_HANDLE);

        vkQueueWaitIdle(grm.getQueue());
        return Error::NONE;
    }

    Error Context::render(const Manager& grm)
    {
        if (m_pipelines.empty())
            return Error::NONE;
        VkResult res;
        if (m_perFrame[current_frame].fenceInitialized) {
            vkWaitForFences(grm.getDevice(), 1, &m_perFrame[current_frame].presentFence, VK_TRUE, UINT64_MAX);
            vkResetFences(grm.getDevice(), 1, &m_perFrame[current_frame].presentFence);
        }

        uint32_t imageIndex = UINT32_MAX;
        res = vkAcquireNextImageKHR(grm.getDevice(), m_swapchain, UINT64_MAX, m_perFrame[current_frame].acquiredSemaphore, VK_NULL_HANDLE, &imageIndex);

        m_perFrame[current_frame].fenceInitialized = true;
        if (res == VK_ERROR_OUT_OF_DATE_KHR) {
            dprintf(2, "we don't support out-of-date swapchain yet.\n");
            return Error::FUNCTION_FAILED;
        } else if (res == VK_SUBOPTIMAL_KHR)
            dprintf(1, "Swapchain is suboptimal.\n");
        else if (res != VK_SUCCESS)
            return Error::FUNCTION_FAILED;

        for (const auto pipeline : m_pipelines) {
            VkCommandBufferBeginInfo commandbufferBeginInfo = {};
            commandbufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            commandbufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

            res = vkBeginCommandBuffer(m_presentCmdBuffer[current_frame], &commandbufferBeginInfo);
            if (res != VK_SUCCESS) return Error::FUNCTION_FAILED;

            VkClearValue clearValues[2];
            clearValues[0].color.float32[0] = 1.0f;
            clearValues[0].color.float32[1] = 1.0f;
            clearValues[0].color.float32[2] = 1.0f;
            clearValues[0].color.float32[3] = 1.0f;
            clearValues[1].depthStencil.depth = 1.0f;
            clearValues[1].depthStencil.stencil = 0.0f;

            VkRenderPassBeginInfo renderPassBeginInfo = {};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassBeginInfo.renderPass = pipeline.getRenderpass();
            renderPassBeginInfo.framebuffer = pipeline.getFramebuffers()[imageIndex];
            renderPassBeginInfo.renderArea.offset = {0, 0};
            renderPassBeginInfo.renderArea.extent = {grm.getNativeWindow().getWidth(), grm.getNativeWindow().getHeight()};
            renderPassBeginInfo.clearValueCount = 2;
            renderPassBeginInfo.pClearValues = clearValues;

            vkCmdBeginRenderPass(m_presentCmdBuffer[current_frame], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

            vkCmdBindPipeline(m_presentCmdBuffer[current_frame], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getPipeline());

            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = (float)grm.getNativeWindow().getWidth();
            viewport.height = (float)grm.getNativeWindow().getHeight();
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;

            vkCmdSetViewport(m_presentCmdBuffer[current_frame], 0, 1, &viewport);

            VkRect2D scissor = {};
            scissor.offset.x = 0;
            scissor.offset.y = 0;
            scissor.extent.width = grm.getNativeWindow().getWidth();
            scissor.extent.height = grm.getNativeWindow().getHeight();

            vkCmdSetScissor(m_presentCmdBuffer[current_frame], 0, 1, &scissor);

            VkDeviceSize bufferOffsets = 0;
            std::vector<VertexBuffer> vertexBuffers = pipeline.getBuffers();
            for (size_t i = 0; i < vertexBuffers.size(); i += 1) {
                VkBuffer tmp = vertexBuffers[i].getHandle();
                vkCmdBindVertexBuffers(m_presentCmdBuffer[current_frame], i, vertexBuffers.size(), &tmp, &bufferOffsets);
            }

            vkCmdPushConstants(m_presentCmdBuffer[current_frame],
                                pipeline.getPipelineLayout(),
                                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                0,
                                4,
                                &m_animTime);

            vkCmdDraw(m_presentCmdBuffer[current_frame], 3, 1, 0, 0);

            vkCmdEndRenderPass(m_presentCmdBuffer[current_frame]);

            res = vkEndCommandBuffer(m_presentCmdBuffer[current_frame]);

            VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            VkSubmitInfo submitInfo = {};
            submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &m_perFrame[current_frame].acquiredSemaphore;
            submitInfo.pWaitDstStageMask = &pipelineStageFlags;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &m_presentCmdBuffer[current_frame];
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &m_perFrame[current_frame].renderCompletedSemaphore;

            res = vkQueueSubmit(grm.getQueue(), 1, &submitInfo, m_perFrame[current_frame].presentFence);
            if (res != VK_SUCCESS) return Error::FUNCTION_FAILED;

            VkPresentInfoKHR presentInfo = {};
            presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
            presentInfo.waitSemaphoreCount = 1;
            presentInfo.pWaitSemaphores = &m_perFrame[current_frame].renderCompletedSemaphore;
            presentInfo.swapchainCount = 1;
            presentInfo.pSwapchains = &m_swapchain;
            presentInfo.pImageIndices = &imageIndex;

            res = vkQueuePresentKHR(grm.getQueue(), &presentInfo);


            if (res == VK_ERROR_OUT_OF_DATE_KHR) {
                dprintf(2, "we don't support out-of-date swapchain yet.\n");
                return Error::FUNCTION_FAILED;
            } else if (res == VK_SUBOPTIMAL_KHR)
                dprintf(1, "Swapchain is suboptimal.\n");
            else if (res != VK_SUCCESS)
                return Error::FUNCTION_FAILED;
        }
        return Error::NONE;
    }

    Error Context::addPipeline(const Manager& grm, VertexBuffer& object, const char *vertexShaderFile, const char *fragmentShaderFile)
    {
        Pipeline pipeline;

        pipeline.addData(object);
        if (pipeline.initShaders(grm.getDevice(), vertexShaderFile, fragmentShaderFile))
            return Error::FUNCTION_FAILED;
        if (pipeline.init(grm, *this))
            return Error::FUNCTION_FAILED;
        m_pipelines.push_back(pipeline);
        return Error::NONE;
    }

    Error Context::initGlobalDescriptor(const Manager& grm)
    {
        VkDescriptorSetLayoutBinding layoutBinding = {};

        layoutBinding.binding = 0;
        layoutBinding.descriptorCount = 1;
        layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        layoutBinding.pImmutableSamplers = 0;
        layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = 1;
        layoutInfo.pBindings = &layoutBinding;

        if (vkCreateDescriptorSetLayout(grm.getDevice(), &layoutInfo, 0, &m_globalInfoLayout))
            return Error::FUNCTION_FAILED;
        return Error::NONE;
    }

} // namespace gr