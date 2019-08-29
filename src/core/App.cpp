#include <chrono>
#include "App.h"
#include "../vk/Buffers.h"

namespace FEM
{

    Error App::init(const AppInitInfo& initInfo)
    {
        if (glfwInit() != GLFW_TRUE)
            return Error::FUNCTION_FAILED;
        if (m_window.init(initInfo.width, initInfo.height, "FreeFEM++"))
            return Error::FUNCTION_FAILED;
        glfwSetWindowUserPointer(m_window.getNativeWindow(), this);
        glfwSetFramebufferSizeCallback(m_window.getNativeWindow(), framebuffferResizeCallback);

        gr::ManagerInitInfo grInitInfo = {m_window};
        if (m_grManager.init(grInitInfo))
            return Error::FUNCTION_FAILED;
        if (m_grContext.init(m_grManager))
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
            if (update()) {
                return Error::FUNCTION_FAILED;
            }
            m_grContext.render(m_grManager);
            m_grContext.swapBuffer();
        }
        return Error::NONE;
    }

    Error App::update()
    {
        static int i = 0;
        if (!i) {
            gr::VertexBuffer object;
            if (object.init(m_grManager, (void *)vertices, NUM_DEMO_VERTICES, SIZE_DEMO_VERTEX,
                {{0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 0}, {1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3}},
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST))
            {
                return Error::FUNCTION_FAILED;
            }
            m_grContext.addPipeline(m_grManager, object, "shaders/vertex.spirv", "shaders/fragment.spirv");
            i = 1;
        }
        m_grContext.m_animTime += 0.001f;
        return Error::NONE;
    }

} // namespace FEM