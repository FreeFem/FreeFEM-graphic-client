#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "../core/Window.h"
#include "VulkanContext.h"
#include "ShaderLoader.h"

namespace FEM {
namespace VK {

bool newShader(const char *ShaderName, const char *filepath, ShaderLoader *Loader, const VulkanContext vkContext) {
    ShaderData *Data = (ShaderData *)malloc(sizeof(ShaderData));
    memset(Data, 0, sizeof(ShaderData));
    int fd = open(filepath, O_RDONLY);
    struct stat st;

    if (fd == -1) return false;
    if (fstat(fd, &st) == -1) return false;
    size_t size = st.st_size;

    unsigned char *array = (unsigned char *)malloc(sizeof(unsigned char) * size);
    read(fd, array, sizeof(unsigned char) * size);

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = size;
    createInfo.pCode = (uint32_t *)array;

    if (vkCreateShaderModule(vkContext.Device, &createInfo, 0, &Data->ShaderModule)) return false;
    Data->ShaderName = strdup(ShaderName);
    if (Loader->ShaderList == 0) {
        Loader->ShaderList = Data;
        Loader->ShaderCount += 1;
    } else {
        Data->next = Loader->ShaderList;
        Loader->ShaderList = Data;
    }
    return true;
}

void destroyShader(ShaderData *Data, const VulkanContext vkContext) {
    vkDestroyShaderModule(vkContext.Device, Data->ShaderModule, 0);
}

VkShaderModule searchShader(const char *Name, const ShaderLoader Loader) {
    if (Loader.ShaderList == 0)
        return VK_NULL_HANDLE;
    else {
        ShaderData *tmp = Loader.ShaderList;
        while (tmp != 0) {
            if (strcmp(tmp->ShaderName, Name) == 0) {
                return tmp->ShaderModule;
            }
            tmp = tmp->next;
        }
    }
    return VK_NULL_HANDLE;
}

void destroyShaderLoader(ShaderLoader Loader, const VulkanContext vkContext) {
    if (Loader.ShaderList == 0) return;

    ShaderData *current = Loader.ShaderList;
    ShaderData *next = 0;
    while (current != 0) {
        next = current->next;
        destroyShader(current, vkContext);
        free(current);
        current = next;
    }
}

}    // namespace VK
}    // namespace FEM