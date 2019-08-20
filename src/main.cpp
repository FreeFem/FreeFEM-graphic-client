#include "core/App.h"

int main()
{
    App app;
    AppInitInfo appInitInfo = {1280, 768};

    if (app.init(appInitInfo))
        return EXIT_FAILURE;
    if (!app.mainLoop())
        return EXIT_FAILURE;
    app.destroy();
    return EXIT_SUCCESS;
}