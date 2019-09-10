#include "Pipeline.h"
#include <cstdlib>
#include <glm/mat4x4.hpp>
#include "../core/Window.h"
#include "VulkanContext.h"

namespace FEM {
namespace VK {

bool newPipeline(Pipeline *Handle, const VulkanContext vkContext, const Window Win) {
    Handle->SubPipelineCount = 0;
    Handle->SubPipelines = 0;

    Handle->Cam = (Camera *)malloc(sizeof(Camera));
    setProjectionOrtho(Handle->Cam, -1, 11, -5, 5);

    VkAttachmentDescription attachmentDescription[2] = {{}, {}};
    attachmentDescription[0].flags = 0;
    attachmentDescription[0].format = vkContext.SurfaceFormat.format;
    attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachmentDescription[1].flags = 0;
    attachmentDescription[1].format = vkContext.DepthImage.ImgInfos.Format;
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

    if (vkCreateRenderPass(vkContext.Device, &createInfo, 0, &Handle->RenderPass)) return false;

    Handle->FramebufferCount = vkContext.SwapchainImageCount;
    Handle->Framebuffers = (VkFramebuffer *)malloc(sizeof(VkFramebuffer) * vkContext.SwapchainImageCount);
    for (uint32_t i = 0; i < vkContext.SwapchainImageCount; i += 1) {
        VkImageView attachment[2] = {vkContext.SwapchainImageViews[i], vkContext.DepthImage.View};
        VkFramebufferCreateInfo fbCreateInfo = {};
        fbCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        fbCreateInfo.renderPass = Handle->RenderPass;
        fbCreateInfo.attachmentCount = 2;
        fbCreateInfo.pAttachments = attachment;
        fbCreateInfo.width = Win.ScreenWidth;
        fbCreateInfo.height = Win.ScreenHeight;
        fbCreateInfo.layers = 1;

        vkCreateFramebuffer(vkContext.Device, &fbCreateInfo, 0, &Handle->Framebuffers[i]);
    }
    return true;
}

void destroySubPipeline(PipelineSubResources *SubPipeline, const VulkanContext vkContext,
                        const Pipeline MotherPipeline) {
    vkDestroyPipelineLayout(vkContext.Device, SubPipeline->Layout, 0);
    vkDestroyPipeline(vkContext.Device, SubPipeline->Handle, 0);
    vmaDestroyBuffer(vkContext.Allocator, SubPipeline->VBuffer.VulkanData.Handle,
                     SubPipeline->VBuffer.VulkanData.Memory);
}

void destroyPipeline(Pipeline Handle, const VulkanContext vkContext) {
    PipelineSubResources *current = Handle.SubPipelines;
    PipelineSubResources *next = 0;
    while (current != 0) {
        next = current->next;
        destroySubPipeline(current, vkContext, Handle);
        current = next;
    }
    vkDestroyRenderPass(vkContext.Device, Handle.RenderPass, 0);
    for (uint32_t i = 0; i < Handle.FramebufferCount; i += 1) {
        vkDestroyFramebuffer(vkContext.Device, Handle.Framebuffers[i], 0);
    }
    free(Handle.Cam);
}

bool addSubPipeline(PipelineSubResources *SubPipeline, const VulkanContext vkContext, Pipeline *MotherPipeline) {
    if (SubPipeline->VertexShader == 0 || SubPipeline->FragmentShader == 0) return false;
    VkPushConstantRange pushRange;
    pushRange.offset = 0;
    pushRange.size = sizeof(glm::mat4);
    pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushRange;

    vkCreatePipelineLayout(vkContext.Device, &pipelineLayoutCreateInfo, 0, &SubPipeline->Layout);

    VkPipelineShaderStageCreateInfo ShaderStageCreateInfo[2] = {};
    ShaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    ShaderStageCreateInfo[0].module = SubPipeline->VertexShader;
    ShaderStageCreateInfo[0].pName = "main";

    ShaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ShaderStageCreateInfo[1].module = SubPipeline->FragmentShader;
    ShaderStageCreateInfo[1].pName = "main";

    uint32_t BindingPoint = 0;
    VkVertexInputBindingDescription inputBindigDescription = {};
    inputBindigDescription.binding = BindingPoint;
    inputBindigDescription.stride = SubPipeline->VBuffer.VulkanData.CreationInfos.ElementSize;
    inputBindigDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription AttributeArray[SubPipeline->VBuffer.AttributeCount];
    for (uint32_t i = 0; i < SubPipeline->VBuffer.AttributeCount; i += 1) {
        AttributeArray[i].binding = BindingPoint;
        AttributeArray[i].location = i;
        AttributeArray[i].offset = SubPipeline->VBuffer.Attributes[i].offset;
        AttributeArray[i].format = SubPipeline->VBuffer.Attributes[i].format;
    }

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfos = {};
    vertexInputStateCreateInfos.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfos.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfos.pVertexBindingDescriptions = &inputBindigDescription;
    vertexInputStateCreateInfos.vertexAttributeDescriptionCount = SubPipeline->VBuffer.AttributeCount;
    vertexInputStateCreateInfos.pVertexAttributeDescriptions = AttributeArray;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyCreateInfo = {};
    inputAssemblyCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    inputAssemblyCreateInfo.primitiveRestartEnable = VK_FALSE;

    VkDynamicState dynamicStateEnables[2] = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};

    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
    dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo.dynamicStateCount = 2;
    dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables;

    VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
    viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.lineWidth = 2;

    VkPipelineColorBlendAttachmentState colorBlendAttachementState = {};
    colorBlendAttachementState.blendEnable = VK_FALSE;
    colorBlendAttachementState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachementState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachementState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_CLEAR;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachementState;
    memset(colorBlendStateCreateInfo.blendConstants, 0, sizeof(float) * 4);

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    depthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilStateCreateInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.stencilTestEnable = VK_FALSE;

    depthStencilStateCreateInfo.front.failOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.front.passOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.front.depthFailOp = VK_STENCIL_OP_KEEP;
    depthStencilStateCreateInfo.front.compareOp = VK_COMPARE_OP_ALWAYS;

    depthStencilStateCreateInfo.back = depthStencilStateCreateInfo.front;

    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo = {};
    multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = 2;
    graphicsPipelineCreateInfo.pStages = ShaderStageCreateInfo;
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfos;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = SubPipeline->Layout;
    graphicsPipelineCreateInfo.renderPass = MotherPipeline->RenderPass;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(vkContext.Device, 0, 1, &graphicsPipelineCreateInfo, 0, &SubPipeline->Handle))
        return false;
    MotherPipeline->SubPipelineCount += 1;
    if (MotherPipeline->SubPipelines == 0) {
        MotherPipeline->SubPipelines = SubPipeline;
    } else {
        SubPipeline->next = MotherPipeline->SubPipelines;
        MotherPipeline->SubPipelines = SubPipeline->next;
    }
    return true;
}

}    // namespace VK
}    // namespace FEM
