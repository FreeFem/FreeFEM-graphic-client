#include <cstring>
#include "ffMeshManager.h"

namespace ffGraph {

ffMesh ffMeshManager_SearchFor(const ffMeshManager& Manager, ffHandle Handle) {
    ffMesh EmptyMesh;
    memset(&EmptyMesh, 0, sizeof(ffMesh));

    if (Handle.Data.Type != ffHandleType::FF_HANDLE_TYPE_MESH)
        return EmptyMesh;
    if (Manager[Handle.Data.Cell].UniqueBytes == Handle.Data.UniqueBytes) {
        return Manager[Handle.Data.Cell].Data;
    }
    return EmptyMesh;
}

void ffMeshManager_DestroyItem(const VmaAllocator& Allocator, ffMeshManager& Manager, ffHandle Handle)
{
    if (!Manager[Handle.Data.Cell].isUsed && Manager[Handle.Data.Cell].UniqueBytes != Handle.Data.UniqueBytes)
        return;
    ffDestroyMesh(Allocator, Manager[Handle.Data.Cell].Data, true);
    Manager[Handle.Data.Cell].isUsed = false;
    Manager[Handle.Data.Cell].UniqueBytes += 1;
    memset(&Manager[Handle.Data.Cell].Data, 0, sizeof(ffMesh));
}

void ffMeshManager_Destroy(const VmaAllocator& Allocator, ffMeshManager& Manager) {
    for (auto& MeshHandle : Manager) {
        ffDestroyMesh(Allocator, MeshHandle.Data, true);
        MeshHandle.isUsed = false;
        MeshHandle.UniqueBytes = UINT16_MAX;
    }
}

}    // namespace ffGraph
