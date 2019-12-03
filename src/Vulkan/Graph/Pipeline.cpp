#include <iostream>
#include "Pipeline.h"
#include "GlobalEnvironment.h"

namespace ffGraph {
namespace Vulkan {

bool ConstructPipeline(Pipeline& P, PipelineCreateInfos& CreateInfo)
{
    if (P.Handle != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(GetLogicalDevice( ), P.Layout, 0);
        vkDestroyPipeline(GetLogicalDevice( ), P.Handle, 0);
    }

    VkPipelineLayoutCreateInfo LayoutCreateInfo = {};
    LayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    if (!CreateInfo.DescriptorListHandle.List.empty()) {
        ConstructDescriptorPool(P.DescriptorPool, CreateInfo.DescriptorListHandle.List);
        ConstructDescriptorSets(P.DescriptorPool, P.DescriptorLayout, P.DescriptorSet, CreateInfo.DescriptorListHandle.List);

        LayoutCreateInfo.setLayoutCount = 1;
        LayoutCreateInfo.pSetLayouts = &P.DescriptorLayout;
    }

    VkPushConstantRange PushConstantRange = {};
    if (CreateInfo.PushConstantHandle.pData != NULL) {
        PushConstantRange.offset = 0;
        PushConstantRange.stageFlags = CreateInfo.PushConstantHandle.Stage;
        PushConstantRange.size = CreateInfo.PushConstantHandle.Size;

        LayoutCreateInfo.pushConstantRangeCount = 1;
        LayoutCreateInfo.pPushConstantRanges = &PushConstantRange;
    }

    vkCreatePipelineLayout(GetLogicalDevice( ), &LayoutCreateInfo, 0, &P.Layout);

    std::vector<VkPipelineShaderStageCreateInfo> ShaderStageCreateInfo;
    ShaderStageCreateInfo.resize(CreateInfo.ShaderInfos.size());
    for (size_t i = 0; i < CreateInfo.ShaderInfos.size(); ++i) {
        ShaderStageCreateInfo[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        ShaderStageCreateInfo[i].stage = CreateInfo.ShaderInfos[i].Stage;
        ShaderStageCreateInfo[i].module = CreateInfo.ShaderInfos[i].Module;
        ShaderStageCreateInfo[i].pName = "main";
    }

    VkVertexInputBindingDescription inputBindingDescription = {};
    inputBindingDescription.binding = 0;
    inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    inputBindingDescription.stride = CreateInfo.VertexSize;

    std::vector<VkVertexInputAttributeDescription> inputAttributeDescriptions;

    inputAttributeDescriptions.resize(CreateInfo.VertexFormat.size());
    for (size_t i = 0; i < CreateInfo.VertexFormat.size(); ++i) {
        inputAttributeDescriptions[i].binding = 0;
        inputAttributeDescriptions[i].format = CreateInfo.VertexFormat[i].Format;
        inputAttributeDescriptions[i].location = i;
        inputAttributeDescriptions[i].offset = CreateInfo.VertexFormat[i].Offset;
    }

    VkPipelineVertexInputStateCreateInfo VertexInputStateInfo = {};

    VertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputStateInfo.vertexBindingDescriptionCount = 1;
    VertexInputStateInfo.pVertexBindingDescriptions = &inputBindingDescription;
    VertexInputStateInfo.vertexAttributeDescriptionCount = (uint32_t)inputAttributeDescriptions.size();
    VertexInputStateInfo.pVertexAttributeDescriptions = inputAttributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = CreateInfo.Topology;
    inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;

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
    rasterizationStateCreateInfo.polygonMode = CreateInfo.PolygonMode;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.lineWidth = CreateInfo.LineWidth;

    VkPipelineColorBlendAttachmentState colorBlendAttachementState = {};
    colorBlendAttachementState.blendEnable = VK_TRUE;
    colorBlendAttachementState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachementState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachementState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachementState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachementState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachementState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo = {};
    colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &colorBlendAttachementState;
    memset(colorBlendStateCreateInfo.blendConstants, 0, sizeof(float) * 4);

    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo = {};
    depthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo.depthTestEnable = VK_FALSE;
    depthStencilStateCreateInfo.depthWriteEnable = VK_FALSE;
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
    multisampleStateCreateInfo.rasterizationSamples = GetSampleCount( );
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = ShaderStageCreateInfo.size();
    graphicsPipelineCreateInfo.pStages = ShaderStageCreateInfo.data();
    graphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = P.Layout;
    graphicsPipelineCreateInfo.renderPass = GetRenderPass( );
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(GetLogicalDevice( ), 0, 1, &graphicsPipelineCreateInfo, 0, &P.Handle))
        return false;
    P.CreationData = CreateInfo;
    return true;
}

void DestroyPipeline(Pipeline& p)
{
    vkDestroyDescriptorSetLayout(GetLogicalDevice( ), p.DescriptorLayout, 0);
    vkDestroyDescriptorPool(GetLogicalDevice( ), p.DescriptorPool, 0);

    vkDestroyPipeline(GetLogicalDevice( ), p.Handle, 0);
    vkDestroyPipelineLayout(GetLogicalDevice( ), p.Layout, 0);
}

} // namespace Vulkan
} // namespace ffGraph