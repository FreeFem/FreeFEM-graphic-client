#include <cstring>
#include "NativeWindow.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

NativeWindow ffNewWindow(VkExtent2D Size)
{
    NativeWindow n;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    n.Handle = glfwCreateWindow(Size.width, Size.height, "FreeFEM", 0, 0);
    n.WindowSize = Size;

    uint32_t glfwExtensionsCount = 0;

    const char **extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);
    n.SurfaceExtensions.reserve(glfwExtensionsCount);

    n.SurfaceExtensions.assign(extensions, extensions + glfwExtensionsCount);
    return n;
}

void ffDestroyWindow(NativeWindow Window)
{
    glfwDestroyWindow(Window.Handle);
}

VkSurfaceKHR ffGetSurface(const VkInstance& Instance, const NativeWindow& Window)
{
    VkSurfaceKHR Surface = VK_NULL_HANDLE;

    VkResult res = glfwCreateWindowSurface(Instance, Window.Handle, 0, &Surface);
    if (res != VK_SUCCESS) {
        LogError(GetCurrentLogLocation(), "glfwCreateWindowSurface failed %d.", res);
        return VK_NULL_HANDLE;
    }
    return Surface;
}

}
}