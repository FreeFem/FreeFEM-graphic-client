#ifndef FF_SHADER_MANAGER_H_
#define FF_SHADER_MANAGER_H_

#include <vector>
#include <string>
#include "ffShader.h"

namespace ffGraph
{

enum ffShaderStage {
    e_ffVertexShaderStage = VK_SHADER_STAGE_VERTEX_BIT,
    e_ffFragmentShaderStage = VK_SHADER_STAGE_FRAGMENT_BIT
};

struct ffShaderHandle {
    std::string Key;
    Vulkan::ffShader Data;
};

typedef std::vector<ffShaderHandle> ffShaderManager;

Vulkan::ffShader ffShaderManager_SearchFor(const ffShaderManager& Manager, const std::string& Key);

void ffShaderManager_Destroy(ffShaderManager& Manager, const VkDevice& Device);

} // namespace ffGraph


#endif // FF_SHADER_MANAGER_H_