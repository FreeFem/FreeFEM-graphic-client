#include "Instance.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                             void *pUserData) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LogWarning(GetCurrentLogLocation( ), "%s [%s] : %s\n",
                   GetVariableAsString(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT), pCallbackData->pMessageIdName,
                   pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LogWarning(GetCurrentLogLocation( ), "%s [%s] : %s\n",
                   GetVariableAsString(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT), pCallbackData->pMessageIdName,
                   pCallbackData->pMessage);
    } else {
        LogWarning(GetCurrentLogLocation( ), "%s [%s] : %s\n",
                   GetVariableAsString(VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT), pCallbackData->pMessageIdName,
                   pCallbackData->pMessage);
    }
    return VK_FALSE;
}

Instance::Instance( ) {}

Instance::Instance(NativeWindow &Window) : m_Window(Window) {}

bool FindExtension(std::vector<const char *> list, std::string toCompare) {
    for (auto &ext : list) {
        if (toCompare.compare(ext) == 0) return true;
    }
    return false;
}

static void FramebufferResizeCallback(GLFWwindow *Window, int width, int height)
{
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    Handle->m_Window.WindowSize = {width, height};
    Handle->reload();
}

static void KeyCallback(GLFWwindow *Window, int key, int scancode, int action, int mods) {
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    float Factor = std::min(Handle->Graphs[Handle->CurrentRenderGraph].Cam.ZoomLevel, 1.f) / 10;
    if (key == GLFW_KEY_LEFT) {
        CameraTranslate(Handle->Graphs[Handle->CurrentRenderGraph].Cam, glm::vec3(1.f * Factor, 0.f, 0.f));
    } else if (key == GLFW_KEY_RIGHT) {
        CameraTranslate(Handle->Graphs[Handle->CurrentRenderGraph].Cam, glm::vec3(-1.f * Factor, 0.f, 0.f));
    } else if (key == GLFW_KEY_UP) {
        CameraTranslate(Handle->Graphs[Handle->CurrentRenderGraph].Cam, glm::vec3(0.f, 1.f * Factor, 0.f));
    } else if (key == GLFW_KEY_DOWN) {
        CameraTranslate(Handle->Graphs[Handle->CurrentRenderGraph].Cam, glm::vec3(0.f, -1.f * Factor, 0.f));
    } else if (key == GLFW_KEY_R) {
        CameraResetPositionAndZoom(Handle->Graphs[Handle->CurrentRenderGraph].Cam);
    } else if (key == GLFW_KEY_U) {
        double x, y = 0;
        glfwGetCursorPos(Window, &x, &y);
        x -= ((float)Handle->m_Window.WindowSize.width / 2.f);
        x /= (float)Handle->m_Window.WindowSize.width / 2;
        y -= ((float)Handle->m_Window.WindowSize.height / 2.f);
        y /= (float)Handle->m_Window.WindowSize.height / 2;
        ApplyCameraTo2DPosition(Handle->Graphs[Handle->CurrentRenderGraph].Cam, x, y);
    } else if (key == GLFW_KEY_A) {
        if (Handle->CurrentRenderGraph > 0)
            --Handle->CurrentRenderGraph;
    } else if (key == GLFW_KEY_D) {
        Handle->CurrentRenderGraph = std::min(Handle->CurrentRenderGraph  + 1U, (uint32_t)Handle->Graphs.size() - 1U);
    }

    Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.ViewProj =
        Handle->Graphs[Handle->CurrentRenderGraph].Cam.Data.ViewProjectionMatrix;
}

static void MouseScroolCallback(GLFWwindow *Window, double xOffset, double yOffset) {
    Instance *Handle = static_cast<Instance *>(glfwGetWindowUserPointer(Window));

    Handle->Graphs[Handle->CurrentRenderGraph].Cam.ZoomLevel -= yOffset * 0.25f;
    Handle->Graphs[Handle->CurrentRenderGraph].Cam.ZoomLevel =
        std::max(Handle->Graphs[Handle->CurrentRenderGraph].Cam.ZoomLevel, 0.25f);
    Handle->Graphs[Handle->CurrentRenderGraph].Cam.ZoomLevel =
        std::min(Handle->Graphs[Handle->CurrentRenderGraph].Cam.ZoomLevel, 5.f);

    float AspectRatio = Handle->Graphs[Handle->CurrentRenderGraph].Cam.AspectRatio;
    float ZoomLevel = Handle->Graphs[Handle->CurrentRenderGraph].Cam.ZoomLevel;

    SetProjection(Handle->Graphs[Handle->CurrentRenderGraph].Cam, -AspectRatio * ZoomLevel, AspectRatio * ZoomLevel,
                  -ZoomLevel, ZoomLevel);
    Handle->Graphs[Handle->CurrentRenderGraph].PushCamera.ViewProj =
        Handle->Graphs[Handle->CurrentRenderGraph].Cam.Data.ViewProjectionMatrix;
}

