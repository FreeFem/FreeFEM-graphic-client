#include "ffMesh.h"
#include "util/Logger.h"

namespace ffGraph {

ffMesh ffCreateMesh(const VmaAllocator Allocator, Array Vertices, Array Indices, uint32_t LayoutCount, Vulkan::BufferLayout *Layouts, Vulkan::IndicesType IndexType)
{
    ffMesh n;

    memset(&n, 0, sizeof(ffMesh));
    if (!isArrayReady(Vertices) || !isArrayReady(Indices)) {
        LogError(GetCurrentLogLocation(), "Array used to create new ffMesh are unready.");
        return n;
    }

    n.Vertices = Vertices;
    n.Indices = Indices;

    n.Layout = std::vector<Vulkan::BufferLayout>(LayoutCount);
    for (uint32_t i = 0; i < LayoutCount; i += 1)
        n.Layout.push_back(Layouts[i]);
    n.IndexType = IndexType;
    n.ModelMatrix= glm::mat4(1.0f);

    Vulkan::ffBufferCreateInfo VerticesCreateInfo = {};
    VerticesCreateInfo.Usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    VerticesCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    VerticesCreateInfo.ElementSize = n.Vertices.ElementSize;
    VerticesCreateInfo.ElementCount = n.Vertices.ElementCount;

    VmaAllocationCreateInfo VerticesAllocInfo = {};
    VerticesAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    VerticesAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    VerticesAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    VerticesAllocInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    n.VulkanVertices = Vulkan::ffCreateBuffer(Allocator, VerticesCreateInfo, VerticesAllocInfo);
    if (!Vulkan::ffIsBufferReady(n.VulkanVertices))
        return n;

    Vulkan::ffBufferCreateInfo IndicesCreateInfo = {};
    IndicesCreateInfo.Usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    IndicesCreateInfo.SharingMode = VK_SHARING_MODE_EXCLUSIVE;
    IndicesCreateInfo.ElementSize = n.Indices.ElementSize;
    IndicesCreateInfo.ElementCount = n.Indices.ElementCount;

    VmaAllocationCreateInfo IndicesAllocInfo = {};
    IndicesAllocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    IndicesAllocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    IndicesAllocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
    IndicesAllocInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_CACHED_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    n.VulkanIndices = Vulkan::ffCreateBuffer(Allocator, IndicesCreateInfo, IndicesAllocInfo);
    if (!Vulkan::ffIsBufferReady(n.VulkanIndices))
        return n;
    Vulkan::ffMapArrayToBuffer(n.VulkanVertices, n.Vertices);
    Vulkan::ffMapArrayToBuffer(n.VulkanIndices, n.Indices);
    return n;
}

void ffDestroyMesh(const VmaAllocator Allocator, ffMesh Mesh, bool DestroyArrays)
{
    Vulkan::ffDestroyBuffer(Allocator, Mesh.VulkanVertices);
    Vulkan::ffDestroyBuffer(Allocator, Mesh.VulkanIndices);
    Mesh.Layout.clear();
    Mesh.IndexType = VK_INDEX_TYPE_NONE_NV;

    if (DestroyArrays) {
        ffDestroyArray(Mesh.Vertices);
        ffDestroyArray(Mesh.Indices);
    }
    Mesh.ModelMatrix = glm::mat4(1.0f);
}

}