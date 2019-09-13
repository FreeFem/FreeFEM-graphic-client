#include <cstring>
#include "ResourceManager.h"

namespace ffGraph
{
namespace Vulkan
{


ffShader SearchForShader(const char *Name, const ffShaderManager ShaderManager)
{
    if (Name == 0 || ShaderManager.empty())
        return {0, 0, 0};
    for (ffShader Shader : ShaderManager) {
        if (strcmp(Name, Shader.ShaderName) == 0)
            return Shader;
    }
    return {0, 0, 0};
}

void ffDestroyShaderManager(const VkDevice Device, ffShaderManager ShaderManager)
{
    for (ffShader Shader : ShaderManager) {
        ffDestroyShader(Device, Shader);
    }
    ShaderManager.clear();
}

}
}