void Instance::load(const std::string &AppName, unsigned int width, unsigned int height) {
    ffInitGFLW( );
    if (m_Window.Handle == NULL) {
        m_Window = ffNewWindow({width, height});
    }
    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pApplicationName = AppName.c_str( );
    AppInfo.engineVersion = VK_MAKE_VERSION(0, 2, 1);
    AppInfo.pEngineName = "FreeFEM Engine";

    VkInstanceCreateInfo createInfos = {};

    createInfos.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfos.pApplicationInfo = &AppInfo;

    Extensions.reserve(m_Window.SurfaceExtensions.size( ));
    for (auto const &ext : m_Window.SurfaceExtensions) {
        Extensions.push_back(ext.data( ));
    }
#ifdef _DEBUG
    if (!FindExtension(Extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    std::vector<const char *> enabledLayers = {
#ifdef _DEBUG
        "VK_LAYER_KHRONOS_validation",
#endif
    };

#ifdef _DEBUG    // Creating debug messenger
    VkDebugUtilsMessengerCreateInfoEXT MessengerCreateInfo = {};

    MessengerCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    MessengerCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                          VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    MessengerCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    MessengerCreateInfo.pfnUserCallback = debugMessengerCallback;
    createInfos.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&MessengerCreateInfo;
#endif

    createInfos.enabledLayerCount = enabledLayers.size( );
    createInfos.ppEnabledLayerNames = enabledLayers.data( );

    createInfos.enabledExtensionCount = Extensions.size( );
    createInfos.ppEnabledExtensionNames = Extensions.data( );

    if (vkCreateInstance(&createInfos, 0, &m_Handle) != VK_SUCCESS) return;
#ifdef _DEBUG
    PFN_vkCreateDebugUtilsMessengerEXT DebugUtilMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(m_Handle, "vkCreateDebugUtilsMessengerEXT"));
    if (DebugUtilMessenger) DebugUtilMessenger(m_Handle, &MessengerCreateInfo, 0, &m_DebugMessenger);
#endif

    memset(&vkContext, 0, sizeof(Context));
    newContext(vkContext, m_Handle, m_Window);

    if (vkContext.vkDevice.Handle == VK_NULL_HANDLE) {
        LogError(GetCurrentLogLocation( ), "Failed to create Context");
        return;
    }
    VmaAllocatorCreateInfo AllocatorCreateInfo = {};
    AllocatorCreateInfo.physicalDevice = vkContext.vkDevice.PhysicalHandle;
    AllocatorCreateInfo.device = vkContext.vkDevice.Handle;

    if (vmaCreateAllocator(&AllocatorCreateInfo, &Allocator)) {
        LogError(GetCurrentLogLocation( ), "Failed to create VmaAllocator.");
        return;
    }

    GraphConstruct = newGraphConstructor(vkContext.vkDevice, Allocator, vkContext.SurfaceFormat.format,
                                         m_Window.WindowSize, vkContext.vkSwapchain.Views);

    Resources = NewResources(vkContext.vkDevice.Handle);

    vkRenderer = NewRenderer(vkContext.vkDevice.Handle, &vkContext.vkDevice.Queue[DEVICE_GRAPH_QUEUE],
                             vkContext.vkDevice.QueueIndex[DEVICE_GRAPH_QUEUE], vkContext.Surface, m_Window.WindowSize);

    pushInitCmdBuffer(vkContext.vkDevice, GraphConstruct.DepthImage, GraphConstruct.ColorImage, vkRenderer.CommandPool);

    glfwSetWindowUserPointer(m_Window.Handle, this);
    glfwSetKeyCallback(m_Window.Handle, KeyCallback);
    glfwSetScrollCallback(m_Window.Handle, MouseScroolCallback);
}

void Instance::destroy( ) {
    vkDeviceWaitIdle(vkContext.vkDevice.Handle);
    for (auto &RenderG : Graphs) DestroyRenderGraph(vkContext.vkDevice.Handle, Allocator, RenderG);
    DestroyResources(vkContext.vkDevice.Handle, Resources);
    DestroyRenderer(vkContext.vkDevice.Handle, vkRenderer);
    DestroyGraphConstructor(vkContext.vkDevice.Handle, Allocator, GraphConstruct);
    destroyContext(vkContext, m_Handle);
#ifdef _DEBUG
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Handle, "vkDestroyDebugUtilsMessengerEXT");
    if (func) func(m_Handle, m_DebugMessenger, 0);
#endif
    vkDestroyInstance(m_Handle, 0);
    ffDestroyWindow(m_Window);
    ffTerminateGLFW( );
}

void Instance::run(std::shared_ptr<std::deque<std::string>> SharedQueue) {
    int i = 0;
    int render = 0;
    RenderGraph Graph;
    while (!ffWindowShouldClose(m_Window)) {
        if (!SharedQueue->empty( )) {
            JSON::SceneLayout Layout = JSON::JSONString_to_SceneLayout(SharedQueue->at(0));
            SharedQueue->pop_front( );
            JSON::LogSceneLayout(Layout);
            VkShaderModule Modules[2] = {Resources.GeometryVertex.Module, Resources.GeometryFragment.Module};
            Graphs.push_back(ConstructRenderGraph(vkContext.vkDevice, GraphConstruct.RenderPass, Allocator, Layout, Modules));
        }
        if (!Graphs.empty( )) {
            //Graphs[CurrentRenderGraph].PushCamera.Model = glm::rotate(Graphs[CurrentRenderGraph].PushCamera.Model, (float)0.0001, glm::vec3(0.f, 1.f, 0.f));
            Render(vkContext, GraphConstruct.RenderPass, GraphConstruct.Framebuffers, vkRenderer,
                   Graphs[CurrentRenderGraph], m_Window.WindowSize);
        }
    }
}

}    // namespace Vulkan
}    // namespace ffGraph
