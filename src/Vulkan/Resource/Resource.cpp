#include <vector>
#include "Resource.h"

namespace ffGraph {
namespace Vulkan {

Resource NewResources(const VkDevice& Device) {
    Resource n;

    std::vector<const char *> ShaderNames = {
        "./shaders/Geo2D.vert.spirv",
        "./shaders/Geo3D.vert.spirv",
        "./shaders/Color.frag.spirv"
    };
    std::vector<VkShaderStageFlagBits> ShaderStage = {
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_VERTEX_BIT,
        VK_SHADER_STAGE_FRAGMENT_BIT
    };

    n.Shaders.resize(ShaderNames.size());
    for (size_t i = 0; i < ShaderNames.size(); ++i) {
        n.Shaders[i] = CreateShader(ShaderNames[i], Device, ShaderStage[i]);
    }

    return n;
}

void DestroyResources(const VkDevice& Device, Resource& Resources) {
    for (auto Shader : Resources.Shaders) {
        DestroyShader(Device, Shader);
    }
}

}    // namespace Vulkan
}    // namespace ffGraph