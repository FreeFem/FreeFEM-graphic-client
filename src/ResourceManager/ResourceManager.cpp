#include "ResourceManager.h"

namespace ffGraph {

bool newResourceManager(ResourceManager *RManager, VkDevice *Device, const VkPhysicalDevice PhysicalDevice)
{
    VmaAllocatorCreateInfo createInfo = {};
    createInfo.device = *Device;
    createInfo.physicalDevice = PhysicalDevice;

    if (vmaCreateAllocator(&createInfo, &RManager->Allocator)) return false;
    RManager->DeviceREF = Device;
    return true;
}

void destroyResourceManager(ResourceManager RManager)
{
    ffShaderManager_Destroy(RManager.ShaderManager, *RManager.DeviceREF);
    vmaDestroyAllocator(RManager.Allocator);
}


uint16_t ffResourceManager_NewMesh(ResourceManager& Manager, uint16_t UID, json JsonData)
{
    ffMesh mesh = ffCreateMesh(Manager.Allocator, JsonData);

    if (ffisMeshReady(mesh) == false)
        return UINT16_MAX;
    else {
        Manager.MeshManager.push_back({UID, mesh});
        return UID;
    }
    return UINT16_MAX;
}

std::string ffResourceManager_NewShader(ResourceManager& Manager, std::string Filepath, std::string Name, ffShaderStage Stage)
{
    Vulkan::ffShader Shader = Vulkan::ffCreateShader(Name.c_str(), Filepath.c_str(), *Manager.DeviceREF, (VkShaderStageFlags)Stage);

    if (ffIsShaderReady(Shader)) {
        Manager.ShaderManager.push_back({Name, Shader});
        return Name;
    }
    return "";
}

std::string ffResourceManager_NewImage(ResourceManager& Manager, std::string Name, Vulkan::ffImageCreateInfo pCreateInfos, VmaAllocationCreateInfo pAllocationInfos)
{
    Vulkan::ffImage Image = Vulkan::ffCreateImage(Manager.Allocator, *Manager.DeviceREF, pCreateInfos, pAllocationInfos);

    if (Vulkan::ffIsImageReady(Image)) {
        Manager.ImageManager.push_back({Name, Image});
        return Name;
    }
    return "";
}

}