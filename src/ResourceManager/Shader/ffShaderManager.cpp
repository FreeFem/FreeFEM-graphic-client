#include <cstring>
#include "ffShaderManager.h"

namespace ffGraph {

Vulkan::ffShader ffShaderManager_SearchFor(const ffShaderManager& Manager, const std::string& Key) {
    Vulkan::ffShader EmptyShader;
    memset(&EmptyShader, 0, sizeof(Vulkan::ffShader));

    for (const auto& ShaderHandle : Manager) {
        if (ShaderHandle.Key.compare(Key) == 0) return ShaderHandle.Data;
    }
    return EmptyShader;
}

void ffShaderManager_Destroy(ffShaderManager& Manager, const VkDevice& Device) {
    for (auto& ShaderHandle : Manager) {
        Vulkan::ffDestroyShader(Device, ShaderHandle.Data);
        ShaderHandle.Key.clear( );
    }
}

}    // namespace ffGraph