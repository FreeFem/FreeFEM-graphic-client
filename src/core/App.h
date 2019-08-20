#ifndef APP_H
#define APP_H

#include <cstdlib>
#include <cstdint>
#include "NativeWindow.h"
#include "../vk/GraphManager.h"
#include "../util/utils.h"
#include "../util/NonCopyable.h"

class vkContext;

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
        GraphManager m_grManager;
};

#endif // APP_H