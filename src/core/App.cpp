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

    if (GrRenderer.init(GrManager, GrContext)) {
        LOGE(FILE_LOCATION( ), "Failed to create gr::Renderer.");
        return ErrorValues::FUNCTION_FAILED;
    }
    return ErrorValues::NONE;
}

void App::destroy( ) {
    vkDeviceWaitIdle(GrManager.Device);
    GrRenderer.destroy(GrManager, GrContext);
    GrContext.destroy(GrManager);
    GrManager.destroy( );
    Window.destroy( );
}

ErrorValues App::mainLoop( ) {
    bool Quit = false;
    GrRenderer.Cam.setPerspective(90.f, (float)((float)GrManager.Window->getWidth() / (float)GrManager.Window->getHeight()), 0.1f, 1000.f);
    GrRenderer.Cam.setPosition(glm::vec3(3.f, 3.f, 3.f));

    while (!Quit) {
        glfwPollEvents( );
        if (glfwWindowShouldClose(Window.getNativeWindow( ))) {
            LOGI("", "Closing window.");
            Quit = true;
        }
        update();
        GrRenderer.render(GrManager, GrContext);
        GrContext.SwapBuffer();
    }
    return ErrorValues::NONE;
}

ErrorValues App::update( ) {
    GrRenderer.update();
    return ErrorValues::NONE;
}

}    // namespace FEM