/**
 * @file ffShader.h
 * @brief Definition of ffShader data type and it's related functions.
 */
#ifndef FF_SHADER_H_
#define FF_SHADER_H_

#include <vulkan/vulkan.h>

namespace ffGraph
{
namespace Vulkan
{

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
 * @brief Look if the Shader creation was successful.
 *
 * @param ffShader [in] - ffShader on which the test is performed.
 * @return bool - Boolean value (true : ffShader is read / false : ffShader isn't ready).
 */
inline bool ffIsShaderReady(ffShader Shader) { return (Shader.ShaderName == VK_NULL_HANDLE) ? false : true; }

/**
 * @brief Create a new ffShader
 *
 * @param const char *Name [in] - Name of the Shader used to query.
 * @param const char *FilePath [in] - SPIRV file location.
 * @param const VkDevice [in] - Device used to create the ShaderModule.
 * @param VkShaderStageFlags Stage [in] - Pipeline stage of the shader.
 *
 * @return ffShader - A new ffShader (use ffGraph::Vulkan::isShaderReady to check return value).
 */
ffShader ffCreateShader(const char *Name, const char *FilePath, const VkDevice Device, VkShaderStageFlags Stage);

/**
 * @brief Destroy a ffShader, releasing it's memory.
 *
 * @param const VkDevice Device [in] - Device used to create the Shader.
 * @param ffShader Shader [in] - Shader to destroy.
 */
void ffDestroyShader(const VkDevice Device, ffShader Shader);

} // namespace Vulkan
} // namespace ffGraph


#endif // FF_SHADER_H_