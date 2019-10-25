#include <imgui.h>
#include "Logger.h"
#include "ImGui_Impl.h"
#include "GlobalEnvironment.h"

namespace ffGraph {
namespace Vulkan {

static bool InitImGuiResource(UiPipeline& Node) {
    ImGuiIO& io = ImGui::GetIO( );

    unsigned char* fontData;
    int texWidth, texHeight = 0;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);
    VkDeviceSize UploadSize = texWidth * texHeight * 4 * sizeof(char);

    ImageCreateInfo Info = {};
    Info.AsView = true;
    Info.Extent.width = texWidth;
    Info.Extent.height = texHeight;

    Info.Format = VK_FORMAT_R8G8B8A8_UNORM;
    Info.SampleCount = VK_SAMPLE_COUNT_1_BIT;
    Info.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    Info.Usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    Info.ViewInfos.AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    Info.QueueIndexCount = 1;
    Info.QueueIndices[0] = GetGraphicQueueIndex( );

    VmaAllocationCreateInfo VmaInfo = {};
    VmaInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    VmaInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    VmaInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    Node.FontImage = CreateImage(GetAllocator( ), GetLogicalDevice( ), Info, VmaInfo);

    BufferCreateInfo bInfo = {};
    bInfo.vkData.Size = UploadSize;
    bInfo.vkData.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bInfo.vkData.Usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

    bInfo.vmaData.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    bInfo.vmaData.Usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    bInfo.vmaData.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

    Buffer StagingBuffer = CreateBuffer(GetAllocator( ), bInfo);

    memcpy(StagingBuffer.Infos.pMappedData, fontData, UploadSize);

    VkCommandBuffer copyCmd;
    VkCommandBufferAllocateInfo cmdAllocInfo = {};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandPool = GetCommandPool( );
    cmdAllocInfo.commandBufferCount = 1;
    vkAllocateCommandBuffers(GetLogicalDevice( ), &cmdAllocInfo, &copyCmd);

    VkCommandBufferBeginInfo BeginInfo = {};
    BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(copyCmd, &BeginInfo);

    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier.image = Node.FontImage.Handle;
    imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier.subresourceRange.levelCount = 1;
    imageMemoryBarrier.subresourceRange.layerCount = 1;
    imageMemoryBarrier.srcAccessMask = 0;
    imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, 0, 0, 0, 1,
                         &imageMemoryBarrier);

    VkBufferImageCopy bufferCopyRegion = {};
    bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferCopyRegion.imageSubresource.layerCount = 1;
    bufferCopyRegion.imageExtent.width = texWidth;
    bufferCopyRegion.imageExtent.height = texHeight;
    bufferCopyRegion.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(copyCmd, StagingBuffer.Handle, Node.FontImage.Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &bufferCopyRegion);

    VkImageMemoryBarrier imageMemoryBarrier2 = {};
    imageMemoryBarrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    imageMemoryBarrier2.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageMemoryBarrier2.image = Node.FontImage.Handle;
    imageMemoryBarrier2.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageMemoryBarrier2.subresourceRange.levelCount = 1;
    imageMemoryBarrier2.subresourceRange.layerCount = 1;
    imageMemoryBarrier2.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    imageMemoryBarrier2.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(copyCmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, 0, 0, 0,
                         1, &imageMemoryBarrier2);

    vkEndCommandBuffer(copyCmd);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    VkFence fence;
    vkCreateFence(GetLogicalDevice( ), &fenceInfo, 0, &fence);
    vkQueueSubmit(GetGraphicQueue( ), 1, &submitInfo, fence);
    vkWaitForFences(GetLogicalDevice( ), 1, &fence, VK_TRUE, UINT64_MAX);

    vkDestroyFence(GetLogicalDevice( ), fence, 0);
    vkFreeCommandBuffers(GetLogicalDevice( ), GetCommandPool( ), 1, &copyCmd);

    DestroyBuffer(GetAllocator( ), StagingBuffer);

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    vkCreateSampler(GetLogicalDevice( ), &samplerInfo, 0, &Node.ImGuiSampler);
}

