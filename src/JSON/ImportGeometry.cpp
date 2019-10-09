#include <nlohmann/json.hpp>
#include <fstream>
#include "deserializer.h"
#include "Logger.h"

namespace ffGraph {
namespace JSON {

using json = nlohmann::json;

/**
 * @brief Used to compute and fill the Array
 */
struct Vertex2D {
    float x, y;
    float r, g, b, a;
};

/**
 * @brief Used to compute and fill the Array
 */
struct Vertex3D {
    float x, y, z;
    float r, g, b, a;
};

Array ImportLineMesh2D(json Geometry, LabelTable &Colors) {
    Array n;

    std::vector<Label> Labels = Geometry["Labels"].get<std::vector<Label>>( );
    for (size_t i = 0; i < Labels.size( ); ++i) {
        AddLabelToTable(Colors, Labels[i]);
    }
    GenerateColorFromLabels(Colors);

    std::vector<uint32_t> Indices = Geometry["Indices"].get<std::vector<uint32_t>>( );

    n = ffNewArray(Indices.size( ), sizeof(Vertex2D));

    std::vector<float> Vertices = Geometry["Vertices"].get<std::vector<float>>( );

    Vertex2D *ptr = (Vertex2D *)n.Data;
    for (size_t i = 0; i < Indices.size( ); ++i) {
        Color vColor = GetColor(Colors, Labels[Indices[i]]);
        ptr[i].x = Vertices[Indices[i] * 2 + 0];
        ptr[i].y = Vertices[Indices[i] * 2 + 1];

        ptr[i].r = vColor.r;
        ptr[i].g = vColor.g;
        ptr[i].b = vColor.b;
        ptr[i].a = vColor.a;
    }
    LogInfo("ImportLineMesh2D", "Successfully imported geometry.");
    Labels.clear( );
    Indices.clear( );
    Vertices.clear( );
    return n;
}

Array ImportLineMesh3D(json Geometry, LabelTable &Colors) {
    Array n;

    std::vector<Label> Labels = Geometry["Labels"].get<std::vector<Label>>( );
    for (size_t i = 0; i < Labels.size( ); ++i) {
        AddLabelToTable(Colors, Labels[i]);
    }
    GenerateColorFromLabels(Colors);

    std::vector<uint32_t> Indices = Geometry["Indices"].get<std::vector<uint32_t>>( );

    n = ffNewArray(Indices.size( ), sizeof(Vertex3D));

    std::vector<float> Vertices = Geometry["Vertices"].get<std::vector<float>>( );

    Vertex3D *ptr = (Vertex3D *)n.Data;
    for (size_t i = 0; i < Indices.size( ); ++i) {
        Color vColor = GetColor(Colors, Labels[Indices[i]]);
        ptr[i].x = Vertices[Indices[i] * 3 + 0];
        ptr[i].y = Vertices[Indices[i] * 3 + 1];
        ptr[i].z = Vertices[Indices[i] * 3 + 2];

        ptr[i].r = vColor.r;
        ptr[i].g = vColor.g;
        ptr[i].b = vColor.b;
        ptr[i].a = vColor.a;
    }
    LogInfo("ImportLineMesh3D", "Successfully imported geometry.");
    Labels.clear( );
    Indices.clear( );
    Vertices.clear( );
    return n;
}

Array ImportTriangleMesh2D(json Geometry, LabelTable &Colors) {
    Array n;

    std::vector<Label> Labels = Geometry["Labels"].get<std::vector<Label>>( );
    for (size_t i = 0; i < Labels.size( ); ++i) {
        AddLabelToTable(Colors, Labels[i]);
    }
    GenerateColorFromLabels(Colors);

    std::vector<uint32_t> Indices = Geometry["Indices"].get<std::vector<uint32_t>>( );

    n = ffNewArray(Indices.size( ), sizeof(Vertex2D));

    std::vector<float> Vertices = Geometry["Vertices"].get<std::vector<float>>( );

    Vertex2D *ptr = (Vertex2D *)n.Data;
    for (size_t i = 0; i < Indices.size( ); ++i) {
        Color vColor = GetColor(Colors, Labels[Indices[i]]);
        ptr[i].x = Vertices[Indices[i] * 2 + 0];
        ptr[i].y = Vertices[Indices[i] * 2 + 1];

        ptr[i].r = vColor.r;
        ptr[i].g = vColor.g;
        ptr[i].b = vColor.b;
        ptr[i].a = vColor.a;
    }
    Labels.clear( );
    Indices.clear( );
    Vertices.clear( );
    LogInfo("ImportTriangleMesh2D", "Successfully imported geometry.");
    return n;
}

Array ImportTriangleMesh3D(json Geometry, LabelTable &Colors) {
    Array n;

    std::vector<Label> Labels = Geometry["Labels"].get<std::vector<Label>>( );
    for (size_t i = 0; i < Labels.size( ); ++i) {
        AddLabelToTable(Colors, Labels[i]);
    }
    GenerateColorFromLabels(Colors);

    std::vector<uint32_t> Indices = Geometry["Indices"].get<std::vector<uint32_t>>( );

    n = ffNewArray(Indices.size( ), sizeof(Vertex3D));

    std::vector<float> Vertices = Geometry["Vertices"].get<std::vector<float>>( );

    Vertex3D *ptr = (Vertex3D *)n.Data;
    for (size_t i = 0; i < Indices.size( ); ++i) {
        Color vColor = GetColor(Colors, Labels[i]);
        ptr[i].x = Vertices[Indices[i] * 3 + 0];
        ptr[i].y = Vertices[Indices[i] * 3 + 1];
        ptr[i].z = Vertices[Indices[i] * 3 + 2];

        ptr[i].r = vColor.r;
        ptr[i].g = vColor.g;
        ptr[i].b = vColor.b;
        ptr[i].a = vColor.a;
    }
    Labels.clear( );
    Indices.clear( );
    Vertices.clear( );
    LogInfo("ImportTriangleMesh3D", "Successfully imported geometry.");
    return n;
}

}    // namespace JSON
}    // namespace ffGraph