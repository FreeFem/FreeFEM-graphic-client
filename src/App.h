#ifndef APP_H_
#define APP_H_

#include <thread>
#include "ffClient.h"
#include "Vulkan/Instance.h"
#include "JSON/ThreadQueue.h"
namespace ffGraph {

struct ffAppCreateInfos {
    std::string Host;
    std::string Port;
    uint32_t width;
    uint32_t height;
};

struct ffApp {
    std::shared_ptr<std::deque<std::string>> SharedQueue;
    JSON::ThreadSafeQueue GeometryQueue;
    std::thread ClientThread;
    uint16_t GeometryInternID = 0;
    Vulkan::Instance vkInstance;
};

ffAppCreateInfos ffGetAppCreateInfos(int ac, char** av);

bool ffAppInitialize(ffAppCreateInfos pCreateInfos, std::shared_ptr<std::deque<std::string>>& SharedQueue, ffApp& App);

}    // namespace ffGraph

#endif    // APP_H_