#include "Instance.h"
#include "Logger.h"
#include "utils.h"

namespace ffGraph {
namespace Vulkan {

static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                             UNUSED_PARAM(VkDebugUtilsMessageTypeFlagsEXT messageType),
                                                             const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                             UNUSED_PARAM(void *pUserData)) {
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
                             vkContext.vkDevice.QueueIndex[DEVICE_GRAPH_QUEUE]);

    pushInitCmdBuffer(vkContext.vkDevice, GraphConstruct.DepthImage, GraphConstruct.ColorImage, vkRenderer.CommandPool);

    glfwSetWindowUserPointer(m_Window.Handle, this);
    initGFLWCallbacks( );
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
    RenderGraph Graph;
    while (!ffWindowShouldClose(m_Window)) {
        Events( );
        if (!SharedQueue->empty( )) {
            JSON::SceneLayout Layout = JSON::JSONString_to_SceneLayout(SharedQueue->at(0));
            SharedQueue->pop_front( );
            Graphs.push_back(
                ConstructRenderGraph(vkContext.vkDevice, GraphConstruct.RenderPass, Allocator, Layout, Resources));
            InitCameraController((*(Graphs.end( ) - 1)).Cam,
                                 (float)m_Window.WindowSize.width / (float)m_Window.WindowSize.height, 90.f,
                                 CameraType::_2D);
            (*(Graphs.end( ) - 1)).PushCamera.ViewProj = (*(Graphs.end( ) - 1)).Cam.Handle.ViewProjMatrix;
        }
        if (!Graphs.empty( )) {
            Render(vkContext, GraphConstruct.RenderPass, GraphConstruct.Framebuffers, vkRenderer,
                   Graphs[CurrentRenderGraph], m_Window.WindowSize);
        }
    }
}

}    // namespace Vulkan
}    // namespace ffGraph
