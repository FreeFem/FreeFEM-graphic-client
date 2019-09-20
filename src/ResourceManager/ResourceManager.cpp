#include "ResourceManager.h"

namespace ffGraph {

bool newResourceManager(ResourceManager* RManager, VkDevice* Device, const VkPhysicalDevice PhysicalDevice) {
    VmaAllocatorCreateInfo createInfo = {};
    createInfo.device = *Device;
    createInfo.physicalDevice = PhysicalDevice;

    if (vmaCreateAllocator(&createInfo, &RManager->Allocator)) return false;
    RManager->DeviceREF = Device;
    return true;
}

void destroyResourceManager(ResourceManager RManager) {
    ffShaderManager_Destroy(RManager.ShaderManager, *RManager.DeviceREF);
    vmaDestroyAllocator(RManager.Allocator);
}

void ffResourceManager_ReadFromQueue(ResourceManager& RManager) {
    if (RManager.SharedQueue->empty( )) return;
    for (auto ite = RManager.SharedQueue->begin( ); ite != RManager.SharedQueue->end( ); ++ite) {
        json JSONData = json::from_cbor(*ite);

        json GeometryArray = JSONData.at("Geometry");
        for (auto i = GeometryArray.begin( ); i != GeometryArray.end( ); ++i) {
            ffResourceManager_NewMesh(RManager, *i, RManager.UniqueBytes);
        }
    }
}

ffHandle ffResourceManager_NewMesh(ResourceManager& Manager, json JsonData, uint16_t& UniqueBytes) {
    ffMesh Mesh = ffCreateMesh(Manager.Allocator, JsonData);

    if (!ffIsMeshReady(Mesh))
        return InvalidHandle;
    for (uint32_t i = 0; i < (uint32_t)Manager.MeshManager.size(); ++i) {
        if (!Manager.MeshManager[i].isUsed) {
            Manager.UniqueBytes = UniqueBytes;
            Manager.MeshManager[i].Data = Mesh;
            UniqueBytes += 1;
            return ffNewHandle(ffHandleType::FF_HANDLE_TYPE_MESH, UniqueBytes - 1, i);
        }
    }
    return InvalidHandle;
}

ffHandle ffResourceManager_NewShader(ResourceManager& Manager, std::string Filepath, ffShaderStage Stage, uint16_t& UniqueBytes) {
    Vulkan::ffShader Shader =
        Vulkan::ffCreateShader(Filepath.c_str( ), *Manager.DeviceREF, (VkShaderStageFlags)Stage);

    if (!ffIsShaderReady(Shader))
        return InvalidHandle;
    for (uint32_t i = 0; i < (uint32_t)Manager.ShaderManager.size(); ++i) {
        if (!Manager.ShaderManager[i].isUsed) {
            Manager.UniqueBytes = UniqueBytes;
            Manager.ShaderManager[i].Data = Shader;
            UniqueBytes += 1;
            return ffNewHandle(ffHandleType::FF_HANDLE_TYPE_SHADER, UniqueBytes - 1, i);
        }
    }
    return InvalidHandle;
}

ffHandle ffResourceManager_NewImage(ResourceManager& Manager, Vulkan::ffImageCreateInfo pCreateInfos,
                                       VmaAllocationCreateInfo pAllocationInfos, uint16_t& UniqueBytes) {
    Vulkan::ffImage Image =
        Vulkan::ffCreateImage(Manager.Allocator, *Manager.DeviceREF, pCreateInfos, pAllocationInfos);

    if (Vulkan::ffIsImageReady(Image))
        return InvalidHandle;
    for (uint32_t i = 0; i < (uint32_t)Manager.ImageManager.size(); ++i) {
        if (!Manager.ImageManager[i].isUsed) {
            Manager.UniqueBytes = UniqueBytes;
            Manager.ImageManager[i].Data = Image;
            UniqueBytes += 1;
            return ffNewHandle(ffHandleType::FF_HANDLE_TYPE_IMAGE, UniqueBytes - 1, i);
        }
    }
    return InvalidHandle;
}

}    // namespace ffGraph