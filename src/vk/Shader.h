#ifndef SHADER_H
#define SHADER_H

#include <vulkan/vulkan.h>
#include "../util/utils.h"

namespace FEM {
namespace gr {
/**
 * @brief Store shaders infos
 */
class Shader {
   public:
    ErrorValues init(const VkDevice &device, const char *vertexShaderFile, const char *fragmentShaderFile);

    void destroy( );

    VkShaderModule VertexModule = VK_NULL_HANDLE;
    VkShaderModule FragmentModule = VK_NULL_HANDLE;

   private:
    ErrorValues createVertexShader(const VkDevice &, const char *vertexShaderFile);
    ErrorValues createFragmentShader(const VkDevice &, const char *vertexShaderFile);
};
}    // namespace gr
}    // namespace FEM

#endif    // SHADER_H