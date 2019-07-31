#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <iostream>
#include "context.h"
#include "window.h"

int main()
{
    try {
        if (glfwInit() == GLFW_FALSE)
            return 1;
        volkInitialize();
        VulkanContext context;
        memset(&context, 0, sizeof(VulkanContext));
        createVulkanContext(&context);

        sWindow window;
        memset(&window, 0, sizeof(sWindow));
        createWindow(1280, 769, "Title", &window);

        while (!glfwWindowShouldClose(window.Window)) {
            glfwPollEvents();
        }

        destroyWindow(window);
        destroyVulkanContext(context);
        glfwTerminate();
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
    }
    return 0;
}