#include "GlobalEnvironment.h"
#include "Pipeline.h"
#include "Resource/Camera/CameraController.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

bool CreatePipeline(uint32_t Flags, std::vector<Pipeline>& Pipelines, VkShaderModule Shaders[2])
{
    Pipeline n;

    for (auto& P : Pipelines) {
        if (P.ID == Flags)
            return true;
    }
    n.ID = Flags;

    VkPushConstantRange PushConstantRange = {};
    PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    PushConstantRange.offset = 0;
    PushConstantRange.size = sizeof(CameraUniform);

    VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.pushConstantRangeCount = 1;
    PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;

    if (vkCreatePipelineLayout(GetLogicalDevice( ), &PipelineLayoutInfo, 0, &n.Layout))
        return false;

    VkPipelineShaderStageCreateInfo ShaderStageInfo[2] = {};
    ShaderStageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    ShaderStageInfo[0].module = Shaders[Pipeline::ShaderVertexStage];
    ShaderStageInfo[0].pName = "main";


    ShaderStageInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ShaderStageInfo[1].module = Shaders[Pipeline::ShaderFragmentStage];
    ShaderStageInfo[1].pName = "main";

    memcpy(n.ShaderModule, Shaders, sizeof(VkShaderModule) * 2);

    VkVertexInputBindingDescription BindingDescription = {};
    VkVertexInputAttributeDescription AttributeDescription[2] = {};

    BindingDescription.binding = 0;
    BindingDescription.stride = sizeof(float) * 7;
    BindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    AttributeDescription[0].binding = 0;
    AttributeDescription[0].location = 0;
    AttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    AttributeDescription[0].offset = sizeof(float) * 0;

    AttributeDescription[1].binding = 0;
    AttributeDescription[1].location = 1;
    AttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    AttributeDescription[1].offset = sizeof(float) * 3;

    VkPipelineVertexInputStateCreateInfo VertexInputStateInfo = {};
    VertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VertexInputStateInfo.vertexBindingDescriptionCount = 1;
    VertexInputStateInfo.pVertexBindingDescriptions = &BindingDescription;

    VertexInputStateInfo.vertexAttributeDescriptionCount = 2;
    VertexInputStateInfo.pVertexAttributeDescriptions = AttributeDescription;

    VkPrimitiveTopology Topo = (Flags & PipelineTopology::LINE_LIST) ?
                                    VK_PRIMITIVE_TOPOLOGY_LINE_LIST :
                                    (Flags & PipelineTopology::POINT_LIST) ?
                                        VK_PRIMITIVE_TOPOLOGY_POINT_LIST :
                                        VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = Topo;
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

    VkPolygonMode PolyMode = (Flags & PipelinePolygonMode::MODE_FILL) ?
                                VK_POLYGON_MODE_FILL :
                                (Flags & PipelinePolygonMode::MODE_LINE) ?
                                    VK_POLYGON_MODE_LINE :
                                    VK_POLYGON_MODE_POINT;

    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo = {};
    rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo.polygonMode = PolyMode;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
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
    colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_COPY;
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
    multisampleStateCreateInfo.rasterizationSamples = GetSampleCount( );
    multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;

    VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {};
    graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    graphicsPipelineCreateInfo.stageCount = 2;
    graphicsPipelineCreateInfo.pStages = ShaderStageInfo;
    graphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = n.Layout;
    graphicsPipelineCreateInfo.renderPass = GetRenderPass( );
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(GetLogicalDevice( ), 0, 1, &graphicsPipelineCreateInfo, 0, &n.Handle))
        return false;
    Pipelines.push_back(n);
    return true;
}

void DestroyPipeline(Pipeline& P)
{
    vkDestroyPipelineLayout(GetLogicalDevice( ), P.Layout, 0);
    vkDestroyPipeline(GetLogicalDevice( ), P.Handle, 0);
}

}
} // namespace ffGraph