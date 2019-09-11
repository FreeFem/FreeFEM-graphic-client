#include "Application.h"
#include <cstring>
#include "../vk/Buffer.h"

namespace FEM {

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
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

ApplicationCreateInfo getApplicationInfos(int ac, char **av) {
    ApplicationCreateInfo infos = {1280, 768};
    int ite = 1;

    if (ac == 1) return infos;
    while (ite < ac) {
        if (strcmp(av[ite], "-ScreenWidth") == 0) {
            if (ite + 1 >= ac) return infos;
            infos.ScreenWidth = atoi(av[ite + 1]);
        } else if (strcmp(av[ite], "-ScreenHeight") == 0) {
            if (ite + 1 >= ac) return infos;
            infos.ScreenHeight = atoi(av[ite + 1]);
        }
        ite += 1;
    }
    return infos;
}

bool newApplication(Application *App, const ApplicationCreateInfo AppCreateInfos) {
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
    App->Shaders = VK::newShaderLoader( );
    return true;
}

void destroyApplication(Application *App) {
    destroyWindow(&App->Screen);
    vkQueueWaitIdle(App->vkContext.Queue);
    VK::destroyShaderLoader(App->Shaders, App->vkContext);
    VK::destroyBufferStorage(App->vkContext.Allocator, App->Buffers);
    VK::destroyPipeline(App->Renderer, App->vkContext);
    VK::destroyVulkanContext(&App->vkContext);
}

void runApplication(Application *App) {
    LOGI("Application", "Running !");
    bool Quit = false;

    while (!Quit) {
        glfwPollEvents( );
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

bool renderCurrent(VK::VulkanContext &vkContext, const VK::Pipeline Renderer, const Window Win) {
    VkResult res;
    if (Renderer.SubPipelineCount == 0)
        return true;
    if (vkContext.FrameInfos[vkContext.CurrentFrame].initialize == true) {
        vkWaitForFences(vkContext.Device, 1, &vkContext.FrameInfos[vkContext.CurrentFrame].Fence, VK_TRUE, UINT64_MAX);
        vkResetFences(vkContext.Device, 1, &vkContext.FrameInfos[vkContext.CurrentFrame].Fence);
    }

    vkContext.FrameInfos[vkContext.CurrentFrame].initialize = true;

    uint32_t imageIndex = UINT32_MAX;
    res =
        vkAcquireNextImageKHR(vkContext.Device, vkContext.Swapchain, UINT64_MAX,
                              vkContext.FrameInfos[vkContext.CurrentFrame].Semaphores[0], VK_NULL_HANDLE, &imageIndex);

    if (res == VK_ERROR_OUT_OF_DATE_KHR)
        return false;
    else if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
        return false;

    VkCommandBufferBeginInfo BeginInfos = {};
    BeginInfos.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    BeginInfos.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

    VK::PipelineSubResources *SubPipeline = Renderer.SubPipelines;

    if (vkBeginCommandBuffer(vkContext.CommandBuffers[vkContext.CurrentFrame], &BeginInfos)) return false;

    VkClearValue clearValue[2] =
    {
        {1.0f, 1.0f, 1.0f, 1.0f},
        {1.0f, 0.f},
    };

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = Renderer.RenderPass;
    renderPassBeginInfo.framebuffer = Renderer.Framebuffers[imageIndex];
    renderPassBeginInfo.renderArea.offset = {0, 0};
    renderPassBeginInfo.renderArea.extent = {Win.ScreenWidth, Win.ScreenHeight};
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValue;

#ifdef _DEBUG
    VK::DebugMakerBegin(vkContext.CommandBuffers[vkContext.CurrentFrame], "Frame rendering buffer", vkContext.CmdDebugMarkerBeginEXT_PFN);
#endif
    vkCmdBeginRenderPass(vkContext.CommandBuffers[vkContext.CurrentFrame], &renderPassBeginInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    while (SubPipeline != 0) {

        vkCmdBindPipeline(vkContext.CommandBuffers[vkContext.CurrentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS,
                          SubPipeline->Handle);

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

        if (SubPipeline->VBuffers.size() > 0) {
            VkDeviceSize bufferOffsets = 0;
            for (auto vBuffer : SubPipeline->VBuffers) {
                vkCmdBindVertexBuffers(vkContext.CommandBuffers[vkContext.CurrentFrame], 0, 1,
                                &vBuffer.VulkanData.Handle, &bufferOffsets);
            }
        }

        vkCmdPushConstants(vkContext.CommandBuffers[vkContext.CurrentFrame], SubPipeline->Layout,
                           VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &Renderer.Cam->finalCamera);

        vkCmdDraw(vkContext.CommandBuffers[vkContext.CurrentFrame],
                  VK::CountNbOfVerticesInSubPipeline(*SubPipeline), 1, 0, 0);

        SubPipeline = SubPipeline->next;
    }

    vkCmdEndRenderPass(vkContext.CommandBuffers[vkContext.CurrentFrame]);
#ifdef _DEBUG
    VK::DebugMakerEnd(vkContext.CommandBuffers[vkContext.CurrentFrame], vkContext.CmdDebugMarkerEndEXT_PFN);
#endif

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

    if (res != VK_SUCCESS) return false;

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
    return true;
}

}    // namespace FEM
