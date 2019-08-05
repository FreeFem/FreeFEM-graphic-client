#include <cstdio>
#include <cstring>
#include "utils.h"
#include "test_data.h"

bool createPipeline(const VkDevice device,
                    const VkRenderPass renderPass,
                    const VkPipelineLayout pipelineLayout,
                    const char *vertexShaderFileName,
                    const char *fragmentShaderFileName,
                    const uint32_t vertexInputBinding,
                    VkPipeline &outPipeline)
{
    VkResult result;

    VkShaderModule vertexModule, fragmentModule;
    bool b1, b2;
    b1 = loadAndCreateShaderModule(device, vertexShaderFileName, vertexModule);
    b2 = loadAndCreateShaderModule(device, fragmentShaderFileName, fragmentModule);

    if (!b1 || !b2) {
        dprintf(2, "Failed to load and create shaders.\n");
        return false;
    }

    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {{}, {}};
    shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo[0].module = vertexModule;
    shaderStageCreateInfo[0].pName = "main";
    shaderStageCreateInfo[0].pSpecializationInfo = 0;

    shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfo[1].module = fragmentModule;
    shaderStageCreateInfo[1].pName = "main";
    shaderStageCreateInfo[1].pSpecializationInfo = 0;

    VkVertexInputBindingDescription vertexInputBindingDescription = {};
    vertexInputBindingDescription.binding = vertexInputBinding;
    vertexInputBindingDescription.stride = sizeof(TriangleDemoVertex);
    vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription vertexInputAttributeDescription[2] = {{}, {}};
    vertexInputAttributeDescription[0].location = 0;
    vertexInputAttributeDescription[0].binding = vertexInputBinding;
    vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributeDescription[0].offset = offsetof(TriangleDemoVertex, x);

    vertexInputAttributeDescription[1].location = 1;
    vertexInputAttributeDescription[1].binding = vertexInputBinding;
    vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    vertexInputAttributeDescription[1].offset = offsetof(TriangleDemoVertex, r);

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateinfo = {};
    vertexInputStateCreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateinfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateinfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
    vertexInputStateCreateinfo.vertexAttributeDescriptionCount = 2;
    vertexInputStateCreateinfo.pVertexAttributeDescriptions = vertexInputAttributeDescription;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
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
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.lineWidth = 1;

    VkPipelineColorBlendAttachmentState colorBlendAttachementState = {};
    colorBlendAttachementState.blendEnable = VK_FALSE;
    colorBlendAttachementState.srcColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachementState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachementState.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachementState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

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
    graphicsPipelineCreateInfo.pStages = shaderStageCreateInfo;
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateinfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = pipelineLayout;
    graphicsPipelineCreateInfo.renderPass = renderPass;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    VkPipeline graphicsPipeline;
    result = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, 0, &graphicsPipeline);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to create pipeline. [%s]\n", VkResultToStr(result));
        return false;
    }
    vkDestroyShaderModule(device, vertexModule, 0);
    vkDestroyShaderModule(device, fragmentModule, 0);

    outPipeline = graphicsPipeline;
    return true;
}
