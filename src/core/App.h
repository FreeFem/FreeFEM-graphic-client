#ifndef APP_H
#define APP_H

#include <cstdlib>
#include <cstdint>
#include "NativeWindow.h"
#include "../vk/GraphManager.h"
#include "../vk/GraphContext.h"
#include "../util/utils.h"
#include "../util/NonCopyable.h"

namespace FEM
{
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
            App() { }

            /**
             * @brief Initialize the application
             *
             * @param const AppInitInfo& initInfo[in] - Reference to a structure containing all data needed to initialize the application.
             *
             * @return FORCE_USE_RESULT Error - Return Error::NONE if everything if the initialization is successful.
             * Will throw a warning at compilation if result isn't checked.
             */
            FORCE_USE_RESULT Error init(const AppInitInfo& initInfo);

            /**
             * @brief Destroy the application.
             */
            void destroy();

            /**
             * @brief Application's main loop.
             */
            Error mainLoop();

        private:
            /**
             * @brief Wrapper around a GLFW window.
             */
            NativeWindow m_window;
            /**
             * @brief Vulkan manager used to create graphic contextes.
             */
            gr::Manager m_grManager;
            /**
             * @brief Vulkan graphic context used to create gr::Pipeline and render.
             */
            gr::Context m_grContext;

            /**
             * @brief GLFW framebuffer resize callback.
             *
             * @param GLFWwindow *window[in] - Pointer to the window.
             * @param int width[in] - New width of the framebuffer.
             * @param int height[in] - New width of the framebuffer.
             */
            static void framebuffferResizeCallback(GLFWwindow *window, int width, int height)
            {
                auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
                app->m_window.setWidth(width);
                app->m_window.setHeight(height);
                if (app->m_grContext.reload(app->m_grManager))
                    return;
            }

            Error update();
    };

} // namespace FEM

#endif // APP_H