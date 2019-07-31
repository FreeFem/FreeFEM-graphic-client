#include <cstring>
#include <cstdio>
#include "window.h"

static bool createGLFWWindow(unsigned int width, unsigned int height, const char *title, sWindow *window)
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window->Window = glfwCreateWindow(width, height, title, 0, 0);
    if (window->Window == 0)
        return false;
    window->Dimensions.height = height;
    window->Dimensions.width = width;
    return true;
}

bool createWindow(uint32_t width, uint32_t height, const char *title, sWindow *window)
{
    if (title == 0 || window == 0)
        return false;
    if (createGLFWWindow(width, height, title, window) == false)
        return false;
    // if (createVkInstance(window) == false) {
    //     glfwDestroyWindow(window->Window);
    //     return false;
    // }
    return (true);
}

void destroyWindow(sWindow window)
{
    glfwDestroyWindow(window.Window);
}