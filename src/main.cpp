#include <cstdlib>
#include <cstring>
#include "window.h"

int main()
{
    if (glfwInit() == GLFW_FALSE)
        return 1;
    volkInitialize();
    sWindow window;
    memset(&window, 0, sizeof(sWindow));
    createWindow(1280, 769, "Title", &window);

    while (!glfwWindowShouldClose(window.Window)) {
        glfwPollEvents();
    }

    destroyWindow(window);
    glfwTerminate();
    return 0;
}