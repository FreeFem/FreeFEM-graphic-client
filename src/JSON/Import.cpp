#include <nlohmann/json.hpp>
#include <fstream>
#include <string>
#include <future>
#include <glm/glm.hpp>
#include "Logger.h"
#include "Import.h"

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

glm::vec2 IsoValue(glm::vec2 T[3], glm::vec2 BarycentricPoint)
{
    glm::mat2 mat(glm::vec2(T[1].x - T[0].x, T[1].y - T[0].y), glm::vec2(T[2].x - T[0].x, T[2].y - T[0].y));

    return mat * BarycentricPoint + T[0];
}

static uint8_t SubPx[13] = {
    0, 5, 4,
    5, 1, 3,
    3, 4, 2,
    3, 4, 5
};

size_t GetPx(size_t RefTrianglePointCount)
{
    if (RefTrianglePointCount == 3)
        return 3;
    else if (RefTrianglePointCount == 6)
        return 12;
    return 3;
}

Geometry ConstructIsoMeshPX(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float> Values, std::vector<float>& RefTriangle)
{
    size_t P = GetPx(RefTriangle.size() / 2LU);
    size_t Size = P * (Indices.size() / 3LU);
    float min, max;

    Geometry n;

    n.Data = ffNewArray(Size, sizeof(Vertex));

    glm::vec2 Triangle[3];
    std::vector<float> Position(RefTriangle.size());
    Vertex *ptr = (Vertex *)n.Data.Data;

    //std::vector<float> V(Size);
    min = max = Values[0];
    // for (size_t i = 0; Indices.size(); ++i) {

    // }
    std::cout << Size / 2LU << "\n";
    for (size_t i = 0; i < Indices.size(); ++i) {
        std::cout << "Indices[" << i << "] = " << Indices[i] << "\n";
        std::cout << "\tVertices[" << Indices[i] << "] = " << Vertices[Indices[i] * 3] << " " << Vertices[Indices[i] * 3 + 1] << "\n";
    }
    for (size_t i = 0; i < Values.size(); ++i) {
        std::cout << "Value[" << i << "] = " << Values[i] << "\n";
        min = std::min(min, Values[i]);
        max = std::max(max, Values[i]);
    }
    for (size_t i = 0; i < Indices.size() / 3LU; ++i) {
        std::vector<float> tmpVal(P / 2LU);
        Triangle[0].x = Vertices[Indices[i * 3] * 3];
        Triangle[0].y = Vertices[Indices[i * 3] * 3 + 1];
        for (size_t j = 0; j < P / 2LU; ++j)
            tmpVal[j] = Values[Indices[i * 3] + j];

        Triangle[1].x = Vertices[Indices[i * 3 + 1] * 3];
        Triangle[1].y = Vertices[Indices[i * 3 + 1] * 3 + 1];

        Triangle[2].x = Vertices[Indices[i * 3 + 2] * 3];
        Triangle[2].y = Vertices[Indices[i * 3 + 2] * 3 + 1];

        for (size_t j = 0; j < RefTriangle.size() / 2LU; ++j) {
            glm::vec2 res = IsoValue(Triangle, glm::vec2(RefTriangle[j * 2], RefTriangle[j * 2 + 1]));

            Position[j * 2] = res.x;
            Position[j * 2 + 1] = res.y;
        }
        Color c;
        float H = 0;
        for (size_t j = 0; j < P / 3; ++j) {
            int count = 0;
            ptr[i * P + j * 3].x = Position[SubPx[j * 3] * 2];
            ptr[i * P + j * 3].y = Position[SubPx[j * 3] * 2 + 1];
            ptr[i * P + j * 3].z = 0.f;
            H = 330.f * (tmpVal[count] - min) / (max - min);
            ++count;
            c = NewColor(H, 1.f, 1.f);
            ptr[i * P + j * 3].r = c.r;
            ptr[i * P + j * 3].g = c.g;
            ptr[i * P + j * 3].b = c.b;
            ptr[i * P + j * 3].a = c.a;

            // --------------------------------------------------------------
            ptr[i * P + j * 3 + 1].x = Position[SubPx[j * 3 + 1] * 2];
            ptr[i * P + j * 3 + 1].y = Position[SubPx[j * 3 + 1] * 2 + 1];
            ptr[i * P + j * 3 + 1].z = 0.f;
            H = 330.f * (tmpVal[count] - min) / (max - min);
            ++count;
            c = NewColor(H, 1.f, 1.f);
            ptr[i * P + j * 3 + 1].r = c.r;
            ptr[i * P + j * 3 + 1].g = c.g;
            ptr[i * P + j * 3 + 1].b = c.b;
            ptr[i * P + j * 3 + 1].a = c.a;

            // --------------------------------------------------------------
            ptr[i * P + j * 3 + 2].x = Position[SubPx[j * 3 + 2] * 2];
            ptr[i * P + j * 3 + 2].y = Position[SubPx[j * 3 + 2] * 2 + 1];
            ptr[i * P + j * 3 + 2].z = 0.f;
            H = 330.f * (tmpVal[count] - min) / (max - min);
            ++count;
            c = NewColor(H, 1.f, 1.f);
            ptr[i * P + j * 3 + 2].r = c.r;
            ptr[i * P + j * 3 + 2].g = c.g;
            ptr[i * P + j * 3 + 2].b = c.b;
            ptr[i * P + j * 3 + 2].a = c.a;
        }
    }
    return n;
}

