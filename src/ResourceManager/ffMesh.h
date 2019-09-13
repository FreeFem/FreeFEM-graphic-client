#ifndef FF_MESH_H_
#define FF_MESH_H_

#include "util/Array.h"
#include "ffBuffer.h"
#include <nlohmann/json.hpp>
#include <glm/mat4x4.hpp>

namespace ffGraph {

// @brief Quality of life using
using json = nlohmann::json;

namespace Vulkan {

/**
 * @brief Stores infos on the layout of data in buffer memory.
 */
struct BufferLayout {
    VkFormat Format;
    VkDeviceSize Offset;
};

// @brief Integer type used to store one index.
typedef VkIndexType IndicesType;

}

/**
 * @brief Renderable object.
 */
struct ffMesh {
    // @brief ID of the Mesh (Used to update data from JSON).
    uint16_t UID;
    // @brief Vertices on CPU memory.
    Array Vertices;
    // @brief Indices on CPU memory.
    Array Indices;

    // @brief Vertices on GPU memory.
    Vulkan::ffBuffer VulkanVertices;
    // @brief Indices on GPU memory.
    Vulkan::ffBuffer VulkanIndices;
    // @brief Vertex layout.
    std::vector<Vulkan::BufferLayout> Layout;
    // @brief Index type.
    Vulkan::IndicesType IndexType;

    // @brief Model matrix for local transformations.
    glm::mat4 ModelMatrix;
};

/**
 * @brief Look if a ffMesh was successfully created.
 */
inline bool isMeshReady(ffMesh m) { return (isArrayReady(m.Vertices) || isArrayReady(m.Indices) || Vulkan::ffIsBufferReady(m.VulkanVertices) || Vulkan::ffIsBufferReady(m.VulkanIndices)); }

/**
 * @brief Create ffMesh from ffGraph::Array
 */
ffMesh ffCreateMesh(const VmaAllocator Allocator, Array Vertices, Array Indices, uint32_t LayoutCount, Vulkan::BufferLayout *Layouts, Vulkan::IndicesType IndexType);

/**
 * @brief Create ffMesh from JSON. (Will be done when the Client and Server are running).
 */
// ffMesh ffCreateMesh(const VmaAllocator Allocator, json ObjectJSON);

/**
 * @brief Destroy a ffMesh
 */
void ffDestroyMesh(const VmaAllocator Allocator, ffMesh Mesh, bool DestroyArrays);

}
#endif // FF_MESH_H_