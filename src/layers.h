#ifndef EXTENSIONS_H
#define EXTENSIONS_H

#include <vulkan/vulkan.h>
#include <cstdio>
#include "utils.h"


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

#endif // EXTENSIONS_H