void UpdateUiPipeline(UiPipeline& Node) {
    vkDeviceWaitIdle(GetLogicalDevice( ));
    ImDrawData* imDrawData = ImGui::GetDrawData( );

    // Note: Alignment is done inside buffer creation
    if (imDrawData == 0) return;
    VkDeviceSize vertexBufferSize = imDrawData->TotalVtxCount * sizeof(ImDrawVert);
    VkDeviceSize indexBufferSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);

    if ((vertexBufferSize == 0) || (indexBufferSize == 0)) {
        return;
    }

    // Update buffers only if vertex or index count has been changed compared to current buffer size

    // Vertex buffer
    if ((Node.ImGuiVertices.Handle == VK_NULL_HANDLE) || (Node.VertexCount != imDrawData->TotalVtxCount)) {
        DestroyBuffer(GetAllocator( ), Node.ImGuiVertices);

        BufferCreateInfo vInfo = {};
        vInfo.vkData.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        vInfo.vkData.Size = vertexBufferSize;
        vInfo.vkData.SharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vInfo.vmaData.Usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        vInfo.vmaData.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        vInfo.vmaData.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        Node.ImGuiVertices = CreateBuffer(GetAllocator( ), vInfo);
        Node.VertexCount = imDrawData->TotalVtxCount;
    }

    // Index buffer
    VkDeviceSize indexSize = imDrawData->TotalIdxCount * sizeof(ImDrawIdx);
    if ((Node.ImGuiIndices.Handle == VK_NULL_HANDLE) || (Node.IndexCount < imDrawData->TotalIdxCount)) {
        DestroyBuffer(GetAllocator( ), Node.ImGuiIndices);

        BufferCreateInfo vInfo = {};
        vInfo.vkData.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        vInfo.vkData.Size = indexBufferSize;
        vInfo.vkData.SharingMode = VK_SHARING_MODE_EXCLUSIVE;

        vInfo.vmaData.Usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        vInfo.vmaData.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
        vInfo.vmaData.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

        Node.ImGuiIndices = CreateBuffer(GetAllocator( ), vInfo);

        Node.IndexCount = imDrawData->TotalIdxCount;
    }

    // Upload data
    ImDrawVert* vtxDst = (ImDrawVert*)Node.ImGuiVertices.Infos.pMappedData;
    ImDrawIdx* idxDst = (ImDrawIdx*)Node.ImGuiIndices.Infos.pMappedData;

    for (int n = 0; n < imDrawData->CmdListsCount; n++) {
        const ImDrawList* cmd_list = imDrawData->CmdLists[n];
        memcpy(vtxDst, cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size * sizeof(ImDrawVert));
        memcpy(idxDst, cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx));
        vtxDst += cmd_list->VtxBuffer.Size;
        idxDst += cmd_list->IdxBuffer.Size;
    }

    // Flush to make writes visible to GPU
    VkMappedMemoryRange mappedRange[2] = {};
    mappedRange[0].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange[0].memory = Node.ImGuiVertices.Infos.deviceMemory;
    mappedRange[0].offset = 0;
    mappedRange[0].size = VK_WHOLE_SIZE;

    mappedRange[1].sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange[1].memory = Node.ImGuiIndices.Infos.deviceMemory;
    mappedRange[1].offset = 0;
    mappedRange[1].size = VK_WHOLE_SIZE;
    vkFlushMappedMemoryRanges(GetLogicalDevice( ), 2, mappedRange);
}

