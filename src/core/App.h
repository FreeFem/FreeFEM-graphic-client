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

    struct AppInitInfo {
        uint32_t width;
        uint32_t height;
    };

    class App {
        public:
            App() { }

            FORCE_USE_RESULT Error init(const AppInitInfo& initInfo);

            void destroy();

            Error mainLoop();

        private:
            NativeWindow m_window;
            gr::Manager m_grManager;
            gr::Context m_grContext;

            static void framebuffferResizeCallback(GLFWwindow *window, int width, int height)
            {
                auto app = reinterpret_cast<App *>(glfwGetWindowUserPointer(window));
                app->m_window.setWidth(width);
                app->m_window.setHeight(height);
                app->m_grContext.reload(app->m_grManager);
            }
    };

} // namespace FEM

#endif // APP_H