#include "GraphContext.h"
#include "GraphManager.h"

namespace gr
{

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
        if (initSwapchain(grm))
            return Error::FUNCTION_FAILED;
        if (initRenderpass(grm))
            return Error::FUNCTION_FAILED;
        if (m_depthImage.init(grm.getDevice(), grm.getPhysicalDeviceMemProps(),
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 0, depthBufferFormat,
            grm.getNativeWindow().getWidth(), grm.getNativeWindow().getHeight(),
            VK_IMAGE_ASPECT_DEPTH_BIT))
            return Error::FUNCTION_FAILED;
        if (initFramebuffer(grm))
            return Error::FUNCTION_FAILED;
        return Error::NONE;
    }

    Error Context::reload(const Manager& grm)
    {
        vkDeviceWaitIdle(grm.getDevice());

        for (size_t i = 0; i < m_framebuffers.size(); i += 1)
            vkDestroyFramebuffer(grm.getDevice(), m_framebuffers[i], 0);
        vkFreeCommandBuffers(grm.getDevice(), m_commandPool, 1, &m_cmdBuffer);
        vkFreeCommandBuffers(grm.getDevice(), m_commandPool, 2, m_presentCmdBuffer);

        // Destroy PipelineLayout/Pipeline
        vkDestroyRenderPass(grm.getDevice(), m_renderpass, 0);

        for (size_t i = 0; i < m_swapImages.size(); i += 1) {
            vkDestroyImage(grm.getDevice(), m_swapImages[i], 0);
        }
        m_swapImages.clear();

        for (size_t i = 0; i < m_swapImageViews.size(); i += 1) {
            vkDestroyImageView(grm.getDevice(), m_swapImageViews[i], 0);
        }
        m_swapImageViews.clear();
        if (initSwapchain(grm))
            return Error::FUNCTION_FAILED;
        if (initRenderpass(grm))
            return Error::FUNCTION_FAILED;
        if (m_depthImage.init(grm.getDevice(), grm.getPhysicalDeviceMemProps(),
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, 0, depthBufferFormat,
            grm.getNativeWindow().getWidth(), grm.getNativeWindow().getHeight(),
            VK_IMAGE_ASPECT_DEPTH_BIT))
            return Error::FUNCTION_FAILED;
        if (initFramebuffer(grm))
            return Error::FUNCTION_FAILED;
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

        if (vkCreateSwapchainKHR(grm.getDevice(), &createInfo, 0, &m_swapchain) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;
        if (oldSwapchain != VK_NULL_HANDLE)
            vkDestroySwapchainKHR(grm.getDevice(), oldSwapchain, 0);
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

    Error Context::initRenderpass(const Manager& grm)
    {
        VkAttachmentDescription attachmentDescription[2] = {{}, {}};
        attachmentDescription[0].flags = 0;
        attachmentDescription[0].format = m_surfaceFormat;
        attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachmentDescription[1].flags = 0;
        attachmentDescription[1].format = depthBufferFormat;
        attachmentDescription[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachmentDescription[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference colorAttachmentReference = {};
        colorAttachmentReference.attachment = 0;
        colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRefence = {};
        depthAttachmentRefence.attachment = 1;
        depthAttachmentRefence.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpassDescription = {};
        subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpassDescription.flags = 0;
        subpassDescription.inputAttachmentCount = 0;
        subpassDescription.pInputAttachments = 0;
        subpassDescription.colorAttachmentCount = 1;
        subpassDescription.pColorAttachments = &colorAttachmentReference;
        subpassDescription.pResolveAttachments = 0;
        subpassDescription.pDepthStencilAttachment = &depthAttachmentRefence;
        subpassDescription.preserveAttachmentCount = 0;
        subpassDescription.pPreserveAttachments = 0;

        VkRenderPassCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        createInfo.pNext = 0;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = attachmentDescription;
        createInfo.subpassCount = 1;
        createInfo.pSubpasses = &subpassDescription;
        createInfo.dependencyCount = 0;
        createInfo.pDependencies = 0;

        if (vkCreateRenderPass(grm.getDevice(), &createInfo, 0, &m_renderpass) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;
        return Error::NONE;
    }

    Error Context::initFramebuffer(const Manager& grm)
    {
        for (const auto view : m_swapImageViews) {
            VkFramebuffer fb;

            VkImageView attachment[2] = {view, m_depthImage.getImageView()};
            VkFramebufferCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            createInfo.pNext = 0;
            createInfo.flags = 0;
            createInfo.renderPass = m_renderpass;
            createInfo.attachmentCount = 2;
            createInfo.pAttachments = attachment;
            createInfo.width = grm.getNativeWindow().getWidth();
            createInfo.height = grm.getNativeWindow().getHeight();
            createInfo.layers = 1;

            vkCreateFramebuffer(grm.getDevice(), &createInfo, 0, &fb);
            m_framebuffers.push_back(fb);
        }
        return Error::NONE;
    }

} // namespace gr