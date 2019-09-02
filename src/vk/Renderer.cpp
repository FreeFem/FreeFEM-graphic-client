#include "Renderer.h"

namespace FEM {

namespace gr {

ErrorValues Pipeline::init(const Manager& grm, const Context& grc, Renderer& grr) {

    // Initialize RenderPass
    VkAttachmentDescription attachmentDescription[2] = {{}, {}};
    attachmentDescription[0].flags = 0;
    attachmentDescription[0].format = grc.SurfaceFormat;
    attachmentDescription[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    attachmentDescription[1].flags = 0;
    attachmentDescription[1].format = grc.DepthBufferFormat;
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

    if (vkCreateRenderPass(grm.Device, &createInfo, 0, &RenderPass) != VK_SUCCESS) return ErrorValues::FUNCTION_FAILED;

    // Initialize Framebuffers
    for (const auto view : grc.SwapImageViews) {
        VkFramebuffer fb;

        VkImageView attachment[2] = {view, grc.DepthImage.getImageView( )};
        VkFramebufferCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.pNext = 0;
        createInfo.flags = 0;
        createInfo.renderPass = RenderPass;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = attachment;
        createInfo.width = grm.Window->getWidth( );
        createInfo.height = grm.Window->getHeight( );
        createInfo.layers = 1;

        vkCreateFramebuffer(grm.Device, &createInfo, 0, &fb);
        Framebuffers.push_back(fb);
    }

    // To-do initialize DescriptorSets

    // Initialize Pipeline
    VkPushConstantRange pushRange;
    pushRange.offset = 0;
    pushRange.size = sizeof(glm::mat4);
    pushRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pNext = 0;
    pipelineLayoutCreateInfo.flags = 0;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushRange;

    vkCreatePipelineLayout(grm.Device, &pipelineLayoutCreateInfo, 0, &PipelineLayout);

    int shaderCount = 2;
    VkPipelineShaderStageCreateInfo shaderStageCreateInfo[2] = {{}, {}};
    shaderStageCreateInfo[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfo[0].module = ShaderModules.VertexModule;
    shaderStageCreateInfo[0].pName = "main";
    shaderStageCreateInfo[0].pSpecializationInfo = 0;

    shaderStageCreateInfo[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfo[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfo[1].module = ShaderModules.FragmentModule;
    shaderStageCreateInfo[1].pName = "main";
    shaderStageCreateInfo[1].pSpecializationInfo = 0;

    int binding_input = 0;
    std::vector<VkVertexInputBindingDescription> vertexInputBindingDescription = {};
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescription = {};
    VkPrimitiveTopology topology;

    for (const auto vBuffer : VertexBuffers) {
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
    rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_LINE;
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
    graphicsPipelineCreateInfo.stageCount = shaderCount;
    graphicsPipelineCreateInfo.pStages = shaderStageCreateInfo;
    graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateinfo;
    graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    graphicsPipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo;
    graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    graphicsPipelineCreateInfo.layout = PipelineLayout;
    graphicsPipelineCreateInfo.renderPass = RenderPass;
    graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    graphicsPipelineCreateInfo.basePipelineIndex = -1;

    if (vkCreateGraphicsPipelines(grm.Device, VK_NULL_HANDLE, 1, &graphicsPipelineCreateInfo, 0, &PipelineHandle))
        return ErrorValues::FUNCTION_FAILED;
    return ErrorValues::NONE;
}

void Pipeline::destroy(const Manager& grm, const Context& grc) {
    ShaderModules.destroy(grm.Device);

    for (size_t i = 0; i < Framebuffers.size( ); i += 1) vkDestroyFramebuffer(grm.Device, Framebuffers[i], 0);

    for (size_t i = 0; i < VertexBuffers.size(); i += 1) VertexBuffers[i].destroy(grm);
    vkDestroyPipeline(grm.Device, PipelineHandle, 0);
    vkDestroyPipelineLayout(grm.Device, PipelineLayout, 0);
    vkDestroyRenderPass(grm.Device, RenderPass, 0);
}

ErrorValues Pipeline::reload(const Manager& grm, const Context& grc, Renderer& grr) {
    for (size_t i = 0; i < Framebuffers.size( ); i += 1) vkDestroyFramebuffer(grm.Device, Framebuffers[i], 0);

    vkDestroyPipeline(grm.Device, PipelineHandle, 0);
    vkDestroyPipelineLayout(grm.Device, PipelineLayout, 0);
    vkDestroyRenderPass(grm.Device, RenderPass, 0);
    init(grm, grc, grr);

    return ErrorValues::NONE;
}

ErrorValues Pipeline::render(const Manager& grm, Context& grc, Renderer& grr) {
    VkResult res;
    if (grc.PerFrame[grc.CurrentFrame].fenceInitialized) {
        vkWaitForFences(grm.Device, 1, &grc.PerFrame[grc.CurrentFrame].presentFence, VK_TRUE, UINT64_MAX);
        vkResetFences(grm.Device, 1, &grc.PerFrame[grc.CurrentFrame].presentFence);
    }

    grc.BlockFrame();

    uint32_t imageIndex = UINT32_MAX;
    res = vkAcquireNextImageKHR(grm.Device, grc.Swapchain, UINT64_MAX, grc.PerFrame[grc.CurrentFrame].acquiredSemaphore,
                                VK_NULL_HANDLE, &imageIndex);
    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        LOGI("Pipeline::render", "We don't support out-of-date swapchain.");
        return ErrorValues::FUNCTION_FAILED;
    } else if (res == VK_SUBOPTIMAL_KHR)
        LOGI("Pipeline::render", "Swapchain is suboptimal.");
    else if (res != VK_SUCCESS)
        return ErrorValues::FUNCTION_FAILED;
    VkCommandBufferBeginInfo cmdBufferBeginInfo = {};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    res = vkBeginCommandBuffer(grc.PresentCmdBuffer[grc.CurrentFrame], &cmdBufferBeginInfo);
    grm.beginDebugMaker(grc.PresentCmdBuffer[grc.CurrentFrame], "PresentCmdBuffer");

    if (res != VK_SUCCESS) return ErrorValues::FUNCTION_FAILED;

    VkClearValue clearValues[2];
    clearValues[0].color.float32[0] = 1.0f;
    clearValues[0].color.float32[1] = 1.0f;
    clearValues[0].color.float32[2] = 1.0f;
    clearValues[0].color.float32[3] = 1.0f;
    clearValues[1].depthStencil.depth = 1.0f;
    clearValues[1].depthStencil.stencil = 0.0f;

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = RenderPass;
    renderPassBeginInfo.framebuffer = Framebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = {grm.Window->getWidth( ), grm.Window->getHeight( )};
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(grc.PresentCmdBuffer[grc.CurrentFrame], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(grc.PresentCmdBuffer[grc.CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineHandle);

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)grm.Window->getWidth( );
    viewport.height = (float)grm.Window->getHeight( );
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(grc.PresentCmdBuffer[grc.CurrentFrame], 0, 1, &viewport);

    VkRect2D scissor = {};
    scissor.offset.x = 0;
    scissor.offset.y = 0;
    scissor.extent.width = grm.Window->getWidth( );
    scissor.extent.height = grm.Window->getHeight( );

    vkCmdSetScissor(grc.PresentCmdBuffer[grc.CurrentFrame], 0, 1, &scissor);

    VkDeviceSize bufferOffsets = 0;
    std::vector<VertexBuffer> vertexBuffers = VertexBuffers;
    for (size_t i = 0; i < vertexBuffers.size( ); i += 1) {
        VkBuffer tmp = vertexBuffers[i].getHandle( );
        vkCmdBindVertexBuffers(grc.PresentCmdBuffer[grc.CurrentFrame], i, vertexBuffers.size( ), &tmp, &bufferOffsets);
    }

    Camera::UniformCamera *currentGlobalCamera = grr.Cam.getCamHandlePTR();
    PushCamera.perspective = currentGlobalCamera->perspective;
    PushCamera.view = currentGlobalCamera->view;
    PushCamera.model = glm::rotate(PushCamera.model, glm::radians(0.5f), glm::vec3(0.f, 1.f, 1.f));
    PushMatrix = PushCamera.perspective * PushCamera.view * PushCamera.model;

    vkCmdPushConstants(grc.PresentCmdBuffer[grc.CurrentFrame], PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0,
                       sizeof(glm::mat4), &PushMatrix);

    vkCmdDraw(grc.PresentCmdBuffer[grc.CurrentFrame], VerticesCount( ), 1, 0, 0);

    vkCmdEndRenderPass(grc.PresentCmdBuffer[grc.CurrentFrame]);

    grm.endDebugMaker(grc.PresentCmdBuffer[grc.CurrentFrame]);
    res = vkEndCommandBuffer(grc.PresentCmdBuffer[grc.CurrentFrame]);

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &grc.PerFrame[grc.CurrentFrame].acquiredSemaphore;
    submitInfo.pWaitDstStageMask = &pipelineStageFlags;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &grc.PresentCmdBuffer[grc.CurrentFrame];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &grc.PerFrame[grc.CurrentFrame].renderCompletedSemaphore;

    res = vkQueueSubmit(grm.Queue, 1, &submitInfo, grc.PerFrame[grc.CurrentFrame].presentFence);

    if (res != VK_SUCCESS) return ErrorValues::FUNCTION_FAILED;

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &grc.PerFrame[grc.CurrentFrame].renderCompletedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &grc.Swapchain;
    presentInfo.pImageIndices = &imageIndex;

    res = vkQueuePresentKHR(grm.Queue, &presentInfo);

    if (res == VK_ERROR_OUT_OF_DATE_KHR) {
        LOGI("Pipeline::render", "We don't support out-of-date swapchain.");
        return ErrorValues::FUNCTION_FAILED;
    } else if (res == VK_SUBOPTIMAL_KHR)
        LOGI("Pipeline::render", "Swapchain is suboptimal.");
    else if (res != VK_SUCCESS)
        return ErrorValues::FUNCTION_FAILED;

    return ErrorValues::NONE;
}

ErrorValues Renderer::render(const Manager& grm, Context& grc)
{
    if (List.Pipelines.empty())
        return ErrorValues::NONE;
    size_t size = List.Pipelines.size();

    for (size_t i = 0; i < size; i += 1) {
        if (List[i].render(grm, grc, *this))
            return ErrorValues::FUNCTION_FAILED;
    }
    return ErrorValues::NONE;
}

void Renderer::destroy(const Manager& grm, const Context& grc)
{
    for (size_t i = 0; i < List.Pipelines.size(); i += 1)
    {
        List.Pipelines[i].destroy(grm, grc);
    }
}

}    // namespace gr

}    // namespace FEM
