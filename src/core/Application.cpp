#include <cstring>
#include "Application.h"
#include "../vk/Buffer.h"

namespace FEM
{

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application *App = (Application *)glfwGetWindowUserPointer(window);

    if (key == GLFW_KEY_UP && action == GLFW_PRESS)
        App->Renderer.Cam->View = glm::translate(App->Renderer.Cam->View, glm::vec3(0, -0.5f, 0));
    else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
        App->Renderer.Cam->View = glm::translate(App->Renderer.Cam->View, glm::vec3(0, 0.5f, 0));
    else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
        App->Renderer.Cam->View = glm::translate(App->Renderer.Cam->View, glm::vec3(-0.5f, 0, 0));
    else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
        App->Renderer.Cam->View = glm::translate(App->Renderer.Cam->View, glm::vec3(0.5f, 0, 0));
}

ApplicationCreateInfo getApplicationInfos(int ac, char **av)
{
    ApplicationCreateInfo infos = {1280, 768};
    int ite = 1;

    if (ac == 1)
        return infos;
    while (ite < ac) {
        if (strcmp(av[ite], "-ScreenWidth") == 0) {
            if (ite + 1 >= ac)
                return infos;
            infos.ScreenWidth = atoi(av[ite + 1]);
        } else if (strcmp(av[ite], "-ScreenHeight") == 0) {
            if (ite + 1 >= ac)
                return infos;
            infos.ScreenHeight = atoi(av[ite + 1]);
        }
        ite += 1;
    }
    return infos;
}

bool newApplication(Application *App, const ApplicationCreateInfo AppCreateInfos)
{
    if (!newWindow(&App->Screen, AppCreateInfos.ScreenWidth, AppCreateInfos.ScreenHeight, "FreeFEM++")) {
        LOGE("newApplication", "Failed to initialize Window.");
        return false;
    }
    glfwSetWindowUserPointer(App->Screen.Handle, App);
    glfwSetKeyCallback(App->Screen.Handle, key_callback);
    if (!VK::newVulkanContext(&App->vkContext, &App->Screen)) {
        LOGE("newApplication", "Failed to initialize VulkanContext.");
        return false;
    }
    if (!VK::newPipeline(&App->Renderer, App->vkContext, App->Screen)) {
        LOGE("newApplication", "Failed to initialize Pipeline.");
        return false;
    }
    App->Shaders = VK::newShaderLoader();
    return true;
}


void destroyApplication(Application *App)
{
    destroyWindow(&App->Screen);
    VK::destroyShaderLoader(App->Shaders, App->vkContext);
    vkDeviceWaitIdle(App->vkContext.Device);
    VK::destroyPipeline(App->Renderer, App->vkContext);
    VK::destroyVulkanContext(&App->vkContext);
}