static bool CreateDescriptorPool(UiPipeline& Node) {
    Node.DescriptorPool = VK_NULL_HANDLE;
    VkDescriptorPoolSize PoolSize[1] = {};
    PoolSize[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    PoolSize[0].descriptorCount = 1;

    VkDescriptorPoolCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    CreateInfo.poolSizeCount = 1;
    CreateInfo.pPoolSizes = PoolSize;
    CreateInfo.maxSets = 1;

    if (vkCreateDescriptorPool(GetLogicalDevice(), &CreateInfo, 0, &Node.DescriptorPool)) return false;
    return true;
}

static bool CreateDescriptorSetLayout(UiPipeline& Node) {
    VkDescriptorSetLayoutBinding Binding = {};
    Binding.binding = 0;
    Binding.descriptorCount = 1;
    Binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    Binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo CreateInfo = {};
    CreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    CreateInfo.bindingCount = 1;
    CreateInfo.pBindings = &Binding;

    if (GetLogicalDevice() == VK_NULL_HANDLE)
        LogError("", "Device not init.\n");
    if (vkCreateDescriptorSetLayout(GetLogicalDevice(), &CreateInfo, 0, &Node.DescriptorSetLayout)) return false;

    VkDescriptorSetAllocateInfo AllocInfo = {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocInfo.descriptorPool = Node.DescriptorPool;
    AllocInfo.descriptorSetCount = 1;
    AllocInfo.pSetLayouts = &Node.DescriptorSetLayout;

    if (vkAllocateDescriptorSets(GetLogicalDevice(), &AllocInfo, &Node.DescriptorSet)) return false;

    VkWriteDescriptorSet WriteDescriptors = {};

    WriteDescriptors.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    WriteDescriptors.descriptorCount = 1;
    WriteDescriptors.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    WriteDescriptors.dstSet = Node.DescriptorSet;

    VkDescriptorImageInfo iInfo = {};
    iInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    iInfo.imageView = Node.FontImage.View;
    iInfo.sampler = Node.ImGuiSampler;
    WriteDescriptors.pImageInfo = &iInfo;
    vkUpdateDescriptorSets(GetLogicalDevice(), 1, &WriteDescriptors, 0, 0);

    return true;
}

UiPipeline NewUiPipeline(const VkShaderModule Shaders[2]) {
    UiPipeline Node;
    InitImGuiResource(Node);
    if (Node.FontImage.View == VK_NULL_HANDLE)
        LogError("", "\n\n View is Null \n\n");
    CreateDescriptorPool(Node);
    CreateDescriptorSetLayout(Node);

    VkPushConstantRange PushConstantRange = {};
    PushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    PushConstantRange.offset = 0;
    PushConstantRange.size = sizeof(UiPipeline::ImGuiPushConst);

    // Building VkPipeline
    VkPipelineLayoutCreateInfo PipelineLayoutInfo = {};
    PipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    PipelineLayoutInfo.pushConstantRangeCount = 1;
    PipelineLayoutInfo.pPushConstantRanges = &PushConstantRange;
    PipelineLayoutInfo.setLayoutCount = 1;
    PipelineLayoutInfo.pSetLayouts = &Node.DescriptorSetLayout;

    vkCreatePipelineLayout(GetLogicalDevice(), &PipelineLayoutInfo, 0, &Node.Layout);

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
    inputBindingDescription.binding = 0;
    inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    inputBindingDescription.stride = sizeof(ImDrawVert);
    vertexInputBindingDescription.push_back(inputBindingDescription);

    VkVertexInputAttributeDescription inputAttributeDescription[3] = {};
    inputAttributeDescription[0].binding = 0;
    inputAttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
    inputAttributeDescription[0].location = 0;
    inputAttributeDescription[0].offset = offsetof(ImDrawVert, pos);

    inputAttributeDescription[1].binding = 0;
    inputAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
    inputAttributeDescription[1].location = 1;
    inputAttributeDescription[1].offset = offsetof(ImDrawVert, uv);

    inputAttributeDescription[2].binding = 0;
    inputAttributeDescription[2].format = VK_FORMAT_R8G8B8A8_UNORM;
    inputAttributeDescription[2].location = 2;
    inputAttributeDescription[2].offset = offsetof(ImDrawVert, col);

    VkPipelineVertexInputStateCreateInfo VertexInputStateInfo = {};
    VertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    VertexInputStateInfo.vertexBindingDescriptionCount = (uint32_t)vertexInputBindingDescription.size( );
    VertexInputStateInfo.pVertexBindingDescriptions = vertexInputBindingDescription.data( );
    VertexInputStateInfo.vertexAttributeDescriptionCount = (uint32_t)3;
    VertexInputStateInfo.pVertexAttributeDescriptions = inputAttributeDescription;

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
    rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;

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
    graphicsPipelineCreateInfo.renderPass = GetRenderPass( );
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(GetLogicalDevice( ), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, 0,
                                  &Node.Handle)) {
        LogError(GetCurrentLogLocation( ), "Failed to create VkPipeline.");
        return Node;
    }
    memcpy(Node.Modules, Shaders, sizeof(VkShaderModule) * 2);
    return Node;
}

void DestroyUiPipeline(UiPipeline& n)
{
    vkDestroyDescriptorSetLayout(GetLogicalDevice(), n.DescriptorSetLayout, 0);
    vkDestroyDescriptorPool(GetLogicalDevice(), n.DescriptorPool, 0);

    vkDestroyPipeline(GetLogicalDevice(), n.Handle, 0);
    vkDestroyPipelineLayout(GetLogicalDevice(), n.Layout, 0);

    DestroyBuffer(GetAllocator(), n.ImGuiVertices);
    DestroyBuffer(GetAllocator(), n.ImGuiIndices);
    DestroyImage(GetAllocator(), GetLogicalDevice(), n.FontImage);
    vkDestroySampler(GetLogicalDevice(), n.ImGuiSampler, 0);
}

}
}