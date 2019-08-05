#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "utils.h"

bool createSurfaceKHR(GLFWwindow *window, const VkInstance instance, VkSurfaceKHR &outSurfaceKHR)
{
    VkResult result = glfwCreateWindowSurface(instance, window, 0, &outSurfaceKHR);

    if (result != VK_SUCCESS) {
        dprintf(2, "Failed to create surface. [%s]\n", VkResultToStr(result));
        return false;
    }
    return true;
}