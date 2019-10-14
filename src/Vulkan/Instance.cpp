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

static bool ImportShaders(ShaderLibrary &Shaders, const VkDevice &Device) {
    Shaders.resize(3);
    Shaders[0] = ImportShader("./shaders/Color.frag.spirv", Device, VK_SHADER_STAGE_FRAGMENT_BIT);
    Shaders[1] = ImportShader("./shaders/Geo2D.vert.spirv", Device, VK_SHADER_STAGE_VERTEX_BIT);
    Shaders[2] = ImportShader("./shaders/Geo3D.vert.spirv", Device, VK_SHADER_STAGE_VERTEX_BIT);
    return true;
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
    initGFLWCallbacks( );
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

    if (CreateEnvironment(Env, m_Handle, m_Window) == false) return;
    if (CreatePerFrameData(Env.GPUInfos.Device, Env.GraphManager.CommandPool, 2, FrameData) == false) return;
    if (ImportShaders(Shaders, Env.GPUInfos.Device) == false) return;
}

void Instance::destroy( ) {
    vkDeviceWaitIdle(Env.GPUInfos.Device);

    for (size_t i = 0; i < Graphs.size( ); ++i) {
        DestroyRenderGraph(Env.GPUInfos.Device, Env.Allocator, Graphs[i]);
    }
    for (size_t i = 0; i < Shaders.size( ); ++i) {
        DestroyShader(Env.GPUInfos.Device, Shaders[i]);
    }
#ifdef _DEBUG
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_Handle, "vkDestroyDebugUtilsMessengerEXT");
    if (func) func(m_Handle, m_DebugMessenger, 0);
#endif
    DestroyPerFrameData(Env.GPUInfos.Device, Env.GraphManager.CommandPool, 2, FrameData);
    DestroyEnvironment(Env, m_Handle);
    vkDestroyInstance(m_Handle, 0);
    ffDestroyWindow(m_Window);
    ffTerminateGLFW( );
}

void Instance::run(std::shared_ptr<std::deque<std::string>> SharedQueue) {
    RenderGraph Graph;
    while (!ffWindowShouldClose(m_Window)) {
        if (Graphs.size( ) != 0) Events( );
        if (!SharedQueue->empty( )) {
            JSON::SceneLayout Layout = JSON::JSONString_to_SceneLayout(SharedQueue->at(0));
            SharedQueue->pop_front( );
            RenderGraphCreateInfos CreateInfos = {Env.GPUInfos.Device, Env.GraphManager.RenderPass,
                                                  Env.GPUInfos.Capabilities.msaaSamples};
            Graphs.push_back(ConstructRenderGraph(CreateInfos, Env.Allocator, Layout, Shaders));
            InitCameraController((*(Graphs.end( ) - 1)).Cam,
                                 (float)m_Window.WindowSize.width / (float)m_Window.WindowSize.height, 90.f,
                                 CameraType::_2D);
            (*(Graphs.end( ) - 1)).PushCamera.ViewProj = (*(Graphs.end( ) - 1)).Cam.Handle.ViewProjMatrix;
        }
        if (!Graphs.empty( )) {
            render( );
        }
    }
}

}    // namespace Vulkan
}    // namespace ffGraph