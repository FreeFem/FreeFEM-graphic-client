#include "GraphConstructor.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static RenderGraphNode ConstructRenderGraphNode(const VkDevice& Device, const VkRenderPass& Renderpass, const VmaAllocator& Allocator, JSON::SceneObject& Obj, const VkShaderModule Modules[2])
{
    RenderGraphNode Node;

    Node.Topology = (VkPrimitiveTopology)Obj.RenderPrimitive;
    Node.Meshes.resize(Obj.Data.size());
    for (int i = 0; i < (int)Obj.Data.size(); ++i) {
        Node.Meshes[i] = newMesh(Allocator, Obj, i);
    }

    VkPushConstantRange PushConstantRange = {};
    PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    PushConstantRange.offset = 0;
    PushConstantRange.size = sizeof(CameraUniform);

    // Building VkPipeline
    VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.pushConstantRangeCount = 1;
    PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;

    vkCreatePipelineLayout(Device, &PipelineLayoutInfo, 0, &Node.Layout);

    VkPipelineShaderStageCreateInfo ShaderStageInfo[2] = {};
    ShaderStageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    ShaderStageInfo[0].module = Modules[0];
    ShaderStageInfo[0].pName = "main";

    ShaderStageInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ShaderStageInfo[1].module = Modules[1];
    ShaderStageInfo[1].pName = "main";

    int BindingInput = 0;
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription = {};
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription = {};
    int location = 0;
    for (const auto& vBuffer : Node.Meshes) {
        VkVertexInputBindingDescription inputBindingDescription;
        inputBindingDescription.binding = BindingInput;
        inputBindingDescription.stride = sizeof(float) * 7;
        inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputBindingDescription.push_back(inputBindingDescription);

        BindingInput += 1;
    }
    VkVertexInputAttributeDescription inputAttributeDescription[2] = {};
    inputAttributeDescription[0].binding = 0;
    inputAttributeDescription[0].location = 0;
    inputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    inputAttributeDescription[0].offset = sizeof(float) * 0;

    inputAttributeDescription[1].binding = 0;
    inputAttributeDescription[1].location = 1;
    inputAttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    inputAttributeDescription[1].offset = sizeof(float) * 3;

    vertexInputAttributeDescription.push_back(inputAttributeDescription[0]);
    vertexInputAttributeDescription.push_back(inputAttributeDescription[1]);

    VkPipelineVertexInputStateCreateInfo VertexInputStateInfo = {};
    VertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputStateInfo.vertexBindingDescriptionCount = (uint32_t)vertexInputBindingDescription.size();
    VertexInputStateInfo.pVertexBindingDescriptions = vertexInputBindingDescription.data();
    VertexInputStateInfo.vertexAttributeDescriptionCount = (uint32_t)vertexInputAttributeDescription.size();
    VertexInputStateInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = Node.Topology;
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
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.lineWidth = Obj.LineWidth;

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
    graphicsPipelineCreateInfo.pStages = ShaderStageInfo;
    graphicsPipelineCreateInfo.pVertexInputState = &VertexInputStateInfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = Node.Layout;
    graphicsPipelineCreateInfo.renderPass = Renderpass;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(Device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, 0, &Node.Handle)) {
        LogError(GetCurrentLogLocation(), "Failed to create VkPipeline.");
        return Node;
    }
    Node.GPUBuffers.resize(Node.Meshes.size());
    for (uint32_t i = 0; i < Node.Meshes.size(); ++i) {
        Node.GPUBuffers[i] = Node.Meshes[i].GPUBuffer;
    }
    return Node;
}

RenderGraph ConstructRenderGraph(const VkDevice& Device, const VkRenderPass& Renderpass, const VmaAllocator& Allocator, JSON::SceneLayout& Layout, const VkShaderModule Modules[2])
{
    RenderGraph n;

    for (auto& obj : Layout.MeshArrays) {
        n.Nodes.push_back(ConstructRenderGraphNode(Device, Renderpass, Allocator, obj, Modules));
    }
    n.Cam = InitCamera(1280.f / 768.f, false);
    CameraResetPositionAndZoom(n.Cam);
    n.PushCamera.Model = glm::mat4(1.0f);
    n.PushCamera.ViewProj = glm::mat4(1.0f);
    return n;
}

void DestroyRenderGraph(const VkDevice& Device, const VmaAllocator& Allocator, RenderGraph Graph)
{
    for (auto& Node : Graph.Nodes) {
        vkDestroyPipeline(Device, Node.Handle, 0);
        vkDestroyPipelineLayout(Device, Node.Layout, 0);
        for (auto& M : Node.Meshes) {
            DestroyMesh(Allocator, M);
        }
    }
}

}
}