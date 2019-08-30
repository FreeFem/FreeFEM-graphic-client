#include "App.h"
#include <chrono>

namespace FEM {

ErrorValues App::init(const AppInitInfo& initInfo) {
    LOGI(FILE_LOCATION( ), "Application Initialization.");
    if (glfwInit( ) != GLFW_TRUE) {
        LOGE(FILE_LOCATION( ), "glfwInit failed, no window can be opened.");
        return ErrorValues::FUNCTION_FAILED;
    }
    if (Window.init(initInfo.width, initInfo.height, "FreeFEM++")) {
        LOGE(FILE_LOCATION( ), "Failed to create NativeWindow.");
        return ErrorValues::FUNCTION_FAILED;
    }
    glfwSetWindowUserPointer(Window.getNativeWindow( ), this);
    glfwSetFramebufferSizeCallback(Window.getNativeWindow( ), framebufferResizeCallback);

    gr::ManagerInitInfo grInitInfo = {Window};
    if (GrManager.init(grInitInfo)) {
        LOGE(FILE_LOCATION( ), "Failed to create gr::Manager.");
        return ErrorValues::FUNCTION_FAILED;
    }
    if (GrContext.init(GrManager)) {
        LOGE(FILE_LOCATION( ), "Failed to create gr::Context.");
        return ErrorValues::FUNCTION_FAILED;
    }
    return ErrorValues::NONE;
}

void App::destroy( ) {
    GrContext.destroy(GrManager);
    GrManager.destroy();
    Window.destroy( );
}

ErrorValues App::mainLoop( ) {
    bool Quit = false;

    while (!Quit) {
        glfwPollEvents( );
        if (glfwWindowShouldClose(Window.getNativeWindow( ))) {
            LOGI("", "Closing window.");
            Quit = true;
        }
        // if (update()) {
        //     return ErrorValues::FUNCTION_FAILED;
        // }
        // GrContext.render(GrManager);
    }
    return ErrorValues::NONE;
}

ErrorValues App::update( ) {
    // static int i = 0;
    // if (!i) {
    //     gr::VertexBuffer object;
    //     if (object.init(GrManager, (void *)vertices, NUM_DEMO_VERTICES,
    //     SIZE_DEMO_VERTEX,
    //         {{0, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 0}, {1,
    //         VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3}},
    //         VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST))
    //     {
    //         return ErrorValues::FUNCTION_FAILED;
    //     }
    //     GrContext.addPipeline(GrManager, object, "shaders/vertex.spirv",
    //     "shaders/fragment.spirv"); i = 1;
    // }
    // GrContext.m_animTime += 0.001f;
    return ErrorValues::NONE;
}

}    // namespace FEM