#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "ffShader.h"
#include "Logger.h"

namespace ffGraph
{
namespace Vulkan
{

ffShader ffCreateShader(const char *Name, const char *FilePath, const VkDevice Device, VkShaderStageFlagBits Stage)
{
    ffShader n;

    memset(&n, 0, sizeof(ffShader));
    int fd = open(FilePath, O_RDONLY);
    struct stat st;

    if (fd == -1) return n;
    if (fstat(fd, &st) == -1) return n;
    size_t size = st.st_size;

    uint8_t *bytes = (uint8_t *)malloc(sizeof(uint8_t) * size);
    if (read(fd, bytes, size) == -1) return n;

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = (uint32_t *)bytes;
    if (vkCreateShaderModule(Device, &createInfo, 0, &n.Module)) {
        LogError(GetCurrentLogLocation(), "Failed to create VkShaderModule for Shader : %s.", Name);
        return n;
    }
    n.Stage = Stage;
    n.ShaderName = strdup(Name);
    if (n.ShaderName == 0) {
        LogError(GetCurrentLogLocation(), "Failed to allocate memory for Shader name %s.", Name);
        vkDestroyShaderModule(Device, n.Module, 0);
        n.Module = VK_NULL_HANDLE;
        return n;
    }
    return n;
}

void ffDestroyShader(const VkDevice Device, ffShader Shader)
{
    vkDestroyShaderModule(Device, Shader.Module, 0);
    free(Shader.ShaderName);
}

} // namespace Vulkan
} // namespace ffGraph
