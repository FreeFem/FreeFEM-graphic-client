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
};

struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

struct Mesh {
    VkPrimitiveTopology Topology;
    Array CPUBuffer;
    VkBuffer GPUBuffer;
    VmaAllocation Memory;
    VmaAllocationInfo Infos;
};

Mesh newMesh(const VmaAllocator& Allocator, JSON::SceneObject& Obj, int ite);

BoundingBox ComputeMeshBoundingBox(const Mesh& M);

void DestroyMesh(const VmaAllocator& Allocator, Mesh m);

} // namespace Vulkan
} // namespace ffGraph

#endif // MESH_H_