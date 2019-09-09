#include "core/Application.h"
#include "vk/Buffer.h"

int main(int ac, char **av) {
    FEM::Application App;
    FEM::ApplicationCreateInfo AppCreateInfos = FEM::getApplicationInfos(ac, av);

    if (!FEM::newGLFWContext())
        return EXIT_FAILURE;

    if (!FEM::newApplication(&App, AppCreateInfos)) {
        LOGE("main", "Failed to initialize Application.");
        return EXIT_FAILURE;
    }

    FEM::runApplication(&App);

    FEM::destroyApplication(&App);

    return EXIT_SUCCESS;
}