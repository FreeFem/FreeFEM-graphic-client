#include <imgui.h>
#include "Instance.h"
#include "Logger.h"
#include "utils.h"
#include "GlobalEnvironment.h"

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
    Shaders.resize(5);
    Shaders[0] = ImportShader("./shaders/Color.frag.spirv", Device, VK_SHADER_STAGE_FRAGMENT_BIT);
    Shaders[1] = ImportShader("./shaders/Geo2D.vert.spirv", Device, VK_SHADER_STAGE_VERTEX_BIT);
    Shaders[2] = ImportShader("./shaders/Geo3D.vert.spirv", Device, VK_SHADER_STAGE_VERTEX_BIT);

    Shaders[3] = ImportShader("./shaders/UI.vert.spirv", Device, VK_SHADER_STAGE_VERTEX_BIT);
    Shaders[4] = ImportShader("./shaders/UI.frag.spirv", Device, VK_SHADER_STAGE_FRAGMENT_BIT);
    return true;
}

static void InitImGui(int width, int height) {
    IMGUI_CHECKVERSION( );
    ImGui::CreateContext( );

    ImGuiStyle &style = ImGui::GetStyle( );
    style.Colors[ImGuiCol_TitleBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.6f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(1.0f, 0.0f, 0.0f, 0.8f);
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_Header] = ImVec4(1.0f, 0.0f, 0.0f, 0.4f);
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.0f, 1.0f, 0.0f, 1.0f);
    // Dimensions
    ImGuiIO &io = ImGui::GetIO( );
    ImFont *font1 = io.Fonts->AddFontDefault( );
    io.DisplaySize = ImVec2(width, height);
    io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;
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
    InitImGui(m_Window.WindowSize.width, m_Window.WindowSize.height);
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
    GlobalEnvironmentPTR = &this->Env;

    VkCommandPoolCreateInfo CmdPoolCreateInfos = {};
    CmdPoolCreateInfos.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    CmdPoolCreateInfos.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    CmdPoolCreateInfos.queueFamilyIndex = Env.GPUInfos.QueueIndex[Env.GPUInfos.TransferQueueIndex];

    if (vkCreateCommandPool(Env.GPUInfos.Device, &CmdPoolCreateInfos, 0, &Env.TransfertCommandPool)) {
        LogError(GetCurrentLogLocation( ), "Failed to create VkCommandPool.");
        return;
    }

    if (CreatePerFrameData(Env.GPUInfos.Device, Env.GraphManager.CommandPool, 2, FrameData) == false) return;
    if (ImportShaders(Shaders, Env.GPUInfos.Device) == false) return;

    VkShaderModule ShadersData[2];
    ShadersData[0] = FindShader(Shaders, "UI.vert");
    ShadersData[1] = FindShader(Shaders, "UI.frag");

    Ui = NewUiPipeline(ShadersData);
}

void Instance::destroy( ) {
    vkDeviceWaitIdle(Env.GPUInfos.Device);
    DestroyUiPipeline(Ui);

    ImGui::DestroyContext( );
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

Environment *GlobalEnvironmentPTR = {};

}    // namespace Vulkan
}    // namespace ffGraph