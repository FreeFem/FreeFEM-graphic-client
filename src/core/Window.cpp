#include <cstring>
#include <cstdlib>
#include "Window.h"

namespace FEM {

bool newGLFWContext(void)
{
    if (glfwInit() != GLFW_TRUE)
        return false;
    return true;
}

bool newWindow(Window *Win, uint32_t width, uint32_t height, const char *title)
{
    if (width < 1 || height < 1 || title == NULL)
        return false;
    Win->ScreenWidth = width;
    Win->ScreenHeight = height;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    Win->Handle = glfwCreateWindow(width, height, title, 0, 0);

    if (Win->Handle == 0) {
        LOGE(FILE_LOCATION(), "Failed to create a window.");
        return false;
    }
    uint32_t glfwExtensionCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    if (glfwExtensionCount == 0)
        return false;
    Win->VulkanExtensions = (char **)malloc(sizeof(char *) * (glfwExtensionCount + 1));
    for (uint32_t i = 0; i < glfwExtensionCount; i += 1) {
        Win->VulkanExtensions[i] = strdup(glfwExtensions[i]);
    }
    Win->VulkanExtensionsCount = glfwExtensionCount;
    Win->VulkanExtensions[glfwExtensionCount] = 0;
    return true;
}

void destroyWindow(Window *Win)
{
    glfwDestroyWindow(Win->Handle);
    for (uint32_t i = 0; Win->VulkanExtensions[i] != 0; i += 1) {
        free(Win->VulkanExtensions[i]);
    }
    free(Win->VulkanExtensions);
}

bool setWindowSize(Window *Win, uint32_t width, uint32_t height)
{
    uint32_t fWidth, fHeight = 0;

    if (width == -1)
        fWidth = Win->ScreenWidth;
    else
        fWidth = width;
    if (height == -1)
        fHeight = Win->ScreenHeight;
    else
        fHeight = height;

    glfwSetWindowSize(Win->Handle, fWidth, fHeight);
    Win->ScreenHeight = fHeight;
    Win->ScreenWidth = fWidth;
    return true;
}

}