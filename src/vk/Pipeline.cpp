#include <cstring>
#include <iostream>
#include <fstream>
#include "Pipeline.h"
#include "GraphContext.h"
#include "GraphManager.h"

namespace gr
{
    static bool loadAndCreateShaderModule(const VkDevice device, const char *filename, VkShaderModule &outShaderModule)
    {
        VkResult result;

        std::ifstream inFile;
        inFile.open(filename, std::ios_base::binary | std::ios_base::ate);

        if (!inFile)
            return false;
        size_t fileSize = inFile.tellg();
        std::vector<char> fileContents(fileSize);

        inFile.seekg(0, std::ios::beg);
        bool readStat = bool(inFile.read(fileContents.data(), fileSize));
        inFile.close();

        if (!readStat)
            return false;
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.pNext = 0;
        createInfo.flags = 0;
        createInfo.codeSize = fileSize;
        createInfo.pCode = reinterpret_cast<uint32_t *>(fileContents.data());

        result = vkCreateShaderModule(device, &createInfo, 0, &outShaderModule);
        if (result != VK_SUCCESS) {
            return false;
        }
        return true;
    }

    static int findMemoryTypeWithProperties(const VkPhysicalDeviceMemoryProperties memoryProperties,
                                 const uint32_t memoryTypeBits,
                                 const VkMemoryPropertyFlags requiredMemoryProperties)
    {
        uint32_t typeBits = memoryTypeBits;

        uint32_t len = std::min(memoryProperties.memoryTypeCount, 32u);
        for (uint32_t i = 0; i < len; i += 1) {
            if ((typeBits & 1) == 1) {
                if ((memoryProperties.memoryTypes[i].propertyFlags & requiredMemoryProperties) == requiredMemoryProperties)
                    return (int)i;
            }
            typeBits >>= 1;
        }
        return -1;
    }

    static Error createAndAllocBuffer(const VkDevice device,
                             const VkPhysicalDeviceMemoryProperties memProps,
                             const VkBufferUsageFlags bufferUsage,
                             const VkMemoryPropertyFlags requiredMemProps,
                             const VkDeviceSize bufferSize,
                             VkBuffer& outBuffer,
                             VkDeviceMemory& outBufferMem)
    {
        VkResult result;
        VkBuffer buffer;
        VkDeviceMemory bufferMemory;

        VkBufferCreateInfo bufferCreateInfo = {};
        bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferCreateInfo.pNext = 0;
        bufferCreateInfo.flags = 0;
        bufferCreateInfo.size = bufferSize;
        bufferCreateInfo.usage = bufferUsage;
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferCreateInfo.queueFamilyIndexCount = 0;
        bufferCreateInfo.pQueueFamilyIndices = 0;

        result = vkCreateBuffer(device, &bufferCreateInfo, 0, &buffer);
        if (result != VK_SUCCESS) {
            return Error::FUNCTION_FAILED;
        }

        VkMemoryRequirements memoryRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

        int memoryTypeIndex = findMemoryTypeWithProperties(memProps, memoryRequirements.memoryTypeBits, requiredMemProps);
        if (memoryTypeIndex < 0) {
            return Error::FUNCTION_FAILED;
        }

        VkMemoryAllocateInfo memoryAllocateInfo = {};
        memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        memoryAllocateInfo.pNext = 0;
        memoryAllocateInfo.allocationSize = memoryRequirements.size;
        memoryAllocateInfo.memoryTypeIndex = (uint32_t)memoryTypeIndex;

        result = vkAllocateMemory(device, &memoryAllocateInfo, 0, &bufferMemory);
        if (result != VK_SUCCESS) {
            return Error::FUNCTION_FAILED;
        }

        result = vkBindBufferMemory(device, buffer, bufferMemory, 0);
        if (result != VK_SUCCESS) {
            return Error::FUNCTION_FAILED;
        }

        outBuffer = buffer;
        outBufferMem = bufferMemory;

        return Error::NONE;
    }

    Error Pipeline::init(const Manager& grm, const Context& grc)
    {
        const size_t vertexBufferSize = sizeof(vertex) * NUM_DEMO_VERTICES;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMem;

        if (initRenderpass(grm, grc))
            return Error::FUNCTION_FAILED;
        if (initFramebuffers(grm, grc))
            return Error::FUNCTION_FAILED;
// Creating temporary test buffer
        std::vector<gr::AttributeDescription> attributes = {{0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, x)}, {1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(vertex, r)}};
        VertexBuffer tmp;
        tmp.init(grm, (void *)vertices, sizeof(vertex) * NUM_DEMO_VERTICES, sizeof(vertex), attributes, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        m_vertexBuffers.push_back(tmp);
//
        if (initPipeline(grm, grc))
            return Error::FUNCTION_FAILED;
        return Error::NONE;
    }

