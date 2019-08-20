#include "App.h"

Error App::init(const AppInitInfo& initInfo)
{
    if (glfwInit() != GLFW_TRUE)
        return Error::FUNCTION_FAILED;
    if (m_window.init(initInfo.width, initInfo.height, "FreeFEM++"))
        return Error::FUNCTION_FAILED;
    GraphManagerInitInfo grInitInfo = {m_window};
    if (m_grManager.init(grInitInfo))
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

void App::destroy()
{
    m_window.destroy();
}

Error App::mainLoop()
{
    bool Quit = false;

    while (!Quit) {
        glfwPollEvents();
        if (glfwWindowShouldClose(m_window.getNativeWindow()))
            Quit = true;
    }
    return Error::NONE;
}