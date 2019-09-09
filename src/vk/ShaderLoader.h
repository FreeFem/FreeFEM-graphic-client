#ifndef SHADER_LOADER_H_
#define SHADER_LOADER_H_

#include <vulkan/vulkan.h>
namespace FEM {
namespace VK {

struct ShaderData {
    char *ShaderName;
    VkShaderModule ShaderModule;
    ShaderData *next;
};

struct ShaderLoader {
    uint32_t ShaderCount;
    ShaderData *ShaderList;
};

inline ShaderLoader newShaderLoader(void) { return (ShaderLoader){0, 0}; }

void destroyShaderLoader(ShaderLoader Loader, const VulkanContext vkContext);

bool newShader(const char *ShaderName, const char *filepath, ShaderLoader *Loader, const VulkanContext vkContext);

void destroyShader(ShaderData *Data, const VulkanContext vkContext);

VkShaderModule searchShader(const char *Name, const ShaderLoader Loader);

} // namespace VK
} // namespace FEM

#endif // SHADER_LOADER_H_