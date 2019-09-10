#ifndef WINDOW_H_
#define WINDOW_H_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "../util/Logger.h"

namespace FEM {

struct Window {
    GLFWwindow *Handle;
    uint32_t ScreenWidth;
    uint32_t ScreenHeight;
    uint32_t VulkanExtensionsCount;
    char **VulkanExtensions;
};

bool newWindow(Window *Win, uint32_t width, uint32_t height, const char *title);

void destroyWindow(Window *Win);

bool setWindowSize(Window *Win, uint32_t width, uint32_t height);

bool newGLFWContext(void);

}    // namespace FEM

#endif    // WINDOW_H_