#include <cstring>
#include "GraphConstructor.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

VkRenderPass newRenderPass(const VkDevice& Device, VkFormat SurfaceFormat, VkSampleCountFlagBits msaaSample) {
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

GraphConstructor newGraphConstructor(const Device& D, const VmaAllocator& Allocator, VkFormat SurfaceFormat,
                                     VkExtent2D WindowSize, std::vector<VkImageView> SwapchainViews) {
    GraphConstructor n;

    memset(&n, 0, sizeof(GraphConstructor));
    n.RenderPass = newRenderPass(D.Handle, SurfaceFormat, D.PhysicalHandleCapabilities.msaaSamples);
    if (n.RenderPass == VK_NULL_HANDLE) return n;
    ImageCreateInfo DepthCreateInfo = {};
    DepthCreateInfo.AsView = true;
    DepthCreateInfo.Extent = WindowSize;
    DepthCreateInfo.Format = VK_FORMAT_D16_UNORM;
    DepthCreateInfo.SampleCount = D.PhysicalHandleCapabilities.msaaSamples;
    DepthCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    DepthCreateInfo.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    DepthCreateInfo.ViewInfos.AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

    VmaAllocationCreateInfo ImageAlloc = {};
    ImageAlloc.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    n.DepthImage = CreateImage(Allocator, D.Handle, DepthCreateInfo, ImageAlloc);
    if (n.DepthImage.Handle == VK_NULL_HANDLE || n.DepthImage.View == VK_NULL_HANDLE) return n;

    ImageCreateInfo ColorCreateInfo = {};
    ColorCreateInfo.AsView = true;
    ColorCreateInfo.Extent = WindowSize;
    ColorCreateInfo.Format = SurfaceFormat;
    ColorCreateInfo.SampleCount = D.PhysicalHandleCapabilities.msaaSamples;
    ColorCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    ColorCreateInfo.Usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    ColorCreateInfo.ViewInfos.AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

    n.ColorImage = CreateImage(Allocator, D.Handle, ColorCreateInfo, ImageAlloc);
    if (n.ColorImage.Handle == VK_NULL_HANDLE || n.ColorImage.View == VK_NULL_HANDLE) return n;

    n.Framebuffers.resize(SwapchainViews.size( ));
    for (uint32_t i = 0; i < n.Framebuffers.size( ); ++i) {
        VkImageView attachment[3] = {n.ColorImage.View, n.DepthImage.View, SwapchainViews[i]};
        VkFramebufferCreateInfo FrameInfo = {};
        FrameInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        FrameInfo.renderPass = n.RenderPass;
        FrameInfo.attachmentCount = 3;
        FrameInfo.pAttachments = attachment;
        FrameInfo.width = WindowSize.width;
        FrameInfo.height = WindowSize.height;
        FrameInfo.layers = 1;

        if (vkCreateFramebuffer(D.Handle, &FrameInfo, 0, &n.Framebuffers[i]) != VK_SUCCESS) {
            return n;
        }
    }
    return n;
}

void DestroyGraphConstructor(const VkDevice& Device, const VmaAllocator& Allocator, GraphConstructor& Graph) {
    DestroyImage(Allocator, Device, Graph.DepthImage);
    DestroyImage(Allocator, Device, Graph.ColorImage);
    for (uint16_t i = 0; i < Graph.Framebuffers.size( ); ++i) {
        vkDestroyFramebuffer(Device, Graph.Framebuffers[i], 0);
    }
    vkDestroyRenderPass(Device, Graph.RenderPass, 0);
}

}    // namespace Vulkan
}    // namespace ffGraph