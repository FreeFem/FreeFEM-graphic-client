/**
 * @file ResourceManager.h
 * @brief Declaration of ResourceManager data type and it's related functions.
 */
#ifndef RESOURCE_MANAGER_H_
#define RESOURCE_MANAGER_H_

#include <deque>
#include <string>
#include <vk_mem_alloc.h>
#include "Image/ffImageManager.h"
#include "Shader/ffShaderManager.h"
#include "Mesh/ffMeshManager.h"
#include "ffHandle.h"

namespace ffGraph {

// @brief Quality of life using
using json = nlohmann::json;

/**
 * @brief Data structure used to handle Vulkan and JSON data transfert.
 */
struct ResourceManager {
    VmaAllocator Allocator;
    VkDevice* DeviceREF;

    uint16_t UniqueBytes = 0;
    std::shared_ptr<std::deque<std::string>> SharedQueue;
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
bool newResourceManager(ResourceManager* RManager, VkDevice* Device, const VkPhysicalDevice PhysicalDevice);

/**
 * @brief Read data from ffGraph::ResourceManager::SharedQueue.
 *
 * @param RManager [in] - ffGraph::ResourceManager which contain the std::deque.
 *
 * @return void
 */
void ReadFromQueue(ResourceManager RManager);

/**
 * @brief Destroy a ffGraph::ResourceManager.
 *
 * @param RManager [in] - ffGraph::ResourceManager to destroy.
 *
 * @return void
 */
void destroyResourceManager(ResourceManager RManager);

/**
 * @brief Ask the ffGraph::ResourceManager to create a ffGraph::Vulkan::ffMesh from JSON. This function is automaticly
 * called byt the ffGraph::ResourceManager.
 *
 * @param RManager [in] - ffGraph::ResourceManager used to store the new ffGraph::Vulkan::ffMesh.
 * @param UID [in] - ffGraph::Vulkan::ffMesh's UID used in the ffGraph::ffMeshManager.
 * @param JsonData [in] - Json object used to create a ffGraph::Vulkan::ffMesh.
 *
 * @return uint16_t - UINT16_MAX if the function failed, UID if it succeed.
 */
ffHandle ffResourceManager_NewMesh(ResourceManager& RManager, json JsonData, uint16_t &UniqueBytes);

/**
 * @brief  Ask the ffGraph::ResourceManager to create a ffGraph::Vulkan::ffShader.
 *
 * @param RManager [in] - ffGraph::ResourceManager used to store the new ffGraph::Vulkan::ffShader.
 * @param Filepath [in] - Path to the shader file.
 * @param Name [in] - Name given to the shader.
 * @param Stage [in] - Vulkan shader pipeline stage.
 *
 * @return std::string - Empty string ("") if the function failed, Name if it succeed.
 */
ffHandle ffResourceManager_NewShader(ResourceManager& RManager, std::string Filepath, std::string Name,
                                        ffShaderStage Stage,
                                        uint16_t &UniqueBytes);

/**
 * @brief Ask the ffGraph::ResourceManager to create a ffGraph::Vulkan::ffImage.
 *
 * @param RManager [in] - ffGraph::ResourceManager used to store the new ffGraph::Vulkan::ffImage.
 * @param Name [in] - Name of the new ffGraph::Vulkan::ffImage.
 * @param pCreateInfos [in] - Data needed to create a ffGraph::Vulkan::ffImage.
 * @param pAllocateInfos [in] - Data used by VulkanMemoryAllocator to allocate memory to ffGraph::Vulkan::ffImage.
 *
 * @return std::string - Empty string ("") if the function failed, Name if it succeed.
 */
ffHandle ffResourceManager_NewImage(ResourceManager& RManager,
                                       Vulkan::ffImageCreateInfo pCreateInfos,
                                       VmaAllocationCreateInfo pAllocationInfos,
                                       uint16_t &UniqueBytes);

}    // namespace ffGraph

#endif    // RESOURCE_MANAGER_H_