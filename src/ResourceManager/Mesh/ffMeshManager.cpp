#include <cstring>
#include "ffMeshManager.h"

namespace ffGraph {

ffMesh ffMeshManager_SearchFor(const ffMeshManager& Manager, uint16_t UID) {
    ffMesh EmptyMesh;
    memset(&EmptyMesh, 0, sizeof(ffMesh));

    for (const auto& MeshHandle : Manager) {
        if (MeshHandle.UID == UID) return MeshHandle.Data;
    }
    return EmptyMesh;
}

void ffMeshManager_Destroy(const VmaAllocator& Allocator, ffMeshManager& Manager) {
    for (auto& MeshHandle : Manager) {
        ffDestroyMesh(Allocator, MeshHandle.Data, true);
        MeshHandle.UID = UINT16_MAX;
    }
}

}    // namespace ffGraph
