#ifndef FF_MESH_MANAGER_H_
#define FF_MESH_MANAGER_H_

#include <vector>
#include "ffMesh.h"

namespace ffGraph {

struct ffMeshHandle {
    uint16_t UID;
    ffMesh Data;
};

typedef std::vector<ffMeshHandle> ffMeshManager;

ffMesh ffMeshManager_SearchFor(const ffMeshManager&, uint16_t UID);

void ffMeshManager_Destroy(const VmaAllocator& Allocator, ffMeshManager& Manager);

} // namespace ffGraph

#endif // FF_MESH_MANAGER_H_