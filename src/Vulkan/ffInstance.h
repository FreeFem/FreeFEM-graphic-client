#ifndef INSTANCE_H_
#define INSTANCE_H_

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

namespace ffGraph
{
namespace Vulkan
{

struct ffInstance {
    VkInstance Handle;
    std::vector<const char *> Extensions;
#ifdef _DEBUG
    VkDebugUtilsMessengerEXT DebugMessenger;
#endif
};

ffInstance ffNewInstance(std::vector<std::string> Extensions, std::vector<std::string> Layers);

void ffDestroyInstance(ffInstance Instance);

} // namespace Vulkan
} // namespace ffGraph

#endif // INSTANCE_H_