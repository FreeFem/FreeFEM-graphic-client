#include <nlohmann/json.hpp>
#include <fstream>
#include "deserializer.h"
#include "Import.h"
#include "Logger.h"

namespace ffGraph {
namespace JSON {

using json = nlohmann::json;

static RenderType GetRenderType(std::string PrimitiveType)
{
    if (PrimitiveType.compare("Line") == 0)
        return RenderType::Line;
    else if (PrimitiveType.compare("Triangle") == 0)
        return RenderType::Triangle;
    else if (PrimitiveType.compare("Tetrahedron") == 0)
        return RenderType::Triangle;
    return RenderType::Line;
}

static Array ChooseImportFunction(std::string PrimitiveType, std::string Type, json Geometry, LabelTable& Table)
{
    if (Type.compare("Mesh2D") == 0) {
        if (PrimitiveType.compare("Line") == 0) {
            LogInfo("ChooseImportFunction", "Importing a 2D mesh (Line)");
            return ImportLineMesh2D(Geometry, Table);
        } else if (PrimitiveType.compare("Triangle") == 0) {
            LogInfo("ChooseImportFunction", "Importing a 2D mesh (Triangle)");
            return ImportTriangleMesh2D(Geometry, Table);
        } else if (PrimitiveType.compare("Point") == 0) {
            LogWarning("Import function", "We don't support the Point primitive type yet.");
            return {0, 0, 0};
        } else if (PrimitiveType.compare("Arrow") == 0) {
            LogWarning("Import function", "We don't support the Arrow primitive type yet.");
            return {0, 0, 0};
        }
    } else {
        if (PrimitiveType.compare("Line") == 0) {
            LogInfo("ChooseImportFunction", "Importing a 3D mesh (Line)");
            return ImportLineMesh3D(Geometry, Table);
        } else if (PrimitiveType.compare("Triangle") == 0) {
            LogInfo("ChooseImportFunction", "Importing a 3D mesh (Triangle)");
            return ImportTriangleMesh3D(Geometry, Table);
        }
    }
    LogWarning("ChooseImportFunction", "Didn't find any suitable function.");
    return {0, 0, 0};
}

static bool JSONObject_to_SceneObject(SceneObject& Obj, LabelTable& Table, json JSONObj, int LineWidth)
{
    std::string PrimitiveType = JSONObj["Primitive"];
    std::string Type = JSONObj["Type"];
    std::string GeoType = JSONObj["GeometryType"];

    Obj.DataType = (Type.compare("Mesh2D") == 0) ? Type::Mesh2D : Type::Mesh3D;
    Obj.LineWidth = LineWidth;
    Obj.RenderPrimitive = GetRenderType(PrimitiveType);
    Obj.GeoType = (GeoType.compare("Surface") == 0) ? GeometryType::Surface : GeometryType::Volume;

    Array r = ChooseImportFunction(PrimitiveType, Type, JSONObj, Table);

    if (r.Data == 0)
        return false;

    Obj.Data.push_back((Array){0, 0, 0});
    (*(Obj.Data.end() - 1)) = r;
    return true;
}

SceneLayout JSONString_to_SceneLayout(std::string& JSON_Data) {
    SceneLayout Layout;
    json Data = json::from_cbor(JSON_Data);

    Layout.MeshArrays.resize(Data["Geometry"].size());
    int i = 0;
    for (auto& Geometry : Data["Geometry"]) {
        if (!JSONObject_to_SceneObject(Layout.MeshArrays[i], Layout.ColorTable, Geometry, 2.f)) {
            LogWarning("JSONString_to_SceneLayout", "Failed to import some geometry.");
        }
        ++i;
    }

    return Layout;
}

void DestroySceneLayout(SceneLayout& Layout) {
    for (auto& obj : Layout.MeshArrays) {
        for (auto array : obj.Data) {
            DestroyArray(array);
        }
    }
}

}    // namespace JSON
}    // namespace ffGraph