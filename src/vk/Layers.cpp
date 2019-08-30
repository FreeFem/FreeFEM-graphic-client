#include "Layers.h"

VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackEXT(UNUSED_PARAM VkDebugReportFlagsEXT flags,
                                                      UNUSED_PARAM VkDebugReportObjectTypeEXT objectType,
                                                      UNUSED_PARAM uint64_t object, UNUSED_PARAM size_t location,
                                                      UNUSED_PARAM int32_t messageCode,
                                                      UNUSED_PARAM const char* pLayerPrefix, const char* pMessage,
                                                      UNUSED_PARAM void* pUserData) {
    dprintf(2, "Validation Layers CallBac : %s.\n", pMessage);
    return VK_FALSE;
}

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(UNUSED_PARAM VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                             UNUSED_PARAM VkDebugUtilsMessageTypeFlagsEXT messageType,
                                             const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                             UNUSED_PARAM void* pUserData) {
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::string message(pCallbackData->pMessageIdName);
        message.append(" > ");
        message.append(pCallbackData->pMessage);
        LOGW("Validation Layer Messenger", message);
    } else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::string message(pCallbackData->pMessageIdName);
        message.append(" > ");
        message.append(pCallbackData->pMessage);
        LOGW("Validation Layer Messenger", message);
    } else {
        std::string message(pCallbackData->pMessageIdName);
        message.append(" > ");
        message.append(pCallbackData->pMessage);
        LOGI("Validation Layer Messenger", message);
    }

    return VK_FALSE;
}

void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
}