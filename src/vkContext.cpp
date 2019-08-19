#include <cstring>
#include "vkContext.hpp"
#include "layers.h"

vkContext::~vkContext()
{
    if (m_queue) {
        vkQueueWaitIdle(m_queue);
        m_queue = VK_NULL_HANDLE;
    }
}

ReturnValue vkContext::init()
{
    ReturnValue result = initInternal();

    if (result) {
        dprintf(2, "Failed to initialized Vulkan.\n");
        return result;
    }
    return result;
}

ReturnValue vkContext::initInternal()
{
    CHECK(initInstance());
    CHECK(initSurface());
    CHECK(initDevice());
}

static void checkValidationLayerSupport(std::vector<const char *> enabledLayers)
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, 0);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    printf("Activated Layers :\n");
    auto ite = enabledLayers.begin();
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
            enabledLayers.erase(ite);
        ++ite;
    }
}

ReturnValue vkContext::initInstance()
{
    const uint32_t vulkanMinor = 0;
    const uint32_t vulkanMajor = 1;

    // Create the instance
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "No Name";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "FreeFEM++ Graphic Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(0, 1, 0);

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Validation Layers
    if (LAYERS_ENABLED) {
        std::vector<const char *> layers = {{"VK_LAYER_LUNARG_standard_validation"}};
        checkValidationLayerSupport(layers);

        createInfo.enabledLayerCount = (uint32_t)layers.size();
        createInfo.ppEnabledLayerNames = layers.data();
    } else {
        createInfo.enabledLayerCount = 0;
        createInfo.ppEnabledLayerNames = 0;
    }

    // Extensions
    if (window) {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        std::vector<const char *> finalExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (LAYERS_ENABLED) {
            finalExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            finalExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }
        createInfo.enabledExtensionCount = (uint32_t)finalExtensions.size();
        createInfo.ppEnabledExtensionNames = finalExtensions.data();
    } else {
        return VK_INCOMPLETE;
    }

    CHECK_VK_FNC(vkCreateInstance(&createInfo, 0, &m_instance));

    // Setup debug callback
    if (LAYERS_ENABLED) {
        VkDebugReportCallbackCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        createInfo.pfnCallback = debugReportCallbackEXT;
        createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT
                        | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
        createInfo.pUserData = this;

        PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
            reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
                vkGetInstanceProcAddr(m_instance, "vkCreateDebugReportCallbackEXT"));
        if (!vkCreateDebugReportCallbackEXT)
            return VK_INCOMPLETE;
        vkCreateDebugReportCallbackEXT(m_instance, &createInfo, 0, &m_debugCallback);
    }
}