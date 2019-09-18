/**
 * @file ffMeshManager.h
 * @brief Declaration of ffMeshManager data type and it's related functions.
 */
#ifndef FF_MESH_MANAGER_H_
#define FF_MESH_MANAGER_H_

#include <vector>
#include "ffMesh.h"

namespace ffGraph {

/**
 * @brief Stored a ffGraph::ffMesh and it's identification number.
 */
struct ffMeshHandle {
    uint16_t UID;
    ffMesh Data;
};

// @brief Stores every ffGraph::ffMesh.
typedef std::vector<ffMeshHandle> ffMeshManager;

/**
 * @brief Search for a ffGraph::ffMesh.
 *
 * @param Manager [in] - ffGraph::ffMeshManager to search in.
 * @param UID [in] - ffGraph::ffMesh identification number.
 *
 * @return ffGraph::ffMesh - Use ffGraph::ffIsMeshReady() to check result value.
 */
ffMesh ffMeshManager_SearchFor(const ffMeshManager& Manager, uint16_t UID);

/**
 * @brief Destroy a ffGraph::ffMeshManager and all the ffGraph::ffMesh it contains.
 *
 * @param Allocator [in] - VmaAllocator used to allocate memory to the ffGraph::Vulkan::ffBuffer.
 * @param Manager [in] - ffGraph::ffMeshManager to destroy.
 *
 * @return void
 */
void ffMeshManager_Destroy(const VmaAllocator& Allocator, ffMeshManager& Manager);

}    // namespace ffGraph

#endif    // FF_MESH_MANAGER_H_