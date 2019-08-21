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
    };

} // namespace FEM

#endif // APP_H