    Error Pipeline::reload(const Manager& grm, const Context& grc)
    {
        if (initRenderpass(grm, grc))
            return Error::FUNCTION_FAILED;
        if (initFramebuffers(grm, grc))
            return Error::FUNCTION_FAILED;
        if (initPipeline(grm, grc))
            return Error::FUNCTION_FAILED;
        return Error::NONE;
    }

    Error Pipeline::initRenderpass(const Manager& grm, const Context& grc)
    {
        VkAttachmentDescription attachmentDescription[2] = {{}, {}};
        attachmentDescription[0].flags = 0;
        attachmentDescription[0].format = grc.getSurfaceFormat();
        attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        attachmentDescription[1].flags = 0;
        attachmentDescription[1].format = grc.getDepthBufferFormat();
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

        if (vkCreateRenderPass(grm.getDevice(), &createInfo, 0, &m_renderpass) != VK_SUCCESS)
            return Error::FUNCTION_FAILED;
        return Error::NONE;
    }

    Error Pipeline::initFramebuffers(const Manager& grm, const Context& grc)
    {
        for (const auto view : grc.get_swapImageViews()) {
            VkFramebuffer fb;

            VkImageView attachment[2] = {view, grc.get_depthImage().getImageView()};
            VkFramebufferCreateInfo createInfo = {};
            createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            createInfo.pNext = 0;
            createInfo.flags = 0;
            createInfo.renderPass = m_renderpass;
            createInfo.attachmentCount = 2;
            createInfo.pAttachments = attachment;
            createInfo.width = grm.getNativeWindow().getWidth();
            createInfo.height = grm.getNativeWindow().getHeight();
            createInfo.layers = 1;

            vkCreateFramebuffer(grm.getDevice(), &createInfo, 0, &fb);
            m_framebuffers.push_back(fb);
        }
        return Error::NONE;
    }

    Error Pipeline::initPipeline(const Manager& grm, const Context& grc)
    {
        VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
        pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutCreateInfo.pNext = 0;
        pipelineLayoutCreateInfo.flags = 0;
        pipelineLayoutCreateInfo.setLayoutCount = 0;
        pipelineLayoutCreateInfo.pSetLayouts = 0;
        pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
        pipelineLayoutCreateInfo.pPushConstantRanges = 0;

        vkCreatePipelineLayout(grm.getDevice(), &pipelineLayoutCreateInfo, 0, &m_layout);

        VkShaderModule vertModule, fragModule;
        if (!loadAndCreateShaderModule(grm.getDevice(), VERTEX_SHADER_FILENAME, vertModule))
            return Error::FUNCTION_FAILED;
        if (!loadAndCreateShaderModule(grm.getDevice(), FRAGMENT_SHADER_FILENAME, fragModule))
            return Error::FUNCTION_FAILED;

        VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {{}, {}};
        shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStageCreateInfo[0].module = vertModule;
        shaderStageCreateInfo[0].pName = "main";
        shaderStageCreateInfo[0].pSpecializationInfo = 0;

        shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStageCreateInfo[1].module = fragModule;
        shaderStageCreateInfo[1].pName = "main";
        shaderStageCreateInfo[1].pSpecializationInfo = 0;

        VkVertexInputBindingDescription vertexInputBindingDescription = {};
        vertexInputBindingDescription.binding = VERTEX_INPUT_BINDING;
        vertexInputBindingDescription.stride = sizeof(vertex);
        vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        VkVertexInputAttributeDescription vertexInputAttributeDescription[2] = {{}, {}};
        vertexInputAttributeDescription[0].location = 0;
        vertexInputAttributeDescription[0].binding = VERTEX_INPUT_BINDING;
        vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexInputAttributeDescription[0].offset = offsetof(vertex, x);

        vertexInputAttributeDescription[1].location = 1;
        vertexInputAttributeDescription[1].binding = VERTEX_INPUT_BINDING;
        vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        vertexInputAttributeDescription[1].offset = offsetof(vertex, r);

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
        graphicsPipelineCreateInfo.layout = m_layout;
        graphicsPipelineCreateInfo.renderPass = m_renderpass;
        graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        graphicsPipelineCreateInfo.basePipelineIndex = -1;

        if (vkCreateGraphicsPipelines(grm.getDevice(), VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, 0, &m_handle))
            return Error::FUNCTION_FAILED;
        vkDestroyShaderModule(grm.getDevice(), vertModule, 0);
        vkDestroyShaderModule(grm.getDevice(), fragModule, 0);
        return Error::NONE;
    }

} // namespace gr
