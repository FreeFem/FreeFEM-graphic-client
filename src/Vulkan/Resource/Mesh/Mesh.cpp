#include <algorithm>
#include "Mesh.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

Mesh newMesh(const VmaAllocator& Allocator, JSON::SceneObject& Obj, int ite)
{
    Mesh n;

    memset(&n, 0, sizeof(Mesh));
    n.Topology = (VkPrimitiveTopology)Obj.RenderPrimitive;

    VkBufferCreateInfo vkCreateInfo = {};
    vkCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    vkCreateInfo.size = Obj.Data[ite].ElementCount * Obj.Data[ite].ElementSize;
    vkCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    vkCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo VerticesAllocInfo = {};
    VerticesAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    VerticesAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    VerticesAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    VerticesAllocInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    if (vmaCreateBuffer(Allocator, &vkCreateInfo, &VerticesAllocInfo, &n.GPUBuffer, &n.Memory, &n.Infos)) {
        LogError(GetCurrentLogLocation(), "Failed to create VkBuffer.\n");
        return n;
    }
    n.CPUBuffer = Obj.Data[ite];

    memcpy(n.Infos.pMappedData, Obj.Data[ite].Data, Obj.Data[ite].ElementCount * Obj.Data[ite].ElementSize);
    return n;
}

void DestroyMesh(const VmaAllocator& Allocator, Mesh m)
{
    vmaDestroyBuffer(Allocator, m.GPUBuffer, m.Memory);
    DestroyArray(m.CPUBuffer);
}

BoundingBox ComputeMeshBoundingBox(const Mesh& M)
{
    Vertex *v = (Vertex *)M.CPUBuffer.Data;
    BoundingBox bb;

    for (size_t i = 0; i < M.CPUBuffer.ElementCount; ++i) {
        bb.A.x = std::max(bb.A.x, v[i].x);
        bb.A.y = std::max(bb.A.y, v[i].y);
        bb.A.z = std::max(bb.A.z, v[i].z);

        bb.B.x = std::max(bb.B.x, v[i].x);
        bb.B.y = std::max(bb.B.y, v[i].y);
        bb.B.z = std::max(bb.B.z, v[i].z);
    }
    return bb;
}

} // namespace Vulkan
} // namespace ffGraph