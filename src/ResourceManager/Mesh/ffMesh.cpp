#include <cstring>
#include "ffMesh.h"
#include "Logger.h"

namespace ffGraph {

static ffMesh ffNewEmptyMesh( ) {
    ffMesh empty;
    memset(&empty, 0, sizeof(ffMesh));
    return empty;
}

ffMesh ffCreateMesh(const VmaAllocator Allocator, Array Vertices, Array Indices, uint32_t LayoutCount,
                    Vulkan::BufferLayout *Layouts, Vulkan::IndicesType IndexType) {
    ffMesh n;

    memset(&n, 0, sizeof(ffMesh));
    if (!isArrayReady(Vertices) || !isArrayReady(Indices)) {
        LogError(GetCurrentLogLocation( ), "Array used to create new ffMesh are unready.");
        return n;
    }

    n.Vertices = Vertices;
    n.Indices = Indices;

    n.Layout = std::vector<Vulkan::BufferLayout>(LayoutCount);
    for (uint32_t i = 0; i < LayoutCount; i += 1) n.Layout.push_back(Layouts[i]);
    n.IndexType = IndexType;
    n.ModelMatrix = glm::mat4(1.0f);

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
    if (!Vulkan::ffIsBufferReady(n.VulkanVertices)) return n;

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
    if (!Vulkan::ffIsBufferReady(n.VulkanIndices)) return n;
    Vulkan::ffMapArrayToBuffer(n.VulkanVertices, n.Vertices);
    Vulkan::ffMapArrayToBuffer(n.VulkanIndices, n.Indices);
    return n;
}

void ffDestroyMesh(const VmaAllocator Allocator, ffMesh Mesh, bool DestroyArrays) {
    Vulkan::ffDestroyBuffer(Allocator, Mesh.VulkanVertices);
    Vulkan::ffDestroyBuffer(Allocator, Mesh.VulkanIndices);
    Mesh.Layout.clear( );
    Mesh.IndexType = VK_INDEX_TYPE_NONE_NV;

    if (DestroyArrays) {
        ffDestroyArray(Mesh.Vertices);
        ffDestroyArray(Mesh.Indices);
    }
    Mesh.ModelMatrix = glm::mat4(1.0f);
}

static ffMesh ffCreateMeshCurve(const VmaAllocator Allocator, json ObjectJSON) {
    std::cout << "Curve\n";
    ffMesh mesh = ffNewEmptyMesh( );
    mesh.IndexType = VK_INDEX_TYPE_UINT16;
    mesh.Layout.push_back({VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 0});
    mesh.Layout.push_back({VK_FORMAT_R32G32B32A32_SFLOAT, sizeof(float) * 3});

    std::vector<float> vertices = ObjectJSON["Vertices"].get<std::vector<float>>( );
    mesh.Vertices = ffNewArray(vertices.size( ), sizeof(float));
    if (isArrayReady(mesh.Vertices) == false) return mesh;
    ffMemcpyArray(mesh.Vertices, vertices.data( ));

    std::vector<uint16_t> indices = ObjectJSON["Indices"].get<std::vector<uint16_t>>( );
    mesh.Indices = ffNewArray(indices.size( ), sizeof(uint16_t));
    if (isArrayReady(mesh.Indices) == false) return mesh;
    ffMemcpyArray(mesh.Indices, indices.data( ));
    return mesh;
}

ffMesh ffCreateMesh(const VmaAllocator Allocator, json ObjectJSON) {
    std::string Type = ObjectJSON.at("Type");

    if (Type.compare("Curve") == 0) return ffCreateMeshCurve(Allocator, ObjectJSON);
    return ffNewEmptyMesh( );
}

ffMesh ffCreateMeshFromString(const VmaAllocator Allocator, std::string JSON_string) {
    using json = nlohmann::json;
    ffMesh mesh = ffNewEmptyMesh( );
    json ObjectJSON = json::from_cbor(JSON_string);

    json Object = ObjectJSON["Geometry"].at(0);
    std::string Type = Object.at("Type");
    if (strcmp(Type.data( ), "Curve") == 0) return ffCreateMeshCurve(Allocator, Object);
    return mesh;
}

}    // namespace ffGraph