#include "core/App.h"
#include "vk/Camera.h"

int main( ) {
    FEM::App app;
    FEM::AppInitInfo appInitInfo = {1280, 768};

    if (app.init(appInitInfo)) {
        LOGE(FILE_LOCATION( ), "Failed to init FEM::App.");
        return EXIT_FAILURE;
    }
    if (app.mainLoop( )) return EXIT_FAILURE;
    app.destroy( );
    return EXIT_SUCCESS;
}