#ifndef SHADER_H
#define SHADER_H

#include <vulkan/vulkan.h>
#include "../util/utils.h"

class Shader {
    public:

    Error init(const VkDevice, const char *vertexShaderFile, const char *fragmentShaderFile);

    void destroy();

    VkShaderModule vertexModule;
    VkShaderModule fragmentModule;
};

#endif // SHADER_H