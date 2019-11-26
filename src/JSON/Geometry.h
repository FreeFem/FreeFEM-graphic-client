#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <cstdint>
#include <vulkan/vulkan.h>
#include <string>
#include "Array.h"
#include "../ffTypes.h"

namespace ffGraph
{

struct Vertex {
    float x, y, z;
    float r, g, b, a;
};

enum GeometryPrimitiveTopology : uint16_t {
    GEO_PRIMITIVE_TOPOLOGY_POINT_LIST = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    GEO_PRIMITIVE_TOPOLOGY_LINE_LIST = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    GEO_PRIMITIVE_TOPOLOGY_LINE_STRIP = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
    GEO_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    GEO_PRIMITIVE_TOPOLOGY_COUNT
};

enum GeometryPolygonMode : uint8_t {
    GEO_POLYGON_MODE_FILL = VK_POLYGON_MODE_FILL,
    GEO_POLYGON_MODE_LINE = VK_POLYGON_MODE_LINE,
    GEO_POLYGON_MODE_COUNT
};

struct GeometryDescriptor {
    unsigned int LineWith = 2;
    uint16_t PrimitiveTopology = GEO_PRIMITIVE_TOPOLOGY_COUNT;
    uint8_t PolygonMode = GEO_POLYGON_MODE_COUNT;
    uint16_t PipelineID;
};

struct Geometry {
    ffTypes Type;
    Array Data;

    GeometryDescriptor Description;
    VkDeviceSize BufferOffset;

    inline size_t count() { return Data.ElementCount; }
    inline size_t size() { return Data.ElementCount * Data.ElementSize; }
};

struct ConstructedGeometry {
    uint16_t PlotID;
    std::string Name;
    uint16_t MeshID;

    ConstructedGeometry(uint16_t pID, uint16_t mID) : PlotID(pID), MeshID(mID) {}
    Geometry Geo;
};

} // namespace ffGraph


#endif // GEOMETRY_H_