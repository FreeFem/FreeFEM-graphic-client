#include "Pipeline.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include "GraphContext.h"
#include "GraphManager.h"

namespace gr {
Error Pipeline::init(const Manager& grm, const Context& grc) {
    if (initRenderpass(grm, grc)) return Error::FUNCTION_FAILED;
    if (initFramebuffers(grm, grc)) return Error::FUNCTION_FAILED;
    if (initDescriptorSetLayout(grm, grc)) return Error::FUNCTION_FAILED;
    if (initPipeline(grm, grc)) return Error::FUNCTION_FAILED;
    return Error::NONE;
}

Error Pipeline::initShaders(const VkDevice& device, const char* vertexShaderFilename,
                            const char* fragmentShaderFilename) {
    if (m_shaders.init(device, vertexShaderFilename, fragmentShaderFilename)) return Error::FUNCTION_FAILED;
    return Error::NONE;
}

Error Pipeline::reload(const Manager& grm, const Context& grc) {
    if (initRenderpass(grm, grc)) return Error::FUNCTION_FAILED;
    if (initFramebuffers(grm, grc)) return Error::FUNCTION_FAILED;
    if (initPipeline(grm, grc)) return Error::FUNCTION_FAILED;
    return Error::NONE;
}

Error Pipeline::initRenderpass(const Manager& grm, const Context& grc) {
    VkAttachmentDescription attachmentDescription[2] = {{}, {}};
    attachmentDescription[0].flags = 0;
    attachmentDescription[0].format = grc.getSurfaceFormat( );
    attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachmentDescription[1].flags = 0;
    attachmentDescription[1].format = grc.getDepthBufferFormat( );
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

    if (vkCreateRenderPass(grm.getDevice( ), &createInfo, 0, &m_renderpass) != VK_SUCCESS)
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

Error Pipeline::initFramebuffers(const Manager& grm, const Context& grc) {
    for (const auto view : grc.getSwapImageViews( )) {
        VkFramebuffer fb;

        VkImageView attachment[2] = {view, grc.getDepthImage( ).getImageView( )};
        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = 0;
        createInfo.flags = 0;
        createInfo.renderPass = m_renderpass;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = attachment;
        createInfo.width = grm.getNativeWindow( ).getWidth( );
        createInfo.height = grm.getNativeWindow( ).getHeight( );
        createInfo.layers = 1;

        vkCreateFramebuffer(grm.getDevice( ), &createInfo, 0, &fb);
        m_framebuffers.push_back(fb);
    }
    return Error::NONE;
}

const char* VERTEX_SHADER_FILENAME = "";
const char* FRAGMENT_SHADER_FILENAME = "";

Error Pipeline::initPipeline(const Manager& grm, UNUSED_PARAM const Context& grc) {
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = 0;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &m_descriptorSetLayout;

    vkCreatePipelineLayout(grm.getDevice( ), &pipelineLayoutCreateInfo, 0, &m_layout);

    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {{}, {}};
    shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo[0].module = m_shaders.getVertexModule( );
    shaderStageCreateInfo[0].pName = "main";
    shaderStageCreateInfo[0].pSpecializationInfo = 0;

    shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfo[1].module = m_shaders.getFragmentModule( );
    shaderStageCreateInfo[1].pName = "main";
    shaderStageCreateInfo[1].pSpecializationInfo = 0;

    int binding_input = 0;
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription = {};
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription = {};
    VkPrimitiveTopology topology;
    printf("Number of Buffer to render : %lu\n", m_vertexBuffers.size( ));
    for (const auto vBuffer : m_vertexBuffers) {
        VkVertexInputBindingDescription inputBindingDescription;
        topology = vBuffer.getTopology( );
        inputBindingDescription.binding = binding_input;
        inputBindingDescription.stride = vBuffer.getStride( );
        inputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        vertexInputBindingDescription.push_back(inputBindingDescription);

        int location = 0;
        for (const auto vDesciptor : vBuffer.getAttributes( )) {
            VkVertexInputAttributeDescription inputAttributeDescription = {};
            inputAttributeDescription.location = location;
            inputAttributeDescription.binding = binding_input;
            inputAttributeDescription.format = vDesciptor.format;
            inputAttributeDescription.offset = vDesciptor.offset;
            vertexInputAttributeDescription.push_back(inputAttributeDescription);
            location += 1;
        }

        printf("Binding point : %d\n", binding_input);
        binding_input += 1;
    }

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateinfo = {};
    vertexInputStateCreateinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateinfo.vertexBindingDescriptionCount = (uint32_t)vertexInputBindingDescription.size( );
    vertexInputStateCreateinfo.pVertexBindingDescriptions = vertexInputBindingDescription.data( );
    vertexInputStateCreateinfo.vertexAttributeDescriptionCount = (uint32_t)vertexInputAttributeDescription.size( );
    vertexInputStateCreateinfo.pVertexAttributeDescriptions = vertexInputAttributeDescription.data( );

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo = {};
    inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo.topology = topology;
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
    graphicsPipelineCreateInfo.pStages = shaderStageCreateInfo;
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateinfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = m_layout;
    graphicsPipelineCreateInfo.renderPass = m_renderpass;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(grm.getDevice( ), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, 0, &m_handle))
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

Error Pipeline::initDescriptorSetLayout(const Manager& grm, UNUSED_PARAM const Context& grc) {
    VkDescriptorSetLayoutBinding cameraLayoutBinding = {};
    cameraLayoutBinding.binding = 0;
    cameraLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    cameraLayoutBinding.descriptorCount = 1;
    cameraLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &cameraLayoutBinding;

    if (vkCreateDescriptorSetLayout(grm.getDevice( ), &layoutInfo, 0, &m_descriptorSetLayout))
        return Error::FUNCTION_FAILED;

    VkDeviceSize bufferSize = sizeof(Camera::UniformCamera);
    m_uniformBuffers.resize(grc.getSwapImages( ).size( ));

    for (size_t i = 0; i < grc.getSwapImages( ).size( ); i += 1) {
        m_uniformBuffers[i].init(grm, grc.getCamera( ).getCamHandlePTR( ), sizeof(Camera::UniformCamera), 0);
    }

    std::vector<VkDescriptorSetLayout> layouts(grc.getSwapImages( ).size( ), m_descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = grc.getDescriptorPool( );
    allocInfo.descriptorSetCount = (uint32_t)grc.getSwapImages( ).size( );
    allocInfo.pSetLayouts = layouts.data( );

    m_descriptorSet.resize(grc.getSwapImages( ).size( ));
    if (vkAllocateDescriptorSets(grm.getDevice( ), &allocInfo, m_descriptorSet.data( ))) return Error::FUNCTION_FAILED;
    for (size_t i = 0; i < grc.getSwapImages( ).size( ); i += 1) {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = m_uniformBuffers[i].getHandle( );
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(Camera::UniformCamera);

        VkWriteDescriptorSet descriptorWrite = {};
        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrite.dstSet = m_descriptorSet[i];
        descriptorWrite.dstBinding = 0;
        descriptorWrite.dstArrayElement = 0;
        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrite.descriptorCount = 1;
        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(grm.getDevice( ), 1, &descriptorWrite, 0, 0);
    }
    return Error::NONE;
}

}    // namespace gr
