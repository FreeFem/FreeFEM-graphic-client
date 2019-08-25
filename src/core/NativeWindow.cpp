#include "NativeWindow.h"

Error NativeWindow::init(uint32_t width, uint32_t height, const char *title)
{
    m_width = width;
    m_height = height;

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = glfwCreateWindow(m_width, m_height, title, 0, 0);

    if (m_window == NULL)
        return Error::FUNCTION_FAILED;

    uint32_t glfwExtensionsCount = 0;
    const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionsCount);

    m_extensions.reserve(glfwExtensionsCount);
    for (uint_fast32_t i = 0; i < glfwExtensionsCount; i += 1)
        m_extensions.push_back(glfwExtensions[i]);

    return Error::NONE;
}

void NativeWindow::destroy()
{
    glfwDestroyWindow(m_window);
    m_window = 0;
}

GLFWwindow *NativeWindow::getNativeWindow() const
{
    return m_window;
}

std::vector<const char *> NativeWindow::getNativeExtensions() const
{
    return m_extensions;
}

bool NativeWindow::isUsable() const
{
    return (m_window) ? true : false;
}