Geometry ConstructIsoMeshVector(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float>& Values)
{
    Geometry n;
    float min, max;

    n.Data = ffNewArray((Vertices.size() / 3LU) * 2LU, sizeof(Vertex));

    size_t size = Vertices.size() / 3LU;
    Vertex *ptr = (Vertex *)n.Data.Data;
    float *tmp = (float *)malloc(sizeof(float) * (Values.size() / 2LU));

    if (tmp == 0) {
        std::cout << "Failed to alloc temporary memory.\n";
        assert(0);
    }
    for (size_t i = 0; i < (Values.size() / 2LU); ++i) {
        tmp[i] = sqrtf(Values[i * 2] * Values[i * 2] + Values[i * 2 + 1] * Values[i * 2 + 1]);
    }
    min = max = tmp[0];
    for (size_t i = 0; i < Values.size() / 2LU; ++i) {
        min = std::min(min, tmp[i]);
        max = std::max(max, tmp[i]);
    }
    for (size_t i = 0; i < size; ++i) {

        ptr[i * 2].x = Vertices[i * 3 + 0];
        ptr[i * 2].y = Vertices[i * 3 + 1];
        ptr[i * 2].z = Vertices[i * 3 + 2];
        float H = 179.f * (tmp[i] - min) / (max - min);
        const Color& c = NewColor(H, 1.f, 1.f);
        ptr[i * 2].r = c.r;
        ptr[i * 2].g = c.g;
        ptr[i * 2].b = c.b;
        ptr[i * 2].a = c.a;

        ptr[i * 2 + 1].x = Vertices[i * 3 + 0] + (Values[i * 2] / max) * 0.09f;
        ptr[i * 2 + 1].y = Vertices[i * 3 + 1] + (Values[i * 2 + 1] / max) * 0.09f;
        ptr[i * 2 + 1].z = Vertices[i * 3 + 2];

        ptr[i * 2 + 1].r = c.r;
        ptr[i * 2 + 1].g = c.g;
        ptr[i * 2 + 1].b = c.b;
        ptr[i * 2 + 1].a = c.a;
    }
    free(tmp);
    return n;
}

