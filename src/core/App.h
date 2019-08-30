#ifndef APP_H
#define APP_H

#include <cstdint>
#include <cstdlib>
#include "../util/Logger.h"
#include "../util/NonCopyable.h"
#include "../util/utils.h"
#include "../vk/GraphContext.h"
#include "../vk/GraphManager.h"
#include "NativeWindow.h"

namespace FEM {
/**
 * @brief Contains all the data used to initialize the application.
 */
struct AppInitInfo {
    uint32_t width;
    uint32_t height;
};

/**
 * @brief Contains all the data used by the program.
 */
class App {
   public:
    App( ) {}

    /**
     * @brief Initialize the application
     *
     * @param const AppInitInfo& initInfo[in] - Reference to a structure
     * containing all data needed to initialize the application.
     *
     * @return FORCE_USE_RESULT Error - Return Error::NONE if everything if the
     * initialization is successful. Will throw a warning at compilation if
     * result isn't checked.
     */
    FORCE_USE_RESULT ErrorValues init(const AppInitInfo &initInfo);

    /**
     * @brief Destroy the application.
     */
    void destroy( );

    /**
     * @brief Application's main loop.
     */
    ErrorValues mainLoop( );

   private:
    /**
     * @brief Wrapper around a GLFW window.
     */
    NativeWindow Window;
    /**
     * @brief Vulkan manager used to create graphic contextes.
     */
    gr::Manager GrManager;
    /**
     * @brief Vulkan graphic context used to create gr::Pipeline and render.
     */
    gr::Context GrContext;

    /**
     * @brief GLFW framebuffer resize callback.
     *
     * @param GLFWwindow *window[in] - Pointer to the window.
     * @param int width[in] - New width of the framebuffer.
     * @param int height[in] - New width of the framebuffer.
     */
    static void framebufferResizeCallback(GLFWwindow *window, int width, int height) {
        auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
        app->Window.setWidth(width);
        app->Window.setHeight(height);
        app->GrContext.destroy(app->GrManager);
        if (app->GrContext.init(app->GrManager)) {
            LOGE("GLFW FramebufferResize Callback", "Failed to recreate gr::Context.");
            return;
        }
    }

    ErrorValues update( );
};

}    // namespace FEM

#endif    // APP_H