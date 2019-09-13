#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <vulkan/vulkan.h>
#include <vector>
#include <deque>
#include <string>
#include "vk_mem_alloc.h"
#include "ffShader.h"

namespace ffGraph
{
namespace Vulkan
{


typedef std::vector<ffShader> ffShaderManager;

struct ResourceManager {
    VmaAllocator Allocator;
    VkDevice *DeviceREF;

    std::deque<std::string>* RawJSONData = {};
    ffShaderManager ShaderManager;
};

ffShader SearchForShader(const char *Name, const ffShaderManager ShaderManager);

void ffDestroyShaderManager(const VkDevice Device, ffShaderManager ShaderManager);

} // namespace Vulkan
} // namespace ffGraph

#endif // RESOURCE_MANAGER_H_