Geometry ConstructIsoMesh(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float>& Values)
{
    Geometry n;
    float min, max = 0;

    n.Data = ffNewArray(Indices.size(), sizeof(Vertex));

    size_t size = Indices.size();
    Vertex *ptr = (Vertex *)n.Data.Data;
    std::vector<float> tmp(Indices.size());

    for (size_t i = 0; i < tmp.size(); ++i) {
        std::cout << Indices[i] << " " << Values[Indices[i]] << "\n";
        tmp[i] = Values[Indices[i]];
    }
    for (size_t i = 0; i < tmp.size(); ++i) {
        min = std::min(min, tmp[i]);
        max = std::max(max, tmp[i]);
    }
    for (size_t i = 0; i < size; ++i) {

        ptr[i].x = Vertices[Indices[i] * 3 + 0];
        ptr[i].y = Vertices[Indices[i] * 3 + 1];
        ptr[i].z = Vertices[Indices[i] * 3 + 2];
        float H = 330.f * (tmp[i] - min) / (max - min);
        const Color& c = NewColor(H, 1.f, 1.f);
        ptr[i].r = c.r;
        ptr[i].g = c.g;
        ptr[i].b = c.b;
        ptr[i].a = c.a;
    }
    return n;
}

Geometry ConstructIsoScalar(std::vector<float>& Vertices, std::vector<uint32_t>& Indices, std::vector<float> Values, std::vector<float>& RefTriangle, std::vector<float>& KSub, float min, float max) {
    size_t nsubT = KSub.size() / 3;
    size_t nsubV = RefTriangle.size() / 2;
    std::cout << "nsubV : " << nsubV << "\n";
    size_t nK = Values.size() / (Indices.size() / 3);
    std::vector<glm::vec2> Pn(nsubV);
    size_t count = 0;
    int o = 0;

    Geometry n;
    n.Data = ffNewArray((Indices.size() / 3) * nsubT * 3, sizeof(Vertex));
    std::cout << "nsubT : " << nsubT << "\n";

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
                ptr[count].r = 0.f;
                ptr[count].g = (ff[k] - min) / (max - min);
                ptr[count].b = 0.f;
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
        std::cout << "Triangle N" << i << "\n";
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
            l += 2;

            ptr[count].x = P.x;
            ptr[count].y = P.y;
            ptr[count].z = 0.f;
            ptr[count].r = 1.f;
            ptr[count].g = 0.f;
            ptr[count].b = 0.f;
            ptr[count].a = 0.f;
            count += 1;

            std::cout << (sqrtf(uv.x * uv.x + uv.y * uv.y) / max) << "\n";
            ptr[count].x = P.x + (uv.x / max) * 0.09f;
            ptr[count].y = P.y + (uv.y / max) * 0.09f;
            ptr[count].z = 0.f;
            ptr[count].r = 1.f;
            ptr[count].g = 0.f;
            ptr[count].b = 0.f;
            ptr[count].a = 0.f;
            count += 1;
        }
        o += nK;
    }
    std::cout << "Count = " << count << "\n";
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
                IsoValues.Geo = ConstructIsoScalar(Vertices, Indices, values, referencetriangle, ksub, Isos["IsoMin"].get<float>(), Isos["IsoMax"].get<float>());
                IsoValues.Geo.Description.PrimitiveTopology = GeometryPrimitiveTopology::GEO_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
                IsoValues.Geo.Type = GetTypeValue("Mesh2D");
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
            //Queue->push(Border);
        }
    }
    std::cout << "Finished importing data.\n";
}

void AsyncImport(std::string CompressedJSON, ThreadSafeQueue& Queue)
{
    json j = json::from_cbor(CompressedJSON);

    uint16_t PlotID = j["Plot"].get<uint16_t>();

    for (auto & Geometry : j["Geometry"]) {
        ImportGeometry(Geometry, &Queue, PlotID);
        //std::async(std::launch::async, ImportGeometry, Geometry, &Queue, PlotID);
    }
}

}    // namespace JSON
}    // namespace ffGraph