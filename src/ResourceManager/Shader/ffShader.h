/**
 * @file ffShader.h
 * @brief Declaration of ffShader data type and it's related functions.
 */
#ifndef FF_SHADER_H_
#define FF_SHADER_H_

#include <vulkan/vulkan.h>

namespace ffGraph {
namespace Vulkan {

/**
 * @brief Stores a ShaderModule
 */
struct ffShader {
    // @brief Name used to query the Shader
    char *ShaderName;
    VkShaderStageFlags Stage;
    VkShaderModule Module;
};

/**
 * @brief Look if the ffGraph::Vulkan::Shader creation was successful.
 *
 * @param ffShader [in] - ffShader on which the test is performed.
 * @return bool - Boolean value (true : ffGraph::Vulkan::Shader is read / false : ffGraph::Vulkan::Shader isn't ready).
 */
inline bool ffIsShaderReady(ffShader Shader) { return (Shader.ShaderName == VK_NULL_HANDLE) ? false : true; }

/**
 * @brief Create a new ffGraph::Vulkan::Shader
 *
 * @param const char *Name [in] - Name of the Shader used to query.
 * @param const char *FilePath [in] - SPIRV file location.
 * @param const VkDevice [in] - Device used to create the ShaderModule.
 * @param VkShaderStageFlags Stage [in] - Pipeline stage of the shader.
 *
 * @return ffGraph::Vulkan::Shader - A new ffGraph::Vulkan::Shader (use ffGraph::Vulkan::ffIsShaderReady to check return
 * value).
 */
ffShader ffCreateShader(const char *Name, const char *FilePath, const VkDevice Device, VkShaderStageFlags Stage);

/**
 * @brief Destroy a ffGraph::Vulkan::Shader, releasing it's memory.
 *
 * @param const VkDevice Device [in] - VkDevice used to create the ffGraph::Vulkan::Shader.
 * @param ffGraph::Vulkan::Shader Shader [in] - ffGraph::Vulkan::Shader to destroy.
 *
 * @return void
 */
void ffDestroyShader(const VkDevice Device, ffShader Shader);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // FF_SHADER_H_