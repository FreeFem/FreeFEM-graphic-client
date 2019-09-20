#include <cstring>
#include "App.h"

ffGraph::ffAppCreateInfos ffGraph::ffGetAppCreateInfos(int ac, char **av)
{
    ffAppCreateInfos Infos = { "localhost", "12345" };

    if (ac < 2)
        return Infos;
    else {
        for (int i = 1; i < ac; i += 1) {
            if (strcmp(av[i], "-Host") == 0) {
                Infos.Host.clear();
                Infos.Host.append(av[i + 1]);
            } else if (strcmp(av[i], "-Port") == 0) {
                Infos.Port.clear();
                Infos.Port.append(av[i + 1]);
            }
        }
    }
    return Infos;
}

void PrintVertices(void *data, size_t idx)
{
    if (idx % 7 == 0)
        std::cout << "\n";
    std::cout << ((float *)data)[0] << ", ";
}

void PrintIndices(void *data, size_t idx)
{
    std::cout << ((unsigned short int *)data)[0] << ", ";
}

int main(int ac, char **av)
{
    ffGraph::ffAppCreateInfos AppCreateInfos = ffGraph::ffGetAppCreateInfos(ac, av);
    ffGraph::ffApp App;
    App.SharedQueue = std::make_shared<std::deque<std::string>>();
    if (ffGraph::Vulkan::ffnewContext(&App.vkContext) == false)
        return 1;
    // if (ffGraph::Vulkan::newResourceManager(&App.RManager, &App.vkContext.Device, App.vkContext.PhysicalDevice) == false)
    //     return 1;
    // ffGraph::ffClient Client(AppCreateInfos.Host, AppCreateInfos.Port, App.SharedQueue);

    // App.ClientThread = std::thread( [ &Client ]( ){ Client.Start(); } );
    // while (1) {
    //     if (!App.SharedQueue->empty()) {
    //         ffGraph::ffMesh test = ffGraph::ffCreateMeshFromString(App.RManager.Allocator, App.SharedQueue->at(0));
    //         std::cout << "Vertices :\n";
    //         ffGraph::ffArrayForEach(test.Vertices, PrintVertices);
    //         std::cout << "\nIndices :\n";
    //         ffGraph::ffArrayForEach(test.Indices, PrintIndices);
    //         std::cout << "\n";
    //         App.SharedQueue->pop_front();
    //     }
    // }
    // Client.Stop();
    // App.ClientThread.join();
    return 0;
}
