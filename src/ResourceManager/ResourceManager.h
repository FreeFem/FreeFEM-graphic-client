/**
 * \file ResourceManager.h
 * @brief Definition of ResourceManager data type and it's related functions.
 */
#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <deque>
#include <string>
#include <vk_mem_alloc.h>
#include "Image/ffImageManager.h"
#include "Shader/ffShaderManager.h"
#include "Mesh/ffMeshManager.h"

namespace ffGraph
{

// @brief Quality of life using
using json = nlohmann::json;

/**
 * @brief Data structure used to handle Vulkan and JSON data transfert.
 */
struct ResourceManager {
    VmaAllocator Allocator;
    VkDevice *DeviceREF;

    ffShaderManager ShaderManager;
    ffImageManager ImageManager;
    ffMeshManager MeshManager;
};

/**
 * @brief Create a new ffGraph::ResourceManager.
 *
 * @param RManager [out] - ffGraph::ResourceManager which will be filled.
 * @param Device [in] - Vulkan device.
 * @param PhysicalDevice [in] - Vulkan physical device.
 *
 * @return bool - false if creation failed.
 */
bool newResourceManager(ResourceManager *RManager, VkDevice *Device, const VkPhysicalDevice PhysicalDevice);

/**
 * @brief Read data from the SharedQueue.
 */
void ReadFromQueue(ResourceManager RManager);

void destroyResourceManager(ResourceManager RManager);

uint16_t ffResourceManager_NewMesh(ResourceManager& Manager, uint16_t UID, json JsonData);

std::string ffResourceManager_NewShader(ResourceManager& Manager, std::string Filepath, std::string Name, ffShaderStage Stage);

std::string ffResourceManager_NewImage(ResourceManager& Manager, std::string Name, Vulkan::ffImageCreateInfo pCreateInfos, VmaAllocationCreateInfo pAllocationInfos);

} // namespace ffGraph

#endif // RESOURCE_MANAGER_H_