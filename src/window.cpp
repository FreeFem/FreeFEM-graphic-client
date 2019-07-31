#include <cstring>
#include "window.h"

static bool createGLFWWindow(unsigned int width, unsigned int height, const char *title, sWindow *window)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window->Window = glfwCreateWindow(width, height, title, 0, 0);
    if (window->Window == 0)
        return false;
    window->Dimensions.height = height;
    window->Dimensions.width = width;
    return true;
}

static bool createVkInstance(sWindow *window)
{
    VkApplicationInfo appInfo;

    memset(&appInfo, 0, sizeof(VkApplicationInfo));
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(VkInstanceCreateInfo));
    createInfo.pApplicationInfo = &appInfo;

#ifdef _DEBUG
    const char *debugLayers[] = {
        "VK_LAYER_LUNARG_standard_validation"
    };
    createInfo.ppEnabledLayerNames = debugLayers;
    craeteInfo.enabledLayerCount = sizeof(debugLayers) / sizeof(debugLayers[0]);
#endif
    const char *extensions[] = {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef _DEBUG
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
    };
    createInfo.ppEnabledExtensionNames = extensions;
    createInfo.enabledExtensionCount = sizeof(extensions) / sizeof(extensions[0]);
}

bool createWindow(uint32_t width, uint32_t height, const char *title, sWindow *window)
{
    if (title == 0 || window == 0)
        return false;
    if (createGLFWWindow(width, height, title, window) == false)
        return false;
    /*
        Init Vulkan here
    */
    return (true);
}

void destroyWindow(sWindow window)
{
    // vkDestroyInstance(window.Instance, 0);
    glfwDestroyWindow(window.Window);
}