#include <cstring>
#include "ffHandle.h"
#include "ffShaderManager.h"

namespace ffGraph {

Vulkan::ffShader ffShaderManager_SearchFor(const ffShaderManager& Manager, ffHandle Handle) {
    Vulkan::ffShader EmptyShader;
    memset(&EmptyShader, 0, sizeof(Vulkan::ffShader));

    if (Handle.Data.Type != ffHandleType::FF_HANDLE_TYPE_SHADER)
        return EmptyShader;
    if (Manager[Handle.Data.Cell].UniqueBytes == Handle.Data.UniqueBytes)
        return Manager[Handle.Data.Cell].Data;
    return EmptyShader;
}

void ffShaderManager_DestroyItem(ffShaderManager& Manager, const VkDevice& Device, ffHandle Handle)
{
    if (!Manager[Handle.Data.Cell].isUsed || Manager[Handle.Data.Cell].UniqueBytes != Handle.Data.UniqueBytes)
        return;

    Vulkan::ffDestroyShader(Device, Manager[Handle.Data.Cell].Data);
    Manager[Handle.Data.Cell].isUsed = false;
    Manager[Handle.Data.Cell].UniqueBytes += 1;
    memset(&Manager[Handle.Data.Cell].Data, 0, sizeof(Vulkan::ffShader));
}

void ffShaderManager_Destroy(ffShaderManager& Manager, const VkDevice& Device) {
    for (auto& ShaderHandle : Manager) {
        ShaderHandle.isUsed = false;
        ShaderHandle.UniqueBytes += 1;
        Vulkan::ffDestroyShader(Device, ShaderHandle.Data);
    }
}

}    // namespace ffGraph