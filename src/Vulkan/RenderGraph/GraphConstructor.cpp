#include <cstring>
#include "GraphConstructor.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

VkRenderPass newRenderPass(const VkDevice& Device, VkFormat SurfaceFormat)
{
    VkAttachmentDescription AttachmentDescription[2] = {};
    AttachmentDescription[0].flags = 0;
    AttachmentDescription[0].format = SurfaceFormat;
    AttachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
    AttachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    AttachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    AttachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    AttachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    AttachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    AttachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    AttachmentDescription[1].flags = 0;
    AttachmentDescription[1].format = VK_FORMAT_D16_UNORM;
    AttachmentDescription[1].samples = VK_SAMPLE_COUNT_1_BIT;
    AttachmentDescription[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    AttachmentDescription[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    AttachmentDescription[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    AttachmentDescription[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    AttachmentDescription[1].initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    AttachmentDescription[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachementReference = {};
    colorAttachementReference.attachment = 0;
    colorAttachementReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentReference = {};
    depthAttachmentReference.attachment = 1;
    depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.flags = 0;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = 0;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachementReference;
    subpassDescription.pResolveAttachments = 0;
    subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = 0;

    VkRenderPassCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.attachmentCount = 2;
    createInfo.pAttachments = AttachmentDescription;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpassDescription;
    createInfo.dependencyCount = 0;
    createInfo.pDependencies = 0;

    VkRenderPass renderpass;
    if (vkCreateRenderPass(Device, &createInfo, 0, &renderpass)) {
        return VK_NULL_HANDLE;
    }
    return renderpass;
}

GraphConstructor newGraphConstructor(const VkDevice& Device, const VmaAllocator& Allocator, VkFormat SurfaceFormat, VkExtent2D WindowSize, std::vector<VkImageView> SwapchainViews)
{
    GraphConstructor n;

    memset(&n, 0, sizeof(GraphConstructor));
    n.RenderPass = newRenderPass(Device, SurfaceFormat);
    if (n.RenderPass == VK_NULL_HANDLE)
        return n;
    ImageCreateInfo ImageCreateInfo = {};
    ImageCreateInfo.AsView = true;
    ImageCreateInfo.Extent = WindowSize;
    ImageCreateInfo.Format = VK_FORMAT_D16_UNORM;
    ImageCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ImageCreateInfo.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    ImageCreateInfo.ViewInfos.AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    VmaAllocationCreateInfo ImageAlloc = {};
    ImageAlloc.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    n.DepthImage = CreateImage(Allocator, Device, ImageCreateInfo, ImageAlloc);
    if (n.DepthImage.Handle == VK_NULL_HANDLE || n.DepthImage.View == VK_NULL_HANDLE)
        return n;

    n.Framebuffers.resize(SwapchainViews.size());
    for (uint32_t i = 0; i < n.Framebuffers.size(); ++i) {
        VkImageView attachment[2] = {SwapchainViews[i], n.DepthImage.View};
        VkFramebufferCreateInfo FrameInfo = {};
        FrameInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FrameInfo.renderPass = n.RenderPass;
        FrameInfo.attachmentCount = 2;
        FrameInfo.pAttachments = attachment;
        FrameInfo.width = WindowSize.width;
        FrameInfo.height = WindowSize.height;
        FrameInfo.layers = 1;

        if (vkCreateFramebuffer(Device, &FrameInfo, 0, &n.Framebuffers[i]) != VK_SUCCESS) {
            return n;
        }
    }
    return n;
}

void DestroyGraphConstructor(const VkDevice& Device, const VmaAllocator& Allocator, GraphConstructor& Graph)
{
    DestroyImage(Allocator, Device, Graph.DepthImage);
    for (uint16_t i = 0; i < Graph.Framebuffers.size(); ++i) {
        vkDestroyFramebuffer(Device, Graph.Framebuffers[i], 0);
    }
    vkDestroyRenderPass(Device, Graph.RenderPass, 0);
}

}
}