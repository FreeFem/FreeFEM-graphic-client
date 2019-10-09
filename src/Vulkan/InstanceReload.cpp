#include "Instance.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static void cleanup_for_reload(Instance* Handle) {
    DestroyImage(Handle->Allocator, Handle->vkContext.vkDevice.Handle, Handle->GraphConstruct.DepthImage);
    memset(&Handle->GraphConstruct.DepthImage, 0, sizeof(Image));
    DestroyImage(Handle->Allocator, Handle->vkContext.vkDevice.Handle, Handle->GraphConstruct.ColorImage);
    memset(&Handle->GraphConstruct.ColorImage, 0, sizeof(Image));

    for (auto framebuffer : Handle->GraphConstruct.Framebuffers)
        vkDestroyFramebuffer(Handle->vkContext.vkDevice.Handle, framebuffer, 0);

    for (auto Node : Handle->GraphConstruct.Graphs) {
        for (auto Pipeline : Node.Nodes) {
            vkDestroyPipeline(Handle->vkContext.vkDevice.Handle, Pipeline.Handle, 0);
            vkDestroyPipelineLayout(Handle->vkContext.vkDevice.Handle, Pipeline.Layout, 0);
            Pipeline.Handle = VK_NULL_HANDLE;
            Pipeline.Layout = VK_NULL_HANDLE;
        }
    }
    vkDestroyRenderPass(Handle->vkContext.vkDevice.Handle, Handle->GraphConstruct.RenderPass, 0);

    for (auto& view : Handle->vkContext.vkSwapchain.Views)
        vkDestroyImageView(Handle->vkContext.vkDevice.Handle, view, 0);

    vkDestroySwapchainKHR(Handle->vkContext.vkDevice.Handle, Handle->vkContext.vkSwapchain.Handle, 0);
    Handle->vkContext.vkSwapchain.Views.clear();
    Handle->vkContext.vkSwapchain.Images.clear();
}

void Instance::reload( ) {
    vkDeviceWaitIdle(vkContext.vkDevice.Handle);

    cleanup_for_reload(this);

    vkContext.vkSwapchain = newSwapchain(vkContext.vkDevice, vkContext.Surface, m_Window.WindowSize);
    GraphConstruct = newGraphConstructor(vkContext.vkDevice, Allocator, vkContext.SurfaceFormat.format, m_Window.WindowSize,
                        vkContext.vkSwapchain.Views);
    pushInitCmdBuffer(vkContext.vkDevice, GraphConstruct.DepthImage, GraphConstruct.ColorImage, vkRenderer.CommandPool);
    for (auto& Graph : Graphs) {
        ReloadRenderGraph(vkContext.vkDevice, GraphConstruct.RenderPass, Resources, Graph);
    }
}

}    // namespace Vulkan
}    // namespace ffGraph