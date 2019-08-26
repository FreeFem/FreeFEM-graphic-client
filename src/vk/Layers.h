#ifndef LAYERS_H
#define LAYERS_H

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdio>
#include "../util/utils.h"

/**
 * @brief Debug layers used by the application.
 */
static const std::vector<const char *> debugLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

/**
 * @brief Vulkan debug report callback.
 *
 * @param VkDebugReportFlagsEXT flags[in] - Specifying what caused a report.
 * @param VkDebugReportObjectTypeEXT objectType[in] - Specifying what vulkan object is reporting.
 * @param uint64_t object[in] - Object which triggered the report.
 * @param size_t location[in] - Location (layer, driver, loader), optional.
 * @param int32_t messageCode[in] - Layer-defined value indicating what test triggered this callback.
 * @param const char *pLayerPrefix[in] - Abbreviation of the name of the component making the callback.
 * @param const char *pMessage[in] - String detailing the trigger conditions.
 * @param void* pUserData[in] - Pointer to data given by the user.
 *
 * @return VkBool32 - Always return false.
 */
VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackEXT(
    UNUSED_PARAM VkDebugReportFlagsEXT flags,
    UNUSED_PARAM VkDebugReportObjectTypeEXT objectType,
    UNUSED_PARAM uint64_t object,
    UNUSED_PARAM size_t location,
    UNUSED_PARAM int32_t messageCode,
    UNUSED_PARAM const char *pLayerPrefix,
    const char *pMessage,
    UNUSED_PARAM void *pUserData);

/**
 * @brief Vulkan validation layers debug messenger callback.
 *
 * @param VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity[in] - Message's severity.
 * @param VkDebugUtilsMessageTypeFlagsEXT messageType[in] - Message's type.
 * @param const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData[in] - Contain the message's data.
 * @param void* pUserData[in] - Pointer to data given by the user.
 *
 * @return VkBool32 - Always return false.
 */
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(UNUSED_PARAM VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, UNUSED_PARAM VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, UNUSED_PARAM void* pUserData);

/**
 * @brief Fill the structure used to create the vulkan validation layers debug callback.
 *
 * @param VkDebugUtilsMessengerCreateInfoEXT& createInfo[out] - Messenger's create info reference to be filled, see <a href="https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#VkDebugUtilsMessengerCreateInfoEXT">VkDebugUtilsMessengerCreateInfoEXT struct</a>.
 */
void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

#endif // LAYERS_H