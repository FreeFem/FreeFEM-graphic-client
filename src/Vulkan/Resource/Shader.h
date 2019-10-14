/**
 * @file Shader.h
 * @brief Declaration of Shader data type and it's related functions.
 */
#ifndef FF_SHADER_H_
#define FF_SHADER_H_

#include <vulkan/vulkan.h>
#include <vector>
#include <cstring>

namespace ffGraph {
namespace Vulkan {

/**
 * @brief Stores a ShaderModule
 */
struct Shader {
    char Name[64];
    VkShaderStageFlags Stage;
    VkShaderModule Module;
};

using ShaderLibrary = std::vector<Shader>;

/**
 * @brief Look if the ffGraph::Vulkan::Shader creation was successful.
 *
 * @param Shader [in] - Shader on which the test is performed.
 * @return bool - Boolean value (true : ffGraph::Vulkan::Shader is read / false : ffGraph::Vulkan::Shader isn't ready).
 */
inline bool ffIsShaderReady(Shader Shader) { return (Shader.Module == VK_NULL_HANDLE) ? false : true; }

/**
 * @brief Create a new ffGraph::Vulkan::Shader
 *
 * @param const char *FilePath [in] - SPIRV file location.
 * @param const VkDevice [in] - Device used to create the ShaderModule.
 * @param VkShaderStageFlags Stage [in] - Pipeline stage of the shader.
 *
 * @return ffGraph::Vulkan::Shader - A new ffGraph::Vulkan::Shader (use ffGraph::Vulkan::ffIsShaderReady to check return
 * value).
 */
Shader ImportShader(const char* FilePath, const VkDevice& Device, VkShaderStageFlags Stage);

/**
 * @brief Destroy a ffGraph::Vulkan::Shader, releasing it's memory.
 *
 * @param const VkDevice Device [in] - VkDevice used to create the ffGraph::Vulkan::Shader.
 * @param ffGraph::Vulkan::Shader Shader [in] - ffGraph::Vulkan::Shader to destroy.
 *
 * @return void
 */
void DestroyShader(const VkDevice Device, Shader Shader);

/**
 * @brief Search for a shader using its name.
 */
inline VkShaderModule FindShader(const ShaderLibrary& Library, const char* Name) {
    for (size_t i = 0; i < Library.size( ); ++i) {
        if (strcmp(Name, Library[i].Name) == 0) return Library[i].Module;
    }
    return VK_NULL_HANDLE;
}

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // FF_SHADER_H_