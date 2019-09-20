#ifndef APP_H_
#define APP_H_

#include <thread>
#include "ffClient.h"
#include "Vulkan/Context.h"
#include "ResourceManager/ResourceManager.h"

namespace ffGraph {

struct ffAppCreateInfos {
    std::string Host;
    std::string Port;
};

struct ffApp {
    std::shared_ptr<std::deque<std::string>> SharedQueue;
    std::thread ClientThread;
    Vulkan::Context vkContext;
    ResourceManager RManager;
};

ffAppCreateInfos ffGetAppCreateInfos(int ac, char **av);

}

#endif // APP_H_