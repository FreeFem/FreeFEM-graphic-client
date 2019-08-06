#include <cstring>
#include "utils.h"
#include "layers.h"

std::vector<const char *> GetRequiredExtensions()
{
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    std::vector<const char *> finalExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
#if _DEBUG
    finalExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
    finalExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif
    return finalExtensions;
}

bool checkValidationLayerSupport(std::vector<const char *> enabledLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, 0);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    printf("Activated Layers :\n");
    for (const char *layerName : enabledLayers) {
        bool layerFound = false;
        for (const auto& layerProps : availableLayers) {
            if (strcmp(layerName, layerProps.layerName) == 0) {
                printf("* %s\n", layerProps.layerName);
                layerFound = true;
                break;
            }
        }
        if (!layerFound)
            return false;
    }
    return true;
}

bool createInstance(VkInstance& outInstance, std::vector<const char *> enabledExtensions, std::vector<const char *> enabledLayers)
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = 0;
    appInfo.pApplicationName = "FreeFEM Graphic Client";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 1);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);

    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = 0;
    instanceCreateInfo.flags = 0;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    instanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
#if _DEBUG
    if (!checkValidationLayerSupport(enabledLayers))
        dprintf(2, "Layer not found.\n");
    instanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
    instanceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

    populateDebugMessengerCreateInfo(debugCreateInfo);
    instanceCreateInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
#else
    instanceCreateInfo.enabledLayerCount = 0;
    instanceCreateInfo.pNext = 0;
#endif
    VkResult result = vkCreateInstance(&instanceCreateInfo, 0, &outInstance);
    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to create instance. [%s]\n", VkResultToStr(result));
        return false;
    }
    return true;
}