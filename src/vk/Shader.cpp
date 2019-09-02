#include "Shader.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

namespace FEM {
namespace gr {

ErrorValues Shader::init(const VkDevice &device, const char *vertexShaderFile, const char *fragmentShaderFile) {
    if (createVertexShader(device, vertexShaderFile)) return ErrorValues::FUNCTION_FAILED;
    if (createFragmentShader(device, fragmentShaderFile)) return ErrorValues::FUNCTION_FAILED;
    return ErrorValues::NONE;
}

void Shader::destroy(const VkDevice &device)
{
    vkDestroyShaderModule(device, VertexModule, 0);
    vkDestroyShaderModule(device, FragmentModule, 0);
}

ErrorValues Shader::createVertexShader(const VkDevice &device, const char *vertexShaderFile) {
    std::ifstream inFile;
    inFile.open(vertexShaderFile, std::ios_base::binary | std::ios_base::ate);

    if (!inFile) return ErrorValues::FILE_ACCESS;
    size_t fileSize = inFile.tellg( );
    std::vector<char> fileContent(fileSize);

    inFile.seekg(0, std::ios::beg);
    bool readStat = bool(inFile.read(fileContent.data( ), fileSize));
    inFile.close( );

    if (!readStat) return false;
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.flags = 0;
    createInfo.codeSize = fileSize;
    createInfo.pCode = reinterpret_cast<uint32_t *>(fileContent.data( ));

    if (vkCreateShaderModule(device, &createInfo, 0, &VertexModule)) return ErrorValues::FUNCTION_FAILED;
    return ErrorValues::NONE;
}

ErrorValues Shader::createFragmentShader(const VkDevice &device, const char *vertexFragmentFile) {
    std::ifstream inFile;
    inFile.open(vertexFragmentFile, std::ios_base::binary | std::ios_base::ate);

    if (!inFile) return ErrorValues::FILE_ACCESS;
    size_t fileSize = inFile.tellg( );
    std::vector<char> fileContent(fileSize);

    inFile.seekg(0, std::ios::beg);
    bool readStat = bool(inFile.read(fileContent.data( ), fileSize));
    inFile.close( );

    if (!readStat) return false;
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.flags = 0;
    createInfo.codeSize = fileSize;
    createInfo.pCode = reinterpret_cast<uint32_t *>(fileContent.data( ));

    if (vkCreateShaderModule(device, &createInfo, 0, &FragmentModule)) return ErrorValues::FUNCTION_FAILED;
    return ErrorValues::NONE;
}
}    // namespace gr
}    // namespace FEM