#include "App.h"
#include <chrono>

namespace FEM {

ErrorValues App::init(const AppInitInfo& initInfo) {
    LOGI(FILE_LOCATION( ), "Application Initialization.");
    if (glfwInit( ) != GLFW_TRUE) {
        LOGE(FILE_LOCATION( ), "glfwInit failed, no window can be opened.");
        return ErrorValues::FUNCTION_FAILED;
    }
    if (m_window.init(initInfo.width, initInfo.height, "FreeFEM++")) {
        LOGE(FILE_LOCATION( ), "Failed to create NativeWindow.");
        return ErrorValues::FUNCTION_FAILED;
    }
    glfwSetWindowUserPointer(m_window.getNativeWindow( ), this);
    glfwSetFramebufferSizeCallback(m_window.getNativeWindow( ), framebuffferResizeCallback);

    gr::ManagerInitInfo grInitInfo = {m_window};
    if (m_grManager.init(grInitInfo)) {
        LOGE(FILE_LOCATION( ), "Failed to create gr::Manager.");
        return ErrorValues::FUNCTION_FAILED;
    }
    if (m_grContext.init(m_grManager)) {
        LOGE(FILE_LOCATION( ), "Failed to create gr::Context.");
        return ErrorValues::FUNCTION_FAILED;
    }
    return ErrorValues::NONE;
}

void App::destroy( ) { m_window.destroy( ); }

ErrorValues App::mainLoop( ) {
    bool Quit = false;

    while (!Quit) {
        glfwPollEvents( );
        if (glfwWindowShouldClose(m_window.getNativeWindow( ))) {
            LOGI("", "Closing window.");
            Quit = true;
        }
        // if (update()) {
        //     return ErrorValues::FUNCTION_FAILED;
        // }
        // m_grContext.render(m_grManager);
    }
    return ErrorValues::NONE;
}

ErrorValues App::update( ) {
    // static int i = 0;
    // if (!i) {
    //     gr::VertexBuffer object;
    //     if (object.init(m_grManager, (void *)vertices, NUM_DEMO_VERTICES,
    //     SIZE_DEMO_VERTEX,
    //         {{0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 0}, {1,
    //         VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3}},
    //         VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST))
    //     {
    //         return ErrorValues::FUNCTION_FAILED;
    //     }
    //     m_grContext.addPipeline(m_grManager, object, "shaders/vertex.spirv",
    //     "shaders/fragment.spirv"); i = 1;
    // }
    // m_grContext.m_animTime += 0.001f;
    return ErrorValues::NONE;
}

}    // namespace FEM