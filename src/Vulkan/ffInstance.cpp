#include <algorithm>
#include "ffInstance.h"
#include "../util/Logger.h"

namespace ffGraph
{
namespace Vulkan
{

VKAPI_ATTR VkBool32 VKAPI_CALL
debugMessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                       VkDebugUtilsMessageTypeFlagsEXT messageType,
                       const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                       void *pUserData) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        LogWarning(GetCurrentLogLocation(), "%s [%s] : %s\n", GetVariableAsString(VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT), pCallbackData->pMessageIdName, pCallbackData->pMessage);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        LogWarning(GetCurrentLogLocation(), "%s [%s] : %s\n", GetVariableAsString(VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT), pCallbackData->pMessageIdName, pCallbackData->pMessage);
    } else {
        LogWarning(GetCurrentLogLocation(), "%s [%s] : %s\n", GetVariableAsString(VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT), pCallbackData->pMessageIdName, pCallbackData->pMessage);
    }
    return VK_FALSE;
}

ffInstance ffNewInstance(std::vector<std::string> Extensions, std::vector<std::string> Layers)
{
    ffInstance n;

    VkApplicationInfo AppInfo = {};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pApplicationName = "FreeFEM";
    AppInfo.engineVersion = VK_MAKE_VERSION(0, 2, 1);
    AppInfo.pEngineName = "FreeFEM Engine";

    VkInstanceCreateInfo createInfos = {};
    createInfos.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfos.pApplicationInfo = &AppInfo;

    std::vector<const char *> enabledLayers(Layers.size());
    for (auto const& layer : Layers) {
        enabledLayers.push_back(layer.data());
    }
#ifdef _DEBUG
    if (std::find(Layers.begin(), eLayers.end(), "VK_LAYER_KHRONOS_Validation") == Layers.end())
        enabledLayers.push_back("VK_LAYER_KHRONOS_Validation");
#endif

    n.Extensions.reserve(Extensions.size());
    for (auto const& ext : Extensions) {
        n.Extensions.push_back(ext.data());
    }
#ifdef _DEBUG
    if (std::find(Extensions.begin(), Extensions.end(), VK_EXT_DEBUG_REPORT_EXTENSION_NAME) == Extensions.end()) {
        Extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
        enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    }
    if (std::find(Extensions.begin(), Extensions.end(), VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == Extensions.end()) {
        Extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        enabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
#endif

#ifdef _DEBUG
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

    createInfos.enabledLayerCount = enabledLayers.size();
    createInfos.ppEnabledLayerNames = enabledLayers.data();

    createInfos.enabledExtensionCount = n.Extensions.size();
    createInfos.ppEnabledExtensionNames = n.Extensions.data();

    if (vkCreateInstance(&createInfos, 0, &n.Handle) != VK_SUCCESS)
        return {VK_NULL_HANDLE, {}};

#ifdef _DEBUG
    PFN_vkCreateDebugUtilsMessengerEXT DebugUtilMessenger = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(n.Handle, "vkCreateDebugUtilsMessengerEXT"));
    if (DebugUtilMessenger)
        DebugUtilMessenger(n.Handle, &MessengerCreateInfo, 0, &n.DebugMessenger)
#endif
    return n;
}

void ffDestroyInstance(ffInstance Instance)
{
    vkDestroyInstance(Instance.Handle, 0);
}

} // namespace Vulkan
} // namespace ffGraph
