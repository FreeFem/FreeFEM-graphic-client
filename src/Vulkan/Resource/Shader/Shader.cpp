#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "Shader.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

Shader CreateShader(const char *FilePath, const VkDevice &Device, VkShaderStageFlags Stage) {
    Shader n;

    memset(&n, 0, sizeof(Shader));
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
        LogError(GetCurrentLogLocation( ), "Failed to create VkShaderModule for Shader : %s.", FilePath);
        return n;
    }
    n.Stage = Stage;
    return n;
}

void DestroyShader(const VkDevice Device, Shader Shader) { vkDestroyShaderModule(Device, Shader.Module, 0); }

}    // namespace Vulkan
}    // namespace ffGraph
