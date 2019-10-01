#ifndef APP_H_
#define APP_H_

#include <thread>
#include "ffClient.h"
#include "Vulkan/Instance.h"

namespace ffGraph {

struct ffAppCreateInfos {
    std::string Host;
    std::string Port;
    uint32_t width;
    uint32_t height;
};

struct ffApp {
    std::shared_ptr<std::deque<std::string>> SharedQueue;
    std::thread ClientThread;
    Vulkan::Instance vkInstance;
};

ffAppCreateInfos ffGetAppCreateInfos(int ac, char **av);

bool ffAppInitialize(ffAppCreateInfos pCreateInfos, std::shared_ptr<std::deque<std::string>>& SharedQueue, ffApp& App);

}

#endif // APP_H_