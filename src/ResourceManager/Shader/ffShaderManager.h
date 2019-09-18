/**
 * @file ffShaderManager.h
 * @brief Declaration of ffShaderManager data type and it's related functions.
 */
#ifndef FF_SHADER_MANAGER_H_
#define FF_SHADER_MANAGER_H_

#include <vector>
#include <string>
#include "ffShader.h"

namespace ffGraph {

/**
 * @brief Vulkan shader stages.
 */
enum ffShaderStage {
    e_ffVertexShaderStage = VK_SHADER_STAGE_VERTEX_BIT,
    e_ffFragmentShaderStage = VK_SHADER_STAGE_FRAGMENT_BIT
};

/**
 * @brief Contains a ffGraph::Vulkan::ffShader and it's std::string Key.
 */
struct ffShaderHandle {
    std::string Key;
    Vulkan::ffShader Data;
};

// @brief Store every ffGraph::Vulkan::ffShader created.
typedef std::vector<ffShaderHandle> ffShaderManager;

/**
 * @brief Search for ffGraph::Vulkan::ffShader in ffGraph::ffShaderManager.
 *
 * @param Manager [in] - ffGraph::ffShaderManager to search in.
 * @param Key [in] - std::string assiociated with a ffGraph::Vulkan::ffShader.
 *
 * @return ffGraph::Vulkan::ffShader - Use ffGraph::Vulkan::ffIsShaderReady() to check return value.
 */
Vulkan::ffShader ffShaderManager_SearchFor(const ffShaderManager& Manager, const std::string& Key);

/**
 * @brief Destroy the ffGraph::ffShaderManager and all the ffGraph::Vulkan::ffShader it contains.
 * This function is called by ffGraph::ResourceManager automaticly.
 *
 * @param Manager [in] - ffGraph::ffShaderManager to destroy.
 * @param Device [in] - VkDevice used to create all the ffGraph::Vulkan::ffShader.
 *
 * @return void
 */
void ffShaderManager_Destroy(ffShaderManager& Manager, const VkDevice& Device);

}    // namespace ffGraph

#endif    // FF_SHADER_MANAGER_H_