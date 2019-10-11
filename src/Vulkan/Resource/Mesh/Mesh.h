#ifndef MESH_H_
#define MESH_H_

#include <vulkan/vulkan.h>
#include <glm/mat4x4.hpp>
#include "vk_mem_alloc.h"
#include "Array.h"
#include "deserializer.h"

namespace ffGraph {
namespace Vulkan {

struct BoundingBox {
    glm::vec3 A = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 B = glm::vec3(0.f, 0.f, 0.f);
    Array Vertices;
};

struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

typedef Array Mesh;

struct BatchLayout {
    size_t size = 0;
    size_t offset = 0;
};

struct Batch {
    uint32_t NbOfMeshBatched = 0;
    VkPrimitiveTopology Topology;
    std::vector<BatchLayout> Layouts = {};
    Mesh BatchedMeshes;
};

#define CastArrayToVertices(Array) ((Vertex*)Array.Data)

Batch newBatch(JSON::SceneObject& Obj);

Batch newBatch(std::vector<Array> Data, VkPrimitiveTopology Topology);

Batch addDataToBatch(Batch& b, JSON::SceneObject& Obj);

void DestroyBatch(Batch& b);

BoundingBox ComputeBatchBoundingBox(Batch& b, bool asRenderableArray, JSON::Dimension Dimension);

BoundingBox ComputeSingleMeshBoundingBox(Batch b, uint32_t index, bool asRenderableArray);

}    // namespace Vulkan
}    // namespace ffGraph

#endif    // MESH_H_