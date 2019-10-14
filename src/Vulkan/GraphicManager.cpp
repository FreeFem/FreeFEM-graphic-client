#include "Environment.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static bool pushDepthImage(const VkDevice& Device, const VkQueue& Queue, const Image DepthImage,
                           const VkCommandPool& Pool) {
    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandPool = Pool;
    AllocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    if (vkAllocateCommandBuffers(Device, &AllocInfo, &cmdBuffer)) return false;
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

    vkQueueSubmit(Queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(Queue);

    vkFreeCommandBuffers(Device, Pool, 1, &cmdBuffer);
    return true;
}

static bool pushColorImage(const VkDevice& Device, const VkQueue& Queue, const Image ColorImage,
                           const VkCommandPool& Pool) {
    VkCommandBufferAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    AllocInfo.commandPool = Pool;
    AllocInfo.commandBufferCount = 1;

    VkCommandBuffer cmdBuffer;
    if (vkAllocateCommandBuffers(Device, &AllocInfo, &cmdBuffer)) return false;
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

    vkQueueSubmit(Queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(Queue);

    vkFreeCommandBuffers(Device, Pool, 1, &cmdBuffer);
    return true;
}

bool pushInitCmdBuffer(const VkDevice& Device, const VkQueue& Queue, const Image DepthImage, const Image ColorImage,
                       const VkCommandPool& Pool) {
    if (!pushDepthImage(Device, Queue, DepthImage, Pool)) return false;
    if (!pushColorImage(Device, Queue, ColorImage, Pool)) return false;
    return true;
}

static VkRenderPass newRenderPass(const VkDevice& Device, VkFormat SurfaceFormat, VkSampleCountFlagBits msaaSample) {
    VkAttachmentDescription AttachmentDescription[3] = {};
    AttachmentDescription[0].flags = 0;
    AttachmentDescription[0].format = SurfaceFormat;
    AttachmentDescription[0].samples = msaaSample;
    AttachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    AttachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    AttachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    AttachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    AttachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    AttachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    AttachmentDescription[1].flags = 0;
    AttachmentDescription[1].format = VK_FORMAT_D16_UNORM;
    AttachmentDescription[1].samples = msaaSample;
    AttachmentDescription[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    AttachmentDescription[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    AttachmentDescription[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    AttachmentDescription[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    AttachmentDescription[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    AttachmentDescription[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    AttachmentDescription[2].flags = 0;
    AttachmentDescription[2].format = SurfaceFormat;
    AttachmentDescription[2].samples = VK_SAMPLE_COUNT_1_BIT;
    AttachmentDescription[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    AttachmentDescription[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    AttachmentDescription[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    AttachmentDescription[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    AttachmentDescription[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    AttachmentDescription[2].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachementReference = {};
    colorAttachementReference.attachment = 0;
    colorAttachementReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference resolveAttachmentReference = {};
    resolveAttachmentReference.attachment = 2;
    resolveAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachementReference;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
    subpassDescription.pResolveAttachments = &resolveAttachmentReference;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.attachmentCount = 3;
    createInfo.pAttachments = AttachmentDescription;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpassDescription;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;

    VkRenderPass renderpass;
    if (vkCreateRenderPass(Device, &createInfo, 0, &renderpass)) {
        return VK_NULL_HANDLE;
    }
    return renderpass;
}

bool CreateGraphicInformations(Environment::GraphicInformations& GInfos, const Environment& Env,
                               const NativeWindow& Window) {
    GInfos.RenderPass =
        newRenderPass(Env.GPUInfos.Device, Env.ScreenInfos.SurfaceFormat.format, Env.GPUInfos.Capabilities.msaaSamples);

    ImageCreateInfo DepthCreateInfo = {};
    DepthCreateInfo.AsView = true;
    DepthCreateInfo.Extent = Window.WindowSize;
    DepthCreateInfo.Format = VK_FORMAT_D16_UNORM;
    DepthCreateInfo.SampleCount = Env.GPUInfos.Capabilities.msaaSamples;
    DepthCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    DepthCreateInfo.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    DepthCreateInfo.ViewInfos.AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    VmaAllocationCreateInfo ImageAlloc = {};
    ImageAlloc.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    GInfos.DepthImage = CreateImage(Env.Allocator, Env.GPUInfos.Device, DepthCreateInfo, ImageAlloc);
    if (GInfos.DepthImage.Handle == VK_NULL_HANDLE || GInfos.DepthImage.View == VK_NULL_HANDLE) {
        LogError("newGraphConstructor", "Failed to create DepthImage.\n");
        return false;
    }
    ImageCreateInfo ColorCreateInfo = {};
    ColorCreateInfo.AsView = true;
    ColorCreateInfo.Extent = Window.WindowSize;
    ColorCreateInfo.Format = Env.ScreenInfos.SurfaceFormat.format;
    ColorCreateInfo.SampleCount = Env.GPUInfos.Capabilities.msaaSamples;
    ColorCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ColorCreateInfo.Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    ColorCreateInfo.ViewInfos.AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    GInfos.ColorImage = CreateImage(Env.Allocator, Env.GPUInfos.Device, ColorCreateInfo, ImageAlloc);
    if (GInfos.ColorImage.Handle == VK_NULL_HANDLE || GInfos.ColorImage.View == VK_NULL_HANDLE) return false;

    GInfos.Framebuffers.resize(Env.ScreenInfos.Views.size( ));
    for (uint32_t i = 0; i < GInfos.Framebuffers.size( ); ++i) {
        VkImageView attachment[3] = {GInfos.ColorImage.View, GInfos.DepthImage.View, Env.ScreenInfos.Views[i]};
        VkFramebufferCreateInfo FrameInfo = {};
        FrameInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FrameInfo.renderPass = GInfos.RenderPass;
        FrameInfo.attachmentCount = 3;
        FrameInfo.pAttachments = attachment;
        FrameInfo.width = Window.WindowSize.width;
        FrameInfo.height = Window.WindowSize.height;
        FrameInfo.layers = 1;

        if (vkCreateFramebuffer(Env.GPUInfos.Device, &FrameInfo, 0, &GInfos.Framebuffers[i]) != VK_SUCCESS) {
            return false;
        }
    }

    VkCommandPoolCreateInfo CmdPoolCreateInfos = {};
    CmdPoolCreateInfos.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CmdPoolCreateInfos.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CmdPoolCreateInfos.queueFamilyIndex = Env.GPUInfos.QueueIndex[Env.GPUInfos.GraphicQueueIndex];

    if (GInfos.CommandPool == VK_NULL_HANDLE) {
        if (vkCreateCommandPool(Env.GPUInfos.Device, &CmdPoolCreateInfos, 0, &GInfos.CommandPool)) {
            LogError(GetCurrentLogLocation( ), "Failed to create VkCommandPool.");
            return false;
        }
    }
    pushInitCmdBuffer(Env.GPUInfos.Device, Env.GPUInfos.Queues[Env.GPUInfos.GraphicQueueIndex], GInfos.DepthImage,
                      GInfos.ColorImage, GInfos.CommandPool);
    // To-Do : Create descriptorSet Pool
    return true;
}

void DestroyGraphicManager(Environment::GraphicInformations& GInfos, const Environment& Env) {
    vkDestroyRenderPass(Env.GPUInfos.Device, GInfos.RenderPass, 0);

    for (auto& frame : GInfos.Framebuffers) {
        vkDestroyFramebuffer(Env.GPUInfos.Device, frame, 0);
    }

    DestroyImage(Env.Allocator, Env.GPUInfos.Device, GInfos.DepthImage);
    DestroyImage(Env.Allocator, Env.GPUInfos.Device, GInfos.ColorImage);

    vkDestroyCommandPool(Env.GPUInfos.Device, GInfos.CommandPool, 0);
    // vkDestroyDescriptorPool(Env.GPUInfos.Device, GInfos.DescriptorPool, 0);
}

}    // namespace Vulkan
}    // namespace ffGraph