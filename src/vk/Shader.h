#ifndef SHADER_H
#define SHADER_H

#include <vulkan/vulkan.h>
#include "../util/utils.h"

/**
 * WIP
 */
class Shader {
    public:

        Error init(const VkDevice& device, const char *vertexShaderFile, const char *fragmentShaderFile);

        void destroy();

        inline VkShaderModule getVertexModule() const { return m_vertexModule; }
        inline VkShaderModule getFragmentModule() const { return m_fragmentModule; }

    private:
        VkShaderModule m_vertexModule = VK_NULL_HANDLE;
        VkShaderModule m_fragmentModule = VK_NULL_HANDLE;

        Error createVertexShader(const VkDevice&, const char *vertexShaderFile);
        Error createFragmentShader(const VkDevice&, const char *vertexShaderFile);
};

#endif // SHADER_H