void runApplication(Application *App)
{
    LOGI("Application", "Running !");
    bool Quit = false;

    VK::newShader("Vertex", "./shaders/geometry.vert.spirv", &App->Shaders, App->vkContext);
    VK::newShader("Fragment", "./shaders/geometry.frag.spirv", &App->Shaders, App->vkContext);

    VK::PipelineSubResources *my_pipeline = (VK::PipelineSubResources *)malloc(sizeof(VK::PipelineSubResources));
    my_pipeline->VertexShader = VK::searchShader("Vertex", App->Shaders);
    my_pipeline->FragmentShader = VK::searchShader("Fragment", App->Shaders);

    VK::BufferInfos BInfos = {};
    BInfos.ElementCount = NUM_DEMO_VERTICES;
    BInfos.ElementSize = SIZE_DEMO_VERTEX;
    BInfos.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    BInfos.AllocInfos.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    BInfos.AllocInfos.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    BInfos.AllocInfos.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    BInfos.AllocInfos.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkVertexInputAttributeDescription InputAttrib[2] = {};
    InputAttrib[0].binding = 0;
    InputAttrib[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    InputAttrib[0].location = 0;
    InputAttrib[0].offset = 0;

    InputAttrib[1].binding = 0;
    InputAttrib[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    InputAttrib[1].location = 1;
    InputAttrib[1].offset = sizeof(float) * 3;
    if (!VK::newVertexBuffer(App->vkContext.Allocator, &my_pipeline->VBuffer, BInfos, 2, InputAttrib)) {
        LOGI("Loop", "Failed to create tmp vertexBuffer");
        return;
    }
    memcpy(my_pipeline->VBuffer.VulkanData.MemoryInfos.pMappedData, vertices, my_pipeline->VBuffer.VulkanData.MemoryInfos.size);
    computeCamera(App->Renderer.Cam);
    VK::addSubPipeline(my_pipeline, App->vkContext, &App->Renderer);

    while (!Quit) {
        glfwPollEvents();
        if (glfwWindowShouldClose(App->Screen.Handle)) {
            LOGI("Application", "Closing !");
            Quit = true;
        }
        computeCamera(App->Renderer.Cam);
        renderCurrent(App->vkContext, App->Renderer, App->Screen);
        App->vkContext.CurrentFrame = !App->vkContext.CurrentFrame;
    }

    return;
}

bool renderCurrent(VK::VulkanContext& vkContext, const VK::Pipeline Renderer, const Window Win)
{
    VkResult res;
    if (vkContext.FrameInfos[vkContext.CurrentFrame].initialize == true) {
        vkWaitForFences(vkContext.Device, 1, &vkContext.FrameInfos[vkContext.CurrentFrame].Fence, VK_TRUE, UINT64_MAX);
        vkResetFences(vkContext.Device, 1, &vkContext.FrameInfos[vkContext.CurrentFrame].Fence);
    }

    vkContext.FrameInfos[vkContext.CurrentFrame].initialize = true;

    uint32_t imageIndex = UINT32_MAX;
    res = vkAcquireNextImageKHR(vkContext.Device, vkContext.Swapchain, UINT64_MAX, vkContext.FrameInfos[vkContext.CurrentFrame].Semaphores[0], VK_NULL_HANDLE, &imageIndex);

    if (res == VK_ERROR_OUT_OF_DATE_KHR)
        return false;
    else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
        return false;

    VkCommandBufferBeginInfo BeginInfos = {};
    BeginInfos.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfos.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK::PipelineSubResources *SubPipeline = Renderer.SubPipelines;
    //while (SubPipeline != 0) {

        if (vkBeginCommandBuffer(vkContext.CommandBuffers[vkContext.CurrentFrame], &BeginInfos))
            return false;


        VkClearValue clearValue[2];
        clearValue[0].color.float32[0] = 1.0f;
        clearValue[0].color.float32[1] = 1.0f;
        clearValue[0].color.float32[2] = 1.0f;
        clearValue[0].color.float32[3] = 0.0f;
        clearValue[1].depthStencil.depth = 1.0f;
        clearValue[1].depthStencil.stencil = 0.0f;

        VkRenderPassBeginInfo renderPassBeginInfo = {};
        renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassBeginInfo.renderPass = Renderer.RenderPass;
        renderPassBeginInfo.framebuffer = Renderer.Framebuffers[imageIndex];
        renderPassBeginInfo.renderArea.offset = {0, 0};
        renderPassBeginInfo.renderArea.extent = {Win.ScreenWidth, Win.ScreenHeight};
        renderPassBeginInfo.clearValueCount = 2;
        renderPassBeginInfo.pClearValues = clearValue;

        vkCmdBeginRenderPass(vkContext.CommandBuffers[vkContext.CurrentFrame], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(vkContext.CommandBuffers[vkContext.CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, SubPipeline->Handle);

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)Win.ScreenWidth;
        viewport.height = (float)Win.ScreenHeight;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(vkContext.CommandBuffers[vkContext.CurrentFrame], 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = Win.ScreenWidth;
        scissor.extent.height = Win.ScreenHeight;

        vkCmdSetScissor(vkContext.CommandBuffers[vkContext.CurrentFrame], 0, 1, &scissor);

        VkDeviceSize bufferOffsets = 0;
        vkCmdBindVertexBuffers(vkContext.CommandBuffers[vkContext.CurrentFrame], 0, 1, &SubPipeline->VBuffer.VulkanData.Handle, &bufferOffsets);

        vkCmdPushConstants(vkContext.CommandBuffers[vkContext.CurrentFrame], SubPipeline->Layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &Renderer.Cam->finalCamera);

        vkCmdDraw(vkContext.CommandBuffers[vkContext.CurrentFrame], SubPipeline->VBuffer.VulkanData.CreationInfos.ElementCount, 1, 0, 0);

        vkCmdEndRenderPass(vkContext.CommandBuffers[vkContext.CurrentFrame]);

        vkEndCommandBuffer(vkContext.CommandBuffers[vkContext.CurrentFrame]);

        VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = &vkContext.FrameInfos[vkContext.CurrentFrame].Semaphores[0];
        submitInfo.pWaitDstStageMask = &pipelineStageFlags;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &vkContext.CommandBuffers[vkContext.CurrentFrame];
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = &vkContext.FrameInfos[vkContext.CurrentFrame].Semaphores[1];

        res = vkQueueSubmit(vkContext.Queue, 1, &submitInfo, vkContext.FrameInfos[vkContext.CurrentFrame].Fence);

        if (res != VK_SUCCESS)
            return false;

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &vkContext.FrameInfos[vkContext.CurrentFrame].Semaphores[1];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &vkContext.Swapchain;
        presentInfo.pImageIndices = &imageIndex;

        res = vkQueuePresentKHR(vkContext.Queue, &presentInfo);
        if (res == VK_ERROR_OUT_OF_DATE_KHR)
            return false;
        else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
            return false;
    //}
    return true;
}

} // namespace FEM
