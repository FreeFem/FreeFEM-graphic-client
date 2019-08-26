#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include "Shader.h"

Error Shader::init(const VkDevice& device, const char *vertexShaderFile, const char *fragmentShaderFile)
{
    if (createVertexShader(device, vertexShaderFile))
        return Error::FUNCTION_FAILED;
    if (createFragmentShader(device, fragmentShaderFile))
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

Error Shader::createVertexShader(const VkDevice& device, const char *vertexShaderFile)
{
    std::ifstream inFile;
    inFile.open(vertexShaderFile, std::ios_base::binary | std::ios_base::ate);

    if (!inFile)
        return Error::FILE_ACCESS;
    size_t fileSize = inFile.tellg();
    std::vector<char> fileContent(fileSize);

    inFile.seekg(0, std::ios::beg);
    bool readStat = bool(inFile.read(fileContent.data(), fileSize));
    inFile.close();

    if (!readStat)
        return false;
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.flags = 0;
    createInfo.codeSize = fileSize;
    createInfo.pCode = reinterpret_cast<uint32_t *>(fileContent.data());

    if (vkCreateShaderModule(device, &createInfo, 0, &m_vertexModule))
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}

Error Shader::createFragmentShader(const VkDevice& device, const char *vertexFragmentFile)
{
    std::ifstream inFile;
    inFile.open(vertexFragmentFile, std::ios_base::binary | std::ios_base::ate);

    if (!inFile)
        return Error::FILE_ACCESS;
    size_t fileSize = inFile.tellg();
    std::vector<char> fileContent(fileSize);

    inFile.seekg(0, std::ios::beg);
    bool readStat = bool(inFile.read(fileContent.data(), fileSize));
    inFile.close();

    if (!readStat)
        return false;
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = 0;
    createInfo.flags = 0;
    createInfo.codeSize = fileSize;
    createInfo.pCode = reinterpret_cast<uint32_t *>(fileContent.data());

    if (vkCreateShaderModule(device, &createInfo, 0, &m_fragmentModule))
        return Error::FUNCTION_FAILED;
    return Error::NONE;
}