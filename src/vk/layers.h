#ifndef LAYERS_H
#define LAYERS_H

#include <vulkan/vulkan.h>
#include <vector>
#include <cstdio>

static const std::vector<const char *> debugLayers =
{
    "VK_LAYER_LUNARG_standard_validation"
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallbackEXT(VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType,
    uint64_t object,
    size_t location,
    int32_t messageCode,
    const char *pLayerPrefix,
    const char *pMessage,
    void *pUserData)
{
    dprintf(2, "Validation Layers CallBack.\n");
	return VK_FALSE;
}

#endif // LAYERS_H