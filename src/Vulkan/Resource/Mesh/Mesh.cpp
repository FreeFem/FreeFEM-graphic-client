#include <algorithm>
#include "Mesh.h"
#include "Logger.h"

namespace ffGraph {
namespace Vulkan {

Batch newBatch(JSON::SceneObject &Obj) {
    size_t count = 0;
    for (size_t i = 0; i < Obj.Data.size( ); ++i) count += Obj.Data[i].ElementCount;

    Batch n;
    n.BatchedMeshes = ffNewArray(count, Obj.Data[0].ElementSize);
    std::cout << "Size : " << n.BatchedMeshes.ElementSize << "\n";
    std::cout << "Count : " << n.BatchedMeshes.ElementCount << "\n";
    n.Topology = (VkPrimitiveTopology)Obj.RenderPrimitive;
    if (n.BatchedMeshes.Data == 0) return {0, (VkPrimitiveTopology)0, {}, {0, 0, 0}};
    BatchLayout Layout = {0, 0};
    for (size_t i = 0; i < Obj.Data.size( ); ++i) {
        Layout.offset += (i == 0) ? 0 : Layout.size;
        Layout.size = Obj.Data[i].ElementSize * Obj.Data[i].ElementCount;

        n.Layouts.push_back(Layout);
        memcpy((char *)n.BatchedMeshes.Data + Layout.offset, Obj.Data[i].Data, Layout.size);
    }
    n.NbOfMeshBatched = Obj.Data.size( );
    return n;
}

Batch newBatch(std::vector<Array> Data, VkPrimitiveTopology Topology) {
    size_t count = 0;
    for (size_t i = 0; i < Data.size( ); ++i) count += Data[i].ElementCount;

    Batch n;

    n.BatchedMeshes = ffNewArray(count, Data[0].ElementSize);
    if (n.BatchedMeshes.Data == 0) return {0, (VkPrimitiveTopology)0, {}, {0, 0, 0}};
    BatchLayout Layout = {0, 0};
    for (size_t i = 0; i < Data.size( ); ++i) {
        Layout.offset += Layout.size;
        Layout.size = Data[i].ElementSize * Data[i].ElementCount;

        n.Layouts.push_back(Layout);
        memcpy((char *)n.BatchedMeshes.Data + Layout.offset, Data[i].Data, Layout.size);
    }
    n.NbOfMeshBatched = Data.size( );
    n.Topology = Topology;
    return n;
}

Vertex *FillCoordinates2DSpace(Vertex *v, glm::vec3 A, glm::vec3 B) {
    v[0].x = A.x;
    v[1].x = B.x;
    v[0].y = A.y;
    v[1].y = A.y;
    v[0].z = A.z;
    v[1].z = A.z;

    v[2].x = B.x;
    v[3].x = B.x;
    v[2].y = A.y;
    v[3].y = B.y;
    v[2].z = A.z;
    v[3].z = A.z;

    v[4].x = B.x;
    v[5].x = A.x;
    v[4].y = B.y;
    v[5].y = B.y;
    v[4].z = A.z;
    v[5].z = A.z;

    v[6].x = A.x;
    v[7].x = A.x;
    v[6].y = B.y;
    v[7].y = A.y;
    v[6].z = A.z;
    v[7].z = A.z;

    return v;
}

Vertex *FillCoordinates3DSpace(Vertex *v, glm::vec3 A, glm::vec3 B) {
    // First Face
    v[0].x = A.x;
    v[1].x = B.x;
    v[0].y = A.y;
    v[1].y = A.y;
    v[0].z = A.z;
    v[1].z = A.z;

    v[2].x = B.x;
    v[3].x = B.x;
    v[2].y = A.y;
    v[3].y = B.y;
    v[2].z = A.z;
    v[3].z = A.z;

    v[4].x = B.x;
    v[5].x = A.x;
    v[4].y = B.y;
    v[5].y = B.y;
    v[4].z = A.z;
    v[5].z = A.z;

    v[6].x = A.x;
    v[7].x = A.x;
    v[6].y = B.y;
    v[7].y = A.y;
    v[6].z = A.z;
    v[7].z = A.z;

    // Second Face
    v[8].x = A.x;
    v[9].x = B.x;
    v[8].y = A.y;
    v[9].y = A.y;
    v[8].z = B.z;
    v[9].z = B.z;

    v[10].x = B.x;
    v[11].x = B.x;
    v[10].y = A.y;
    v[11].y = B.y;
    v[10].z = B.z;
    v[11].z = B.z;

    v[12].x = B.x;
    v[13].x = A.x;
    v[12].y = B.y;
    v[13].y = B.y;
    v[12].z = B.z;
    v[13].z = B.z;

    v[14].x = A.x;
    v[15].x = A.x;
    v[14].y = B.y;
    v[15].y = A.y;
    v[14].z = B.z;
    v[15].z = B.z;

    // Link faces
    v[16].x = A.x;
    v[17].x = A.x;
    v[16].z = A.x;
    v[17].z = A.z;
    v[16].y = A.x;
    v[17].y = B.x;

    v[18].x = B.x;
    v[19].x = B.x;
    v[18].z = A.x;
    v[19].z = A.z;
    v[18].y = A.x;
    v[19].y = B.x;

    v[20].x = B.x;
    v[21].x = B.x;
    v[20].z = B.x;
    v[21].z = B.z;
    v[20].y = A.x;
    v[21].y = B.x;

    v[22].x = A.x;
    v[24].x = A.x;
    v[22].z = B.x;
    v[24].z = B.z;
    v[22].y = A.x;
    v[24].y = B.x;

    return v;
}

Vertex *FillColorSpace(Vertex *v, size_t count, float r, float g, float b, float a) {
    for (size_t i = 0; i < count; ++i) {
        v[i].r = r;
        v[i].g = g;
        v[i].b = b;
        v[i].a = a;
    }
    return v;
}

BoundingBox ComputeBatchBoundingBox(Batch b, bool asRenderableArray) {
    Vertex *v = (Vertex *)b.BatchedMeshes.Data;
    BoundingBox bbox;

    for (size_t i = 0; i < b.BatchedMeshes.ElementCount; ++i) {
        bbox.A.x = std::max(bbox.A.x, v[i].x);
        bbox.A.y = std::max(bbox.A.y, v[i].y);
        bbox.A.z = std::max(bbox.A.z, v[i].z);

        bbox.B.x = std::min(bbox.B.x, v[i].x);
        bbox.B.y = std::min(bbox.B.y, v[i].y);
        bbox.B.z = std::min(bbox.B.z, v[i].z);
    }
    if (asRenderableArray) {
        if (bbox.B.z == bbox.A.z) {
            bbox.Vertices = ffNewArray(8, sizeof(float) * 7);
            v = CastArrayToVertices(bbox.Vertices);
            v = FillColorSpace(v, bbox.Vertices.ElementCount, 0.1, 0.5, 0.5,
                               0.4);    // BoundingBox will be rendered in a blue color.
            v = FillCoordinates2DSpace(v, bbox.A, bbox.B);
        } else {
            bbox.Vertices = ffNewArray(24, sizeof(float) * 7);
            v = CastArrayToVertices(bbox.Vertices);
            v = FillColorSpace(v, bbox.Vertices.ElementCount, 0.1, 0.5, 0.5,
                               0.4);    // BoundingBox will be rendered in a blue color.
            v = FillCoordinates3DSpace(v, bbox.A, bbox.B);
        }
    }
    return bbox;
}

Batch addDataToBatch(Batch &b, JSON::SceneObject &Obj) {
    std::cout << "Add to batch.\n";
    if (b.Topology != (VkPrimitiveTopology)Obj.RenderPrimitive) {
        LogWarning(GetCurrentLogLocation( ), "Tried to add data from the wrong type to a batch.");
        return b;
    }
    size_t count = 0;
    for (size_t i = 0; i < Obj.Data.size( ); ++i) count += Obj.Data[i].ElementCount;

    Array nArray = ffNewArray(count + b.BatchedMeshes.ElementCount, sizeof(float) * 7);
    ffMemcpyArray(nArray, b.BatchedMeshes, b.BatchedMeshes.ElementCount * b.BatchedMeshes.ElementSize);
    BatchLayout Layout = {0, b.Layouts.back( ).offset};
    for (size_t i = 0; i < Obj.Data.size( ); ++i) {
        Layout.offset += (b.Layouts.empty( )) ? 0 : b.Layouts.back( ).size;
        Layout.size = Obj.Data[i].ElementSize * Obj.Data[i].ElementCount;

        b.Layouts.push_back(Layout);
        memcpy((char *)nArray.Data + Layout.offset, Obj.Data[i].Data, Layout.size);
    }
    DestroyArray(b.BatchedMeshes);
    b.BatchedMeshes = nArray;
    return b;
}

Batch addDataToBatch(Batch &b, Array &Data) {
    size_t count = b.BatchedMeshes.ElementCount + Data.ElementCount;

    Array nArray = ffNewArray(count, sizeof(float) * 7);
    ffMemcpyArray(nArray, b.BatchedMeshes, b.BatchedMeshes.ElementCount * b.BatchedMeshes.ElementSize);

    BatchLayout Layout = {Data.ElementCount * Data.ElementSize, b.Layouts.back( ).offset + b.Layouts.back( ).size};
    b.Layouts.push_back(Layout);
    DestroyArray(b.BatchedMeshes);
    b.BatchedMeshes = nArray;
    return b;
}

void DestroyBatch(Batch &b) {
    DestroyArray(b.BatchedMeshes);
    b.Layouts.clear( );
}

}    // namespace Vulkan
}    // namespace ffGraph