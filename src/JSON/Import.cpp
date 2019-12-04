#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <future>
#include <glm/glm.hpp>
#include "Logger.h"
#include "Import.h"
#include "IO.h"

namespace ffGraph {
namespace JSON {

static GeometryPrimitiveTopology GetMainPrimitiveTopology(std::string& j)
{
    if (j.compare("Curve2D") == 0 || j.compare("Curve3D") == 0)
        return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_LINE_LIST;
    else if (j.compare("Mesh2D") == 0 || j.compare("Mesh3D") == 0)
        return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_COUNT;
}

static GeometryPrimitiveTopology GetBorderPrimitiveTopology(std::string& j)
{
    if (j.compare("Mesh2D") == 0)
        return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_LINE_LIST;
    else if (j.compare("Mesh3D") == 0)
        return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    return GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_COUNT;
}

Geometry ConstructGeometry(std::vector<float> Vertices, std::vector<uint32_t> Indices, std::vector<int> Labels, LabelTable& Table)
{
    Geometry n;

    for (const auto& lab : Labels) {
        AddLabelToTable(Table, lab);
    }
    GenerateColorFromLabels(Table);

    n.Data = ffNewArray(Indices.size(), sizeof(Vertex));

    size_t size = Indices.size();
    Vertex *ptr = (Vertex *)n.Data.Data;
    for (size_t i = 0; i < size; ++i) {

        ptr[i].x = Vertices[Indices[i] * 3 + 0];
        ptr[i].y = Vertices[Indices[i] * 3 + 1];
        ptr[i].z = Vertices[Indices[i] * 3 + 2];
        ptr[i].r = 0.f;
        ptr[i].g = 0.f;
        ptr[i].b = 0.f;
        ptr[i].a = 1.f;
    }
    return n;
}

Geometry ConstructBorder(std::vector<float> Vertices, std::vector<uint32_t> Indices, std::vector<int> Labels, LabelTable& Table)
{
    Geometry n;

    for (const auto& lab : Labels) {
        AddLabelToTable(Table, lab);
    }
    GenerateColorFromLabels(Table);

    n.Data = ffNewArray(Indices.size(), sizeof(Vertex));

    size_t size = Indices.size();
    Vertex *ptr = (Vertex *)n.Data.Data;
    for (size_t i = 0; i < size; ++i) {

        ptr[i].x = Vertices[Indices[i] * 3 + 0];
        ptr[i].y = Vertices[Indices[i] * 3 + 1];
        ptr[i].z = Vertices[Indices[i] * 3 + 2];
        const Color& c = GetColor(Table, Labels[i]);
        ptr[i].r = c.r;
        ptr[i].g = c.g;
        ptr[i].b = c.b;
        ptr[i].a = c.a;
    }
    return n;
}

static glm::vec2 IsoValue(glm::vec2 T[3], glm::vec2 BarycentricPoint)
{
    glm::mat2 mat(glm::vec2(T[1].x - T[0].x, T[1].y - T[0].y), glm::vec2(T[2].x - T[0].x, T[2].y - T[0].y));

    return mat * BarycentricPoint + T[0];
}

Geometry ConstructIsoScalar(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float> Values, std::vector<float>& RefTriangle, std::vector<float>& KSub, float min, float max) {
    size_t nsubT = KSub.size() / 3;
    size_t nsubV = RefTriangle.size() / 2;
    size_t nK = Values.size() / (Indices.size() / 3);
    std::vector<glm::vec2> Pn(nsubV);
    size_t count = 0;
    int o = 0;

    Geometry n;
    n.Data = ffNewArray((Indices.size() / 3) * nsubT * 3, sizeof(Vertex));

    Vertex *ptr = (Vertex *)n.Data.Data;

    for (size_t i = 0; i < (Indices.size() / 3); ++i) {
        glm::vec2 triangle[3] = {
            glm::vec2(Vertices[Indices[i * 3] * 3 + 0], Vertices[Indices[i * 3] * 3 + 1]),
            glm::vec2(Vertices[Indices[i * 3 + 1] * 3 + 0], Vertices[Indices[i * 3 + 1] * 3 + 1]),
            glm::vec2(Vertices[Indices[i * 3 + 2] * 3 + 0], Vertices[Indices[i * 3 + 2] * 3 + 1])
        };
        for (size_t j = 0; j < nsubV; ++j) {
            Pn[j] = IsoValue(triangle, glm::vec2(RefTriangle[j * 2], RefTriangle[j * 2 + 1]));
        }
        for (size_t sk = 0; sk < nsubT; ++sk) {
            int i0 = KSub[sk * 3 + 0];
            int i1 = KSub[sk * 3 + 1];
            int i2 = KSub[sk * 3 + 2];

            glm::vec3 ff = glm::vec3(Values[o + i0], Values[o + i1], Values[o + i2]);
            glm::vec2 Pt[3] = {
                Pn[i0], Pn[i1], Pn[i2]
            };

            for (size_t k = 0; k < 3; ++k) {
                ptr[count].x = Pt[k].x;
                ptr[count].y = Pt[k].y;
                ptr[count].z = 0.f;
                ptr[count].r = 0.5f;
                ptr[count].g = (ff[k] - min) / (max - min);
                ptr[count].b = 0.5f;
                ptr[count].a = 1.0f;
                count += 1;
            }
        }
        o += nK;
    }
    return n;
}

Geometry ConstructIsoVector(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float> Values, std::vector<float>& RefTriangle, std::vector<float>& KSub, float min, float max) {
    size_t nsubT = KSub.size() / 3;
    size_t nsubV = RefTriangle.size() / 2;
    size_t nK = Values.size() / (Indices.size() / 3);
    std::vector<glm::vec2> Pn(nsubV);
    size_t count = 0;
    int o = 0;

    Geometry n;
    n.Data = ffNewArray(Vertices.size() * nsubT * 2, sizeof(Vertex));

    for (size_t i = 0; i < Values.size() / 2; ++i) {
        min = std::min(min, sqrtf(Values[i * 2] * Values[i * 2] + Values[i * 2 + 1] * Values[i * 2 + 1]));
        max = std::max(max, sqrtf(Values[i * 2] * Values[i * 2] + Values[i * 2 + 1] * Values[i * 2 + 1]));
    }

    Vertex *ptr = (Vertex *)n.Data.Data;
    for (size_t i = 0; i < (Indices.size() / 3); ++i) {
        for (size_t j = 0; j < nsubV; ++j) {
            glm::vec2 triangle[3] = {
                glm::vec2(Vertices[Indices[i * 3] * 3 + 0], Vertices[Indices[i * 3] * 3 + 1]),
                glm::vec2(Vertices[Indices[i * 3 + 1] * 3 + 0], Vertices[Indices[i * 3 + 1] * 3 + 1]),
                glm::vec2(Vertices[Indices[i * 3 + 2] * 3 + 0], Vertices[Indices[i * 3 + 2] * 3 + 1])
            };
            Pn[j] = IsoValue(triangle, glm::vec2(RefTriangle[j * 2], RefTriangle[j * 2 + 1]));
        }
        for (size_t k = 0, l = 0; k < nsubV; ++k) {
            glm::vec2 P = Pn[k];
            glm::vec2 uv(Values[o + l], Values[o + l + 1]);
            float tmp = (sqrtf(uv.x * uv.x + uv.y * uv.y) - min) / (max - min);
            l += 2;

            ptr[count].x = P.x;
            ptr[count].y = P.y;
            ptr[count].z = 0.f;
            ptr[count].r = 1.0f;
            ptr[count].g = tmp;
            ptr[count].b = 0.5f;
            ptr[count].a = 0.5f;
            count += 1;

            ptr[count].x = P.x + (uv.x / max) * (max - min);
            ptr[count].y = P.y + (uv.y / max) * (max - min);
            ptr[count].z = 0.f;
            ptr[count].r = 1.0f;
            ptr[count].g = tmp;
            ptr[count].b = 0.5f;
            ptr[count].a = 0.5f;
            count += 1;
        }
        o += nK;
    }
    return n;
}

void ImportGeometry(json GeoJSON, ThreadSafeQueue *Queue, uint16_t PlotID)
{
    LabelTable Table;

    std::string GeoType = GeoJSON["Type"].get<std::string>();
    uint16_t MeshID = GeoJSON["Id"].get<uint16_t>();
    ConstructedGeometry Data(PlotID, MeshID);

    std::vector<float> Vertices = GeoJSON["Vertices"].get<std::vector<float>>();
    std::vector<uint32_t> Indices = GeoJSON["MeshIndices"].get<std::vector<uint32_t>>();
    std::vector<int> Labels = GeoJSON["MeshLabels"].get<std::vector<int>>();

    Data.Geo = ConstructGeometry(Vertices, Indices, Labels, Table);
    if (Data.Geo.Data.Data == 0) {
        LogWarning("AsyncImport", "Failed to import mesh.");
    } else {
        Data.Geo.Description.PrimitiveTopology = GetMainPrimitiveTopology(GeoType);
        Data.Geo.Description.PolygonMode = GEO_POLYGON_MODE_LINE;
        Data.Geo.Type = GetTypeValue(GeoType.c_str());
        Queue->push(Data);
    }

    bool AsIsoValues = GeoJSON["IsoValues"].get<bool>();
    if (AsIsoValues) {
        std::cout << "Importing " << GeoJSON["IsoArray"].size() << " iso values.\n";
        for (auto& Isos : GeoJSON["IsoArray"]) {
            ConstructedGeometry IsoValues(PlotID, MeshID);

            std::vector<float> values = Isos["IsoV1"].get<std::vector<float>>();
            std::vector<float> ksub = Isos["IsoKSub"].get<std::vector<float>>();
            std::vector<float> referencetriangle = Isos["IsoPSub"].get<std::vector<float>>();
            bool IsoAsVector = Isos["IsoVector"].get<bool>();

            if (IsoAsVector) {
                std::cout << "\tVectors.\n";
                IsoValues.Geo = ConstructIsoVector(Vertices, Indices, values, referencetriangle, ksub, Isos["IsoMin"].get<float>(), Isos["IsoMax"].get<float>());
                IsoValues.Geo.Type = GetTypeValue("Vector2D");
                IsoValues.Geo.Description.PrimitiveTopology = GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_LINE_LIST;
            } else {
                std::cout << "\tScalars.\n";
                //IsoValues.Geo = ConstructIsoScalar(Vertices, Indices, values, referencetriangle, ksub, Isos["IsoMin"].get<float>(), Isos["IsoMax"].get<float>());
                IsoValues.Geo = ConstructIsoLines(Vertices, Indices, values, referencetriangle, ksub, Isos["IsoMin"].get<float>(), Isos["IsoMax"].get<float>());
                IsoValues.Geo.Description.PrimitiveTopology = GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_LINE_LIST;
                IsoValues.Geo.Type = GetTypeValue("Curve2D");
            }
            IsoValues.Geo.Description.PolygonMode = GEO_POLYGON_MODE_LINE;
            Queue->push(IsoValues);
            values.clear();
        }
    }

    bool AsBorder = GeoJSON["Borders"].get<bool>();
    if (AsBorder) {
        std::cout << "Import border.\n";
        ConstructedGeometry Border(PlotID, MeshID);

        Indices.clear();
        Labels.clear();

        std::vector<uint32_t> Indices = GeoJSON["BorderIndices"].get<std::vector<uint32_t>>();
        std::vector<int> Labels = GeoJSON["BorderLabels"].get<std::vector<int>>();

        Border.Geo = ConstructBorder(Vertices, Indices, Labels, Table);

        if (Border.Geo.Data.Data == 0) {
            LogWarning("AsyncImport", "Failed to import border.");
        } else {
            Border.Geo.Description.PrimitiveTopology = GetBorderPrimitiveTopology(GeoType);
            Border.Geo.Description.PolygonMode = GEO_POLYGON_MODE_LINE;
            Border.Geo.Type = GetTypeValue(((Border.Geo.Description.PrimitiveTopology == GEO_PRIMITIVE_TOPOLOGY_LINE_LIST) ? "Curve2D" : "Mesh3D"));
            Queue->push(Border);
        }
    }
    std::cout << "Finished importing data.\n";
}

void AsyncImport(std::string CompressedJSON, ThreadSafeQueue& Queue)
{
    json j = json::from_cbor(CompressedJSON);
    uint16_t PlotID = j["Plot"].get<uint16_t>();

    for (auto & Geometry : j["Geometry"]) {
        //ImportGeometry(Geometry, &Queue, PlotID);
        std::async(std::launch::async, ImportGeometry, Geometry, &Queue, PlotID);
    }
}

}    // namespace JSON
}    // namespace ffGraph