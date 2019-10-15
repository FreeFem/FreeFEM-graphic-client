#include "RenderGraph.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static RenderGraphNode FillRenderGraphNode(JSON::SceneObject& Obj) {
    RenderGraphNode Node;

    Node.GeoType = Obj.GeoType;
    Node.BatchDimension = Obj.DataType;
    Node.to_render = (Obj.GeoType == JSON::GeometryType::Volume) ? false : true;
    Node.CPUMeshData = newBatch(Obj);
    Node.LineWidth = Obj.LineWidth;

    return Node;
}

RenderGraphNode FillRenderGraphNode(Array& Data, JSON::GeometryType GeoType, JSON::Dimension n, int LineWidth) {
    RenderGraphNode Node;

    Node.Layout = VK_NULL_HANDLE;
    Node.Handle = VK_NULL_HANDLE;

    Node.GeoType = GeoType;
    Node.BatchDimension = n;
    Node.to_render = (Node.GeoType == JSON::GeometryType::Volume) ? false : true;
    Node.CPUMeshData = newBatch(
        {Data}, (n == JSON::Dimension::Mesh2D) ? VK_PRIMITIVE_TOPOLOGY_LINE_LIST : VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
    Node.LineWidth = LineWidth;

    return Node;
}

static uint32_t CountDescriptor(std::vector<DescriptorInfos> Infos, VkDescriptorType Type)
{
    uint32_t count = 0;

    for (size_t i = 0; i < Infos.size(); ++i) {
        if (Infos[i].Type == Type)
            ++count;
    }
    return count;
}

static bool CreateDescriptorPool(RenderGraphCreateInfos CreateInfos, RenderGraphNode& Node)
{
    Node.DescriptorPool = VK_NULL_HANDLE;
    if (CreateInfos.Descriptors.size() == 0)
        return true;
    VkDescriptorPoolSize PoolSize[2] = {};
    PoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    PoolSize[0].descriptorCount = CountDescriptor(CreateInfos.Descriptors, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);

    PoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    PoolSize[1].descriptorCount = CountDescriptor(CreateInfos.Descriptors, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    VkDescriptorPoolCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    CreateInfo.poolSizeCount = 1;
    CreateInfo.pPoolSizes = PoolSize;
    CreateInfo.maxSets = CreateInfos.Descriptors.size();

    if (vkCreateDescriptorPool(CreateInfos.Device, &CreateInfo, 0, &Node.DescriptorPool))
        return false;
    return true;
}

static bool CreateDescriptorSetLayout(RenderGraphCreateInfos CreateInfos, RenderGraphNode& Node)
{
    if (CreateInfos.Descriptors.size() == 0)
        return true;
    std::vector<VkDescriptorSetLayoutBinding> Binding;
    Binding.resize(CreateInfos.Descriptors.size());
    for (size_t i = 0; i < CreateInfos.Descriptors.size(); ++i) {
        Binding[i].binding = i;
        Binding[i].descriptorType = CreateInfos.Descriptors[i].Type;
        Binding[i].descriptorCount = 1;
        Binding[i].stageFlags = CreateInfos.Descriptors[i].Stage;
    }

    VkDescriptorSetLayoutCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    CreateInfo.bindingCount = Binding.size();
    CreateInfo.pBindings = Binding.data();

    if (vkCreateDescriptorSetLayout(CreateInfos.Device, &CreateInfo, 0, &Node.DescriptorSetLayout))
        return false;

    VkDescriptorSetAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocInfo.descriptorPool = Node.DescriptorPool;
    AllocInfo.descriptorSetCount = 1;
    AllocInfo.pSetLayouts = &Node.DescriptorSetLayout;

    if (vkAllocateDescriptorSets(CreateInfos.Device, &AllocInfo, &Node.DescriptorSet))
        return false;

    std::vector<VkWriteDescriptorSet> WriteDescriptors;
    WriteDescriptors.resize(CreateInfos.Descriptors.size());
    std::vector<VkDescriptorBufferInfo> BufferInfos;
    BufferInfos.resize(CountDescriptor(CreateInfos.Descriptors, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER));
    std::vector<VkDescriptorImageInfo> ImageInfos;
    ImageInfos.resize(CountDescriptor(CreateInfos.Descriptors, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER));
    uint32_t BCount = 0;
    uint32_t ICount = 0;

    for (size_t i = 0; i < CreateInfos.Descriptors.size(); ++i) {
        WriteDescriptors[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        WriteDescriptors[i].descriptorCount = CreateInfos.Descriptors.size();
        WriteDescriptors[i].descriptorType = CreateInfos.Descriptors[i].Type;
        WriteDescriptors[i].dstSet = Node.DescriptorSet;
        if (CreateInfos.Descriptors[i].Type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
            VkDescriptorBufferInfo bInfo = {};
            bInfo.buffer = *CreateInfos.Descriptors[i].Uniform;
            bInfo.offset = 0;
            bInfo.range = CreateInfos.Descriptors[i].Size;
            BufferInfos[BCount] = bInfo;
            WriteDescriptors[i].pBufferInfo = &BufferInfos[i];
            BCount += 1;
        } else if (CreateInfos.Descriptors[i].Type == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER) {
            VkDescriptorImageInfo iInfo = {};
            iInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            iInfo.imageView = *CreateInfos.Descriptors[i].View;
            iInfo.sampler = *CreateInfos.Descriptors[i].Sampler;
            ImageInfos[ICount] = iInfo;
            WriteDescriptors[i].pImageInfo = &ImageInfos[ICount];
            ICount += 1;
        }
    }

    vkUpdateDescriptorSets(CreateInfos.Device, WriteDescriptors.size(), WriteDescriptors.data(), 0, 0);
    return true;
}

static RenderGraphNode ConstructRenderGraphNode(RenderGraphCreateInfos CreateInfos, const VkShaderModule Shaders[2], RenderGraphNode& Node) {
    CreateDescriptorPool(CreateInfos, Node);
    CreateDescriptorSetLayout(CreateInfos, Node);

    VkPushConstantRange PushConstantRange = {};
    PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    PushConstantRange.offset = 0;
    PushConstantRange.size = sizeof(CameraUniform);

    // Building VkPipeline
    VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.pushConstantRangeCount = 1;
    PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;
    if (CreateInfos.Descriptors.size() != 0) {
        PipelineLayoutInfo.setLayoutCount = 1;
        PipelineLayoutInfo.pSetLayouts = &Node.DescriptorSetLayout;
    }

    vkCreatePipelineLayout(CreateInfos.Device, &PipelineLayoutInfo, 0, &Node.Layout);

    VkPipelineShaderStageCreateInfo ShaderStageInfo[2] = {};
    ShaderStageInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStageInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    ShaderStageInfo[0].module = Shaders[0];
    ShaderStageInfo[0].pName = "main";

    ShaderStageInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    ShaderStageInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    ShaderStageInfo[1].module = Shaders[1];
    ShaderStageInfo[1].pName = "main";

    int BindingInput = 0;
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription = {};

    VkVertexInputBindingDescription inputBindingDescription;
    VkVertexInputAttributeDescription inputAttributeDescription[2] = {};
    if (Node.BatchDimension == JSON::Dimension::Mesh2D) {
        inputBindingDescription.binding = BindingInput;
        inputBindingDescription.stride = sizeof(float) * 6;
        inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputBindingDescription.push_back(inputBindingDescription);

        inputAttributeDescription[0].binding = 0;
        inputAttributeDescription[0].location = 0;
        inputAttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
        inputAttributeDescription[0].offset = sizeof(float) * 0;

        inputAttributeDescription[1].binding = 0;
        inputAttributeDescription[1].location = 1;
        inputAttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        inputAttributeDescription[1].offset = sizeof(float) * 2;

    } else {
        inputBindingDescription.binding = BindingInput;
        inputBindingDescription.stride = sizeof(float) * 7;
        inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputBindingDescription.push_back(inputBindingDescription);

        inputAttributeDescription[0].binding = 0;
        inputAttributeDescription[0].location = 0;
        inputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        inputAttributeDescription[0].offset = sizeof(float) * 0;

        inputAttributeDescription[1].binding = 0;
        inputAttributeDescription[1].location = 1;
        inputAttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        inputAttributeDescription[1].offset = sizeof(float) * 3;
    }

    VkPipelineVertexInputStateCreateInfo VertexInputStateInfo = {};
    VertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputStateInfo.vertexBindingDescriptionCount = (uint32_t)vertexInputBindingDescription.size( );
    VertexInputStateInfo.pVertexBindingDescriptions = vertexInputBindingDescription.data( );
    VertexInputStateInfo.vertexAttributeDescriptionCount = (uint32_t)2;
    VertexInputStateInfo.pVertexAttributeDescriptions = inputAttributeDescription;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = Node.CPUMeshData.Topology;
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
    rasterizationStateCreateInfo.polygonMode = Node.PolygonMode;
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
    rasterizationStateCreateInfo.lineWidth = Node.LineWidth;

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
    multisampleStateCreateInfo.rasterizationSamples = CreateInfos.msaaSamples;
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
    graphicsPipelineCreateInfo.layout = Node.Layout;
    graphicsPipelineCreateInfo.renderPass = CreateInfos.RenderPass;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(CreateInfos.Device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, 0,
                                  &Node.Handle)) {
        LogError(GetCurrentLogLocation( ), "Failed to create VkPipeline.");
        return Node;
    }
    memcpy(Node.Modules, Shaders, sizeof(VkShaderModule) * 2);
    return Node;
}

RenderGraph ConstructRenderGraph(RenderGraphCreateInfos CreateInfos, const VmaAllocator& Allocator,
                                 JSON::SceneLayout& Layout, const ShaderLibrary& Shaders) {
    RenderGraph n;

    uint32_t sMax = 0;
    JSON::Dimension Dim;
    for (auto& obj : Layout.MeshArrays) {
        RenderGraphNode tmp = FillRenderGraphNode(obj);
        Dim = tmp.BatchDimension;

        VkShaderModule Modules[2] = {
            FindShader(Shaders, (Dim == JSON::Dimension::Mesh2D) ? "Geo2D.vert" : "Geo3D.vert"),
            FindShader(Shaders, "Color.frag")};
        n.Nodes.push_back(ConstructRenderGraphNode(CreateInfos, Modules, tmp));

        sMax = std::max(
            sMax, (uint32_t)(tmp.CPUMeshData.BatchedMeshes.ElementCount * tmp.CPUMeshData.BatchedMeshes.ElementSize));
    }
    ConstructUiNode(CreateInfos, Shaders, n.UiNode);
    BufferCreateInfo bCreateInfo = {};
    bCreateInfo.vkData.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bCreateInfo.vkData.Size = sMax;
    bCreateInfo.vkData.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

    bCreateInfo.vmaData.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    bCreateInfo.vmaData.Usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    n.PushBuffer = CreateBuffer(Allocator, bCreateInfo);

    if (n.PushBuffer.Handle == VK_NULL_HANDLE) return n;

    n.Layout = Layout;
    n.PushCamera.Model = glm::mat4(1.0f);
    n.PushCamera.ViewProj = glm::mat4(1.0f);
    InitCameraController(n.Cam, CreateInfos.AspectRatio, 90.f, (Dim == JSON::Dimension::Mesh2D) ? CameraType::_2D : CameraType::_3D);
    return n;
}

void DestroyRenderGraph(const VkDevice& Device, const VmaAllocator& Allocator, RenderGraph Graph) {
    for (auto& Node : Graph.Nodes) {
        vkDestroyPipeline(Device, Node.Handle, 0);
        vkDestroyPipelineLayout(Device, Node.Layout, 0);
        vkDestroyDescriptorSetLayout(Device, Node.DescriptorSetLayout, 0);
        vkDestroyDescriptorPool(Device, Node.DescriptorPool, 0);
        DestroyBatch(Node.CPUMeshData);
    }
    DestroyUiNode(Graph.UiNode);
    DestroyBuffer(Allocator, Graph.PushBuffer);
}

void ReloadRenderGraph(RenderGraphCreateInfos CreateInfos, RenderGraph& Graph) {
    if (!Graph.Update) return;
    vkDeviceWaitIdle(CreateInfos.Device);
    for (auto& Node : Graph.Nodes) {
        if (Node.Update) {
            if (Node.Handle != VK_NULL_HANDLE && Node.Layout != VK_NULL_HANDLE) {
                vkDestroyPipeline(CreateInfos.Device, Node.Handle, 0);
                vkDestroyPipelineLayout(CreateInfos.Device, Node.Layout, 0);
                Node.Handle = VK_NULL_HANDLE;
                Node.Layout = VK_NULL_HANDLE;
            }
            ConstructRenderGraphNode(CreateInfos, Node.Modules, Node);
            Node.Update = false;
        }
    }
    ReloadUINode(CreateInfos, Graph.UiNode);
    Graph.Update = false;
}

}    // namespace Vulkan
}    // namespace ffGraph