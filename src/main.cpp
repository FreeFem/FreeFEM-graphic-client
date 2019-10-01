#include <cstring>
#include "App.h"
#include "Vulkan/Instance.h"

ffGraph::ffAppCreateInfos ffGraph::ffGetAppCreateInfos(int ac, char** av) {
    ffAppCreateInfos Infos = {"localhost", "12345", 1280, 768};

    if (ac < 2)
        return Infos;
    else {
        for (int i = 1; i < ac; i += 1) {
            if (strcmp(av[i], "-Host") == 0) {
                Infos.Host.clear( );
                Infos.Host.append(av[i + 1]);
            } else if (strcmp(av[i], "-Port") == 0) {
                Infos.Port.clear( );
                Infos.Port.append(av[i + 1]);
            } else if (strcmp(av[i], "-ScreenWidth") == 0) {
                Infos.width = atoi(av[i + 1]);
            } else if (strcmp(av[i], "-ScreenHeight") == 0) {
                Infos.height = atoi(av[i + 1]);
            }
        }
    }
    return Infos;
}

namespace ffGraph {

bool ffAppInitialize(ffAppCreateInfos pCreateInfos, std::shared_ptr<std::deque<std::string>>& SharedQueue, ffApp& App) {
    App.SharedQueue = SharedQueue;
    App.vkInstance.load("FreeFem", pCreateInfos.width, pCreateInfos.height);
    return true;
}

void ffAppRun(ffApp& App) { App.vkInstance.run(App.SharedQueue); }

}    // namespace ffGraph

int main(int ac, char** av) {
    ffGraph::ffAppCreateInfos AppCreateInfos = ffGraph::ffGetAppCreateInfos(ac, av);
    ffGraph::ffApp App;
    std::shared_ptr<std::deque<std::string>> SharedQueue = std::make_shared<std::deque<std::string>>( );
    ffGraph::ffAppInitialize(AppCreateInfos, SharedQueue, App);
    ffGraph::ffClient Client(AppCreateInfos.Host, AppCreateInfos.Port, App.SharedQueue);

    App.ClientThread = std::thread([&Client]( ) { Client.Start( ); });
    ffGraph::ffAppRun(App);
    Client.Stop( );
    App.vkInstance.destroy( );
    App.ClientThread.join( );
    return 0;
}
