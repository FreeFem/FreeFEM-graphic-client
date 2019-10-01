#include <nlohmann/json.hpp>
#include <fstream>
#include "deserializer.h"
#include "Logger.h"

namespace ffGraph {
namespace JSON {

using json = nlohmann::json;

static SceneObject JSONObject3D_to_SceneObject(json JSONObj) {
    SceneObject SceneObj;

    SceneObj.DataType = Type::Mesh3D;

    std::string ObjRenderType = JSONObj["Primitive"];
    if (ObjRenderType.compare("Line") == 0)
        SceneObj.RenderPrimitive = RenderType::Line;
    else if (ObjRenderType.compare("Triangle") == 0)
        SceneObj.RenderPrimitive = RenderType::Triangle;

    std::vector<float> vertices = JSONObj["Vertices"].get<std::vector<float>>( );
    Array tmp = ffNewArray(vertices.size( ), sizeof(float));

    ffMemcpyArray(tmp, vertices.data( ));

    SceneObj.Data.push_back(tmp);

    return SceneObj;
}

static SceneObject JSONObject_to_SceneObject(json JSONObj, int LineWidth) {
    SceneObject SceneObj;

    std::string ObjType = JSONObj["Type"];

    SceneObj.LineWidth = LineWidth;
    if (ObjType.compare("Mesh2D") == 0)
        SceneObj.DataType = Type::Mesh2D;
    else
        return JSONObject3D_to_SceneObject(JSONObj);

    std::string ObjRenderType = JSONObj["Primitive"];

    if (ObjRenderType.compare("Line") == 0)
        SceneObj.RenderPrimitive = RenderType::Line;
    else if (ObjRenderType.compare("Triangle") == 0)
        SceneObj.RenderPrimitive = RenderType::Triangle;

    std::vector<unsigned long int> indices = JSONObj["Indices"].get<std::vector<unsigned long int>>( );

    std::vector<float> vertices = JSONObj["Vertices"].get<std::vector<float>>( );
    Array tmp = ffNewArray(indices.size( ), sizeof(float) * 7);

    int count = 0;
    float* ptr = (float*)tmp.Data;
    for (unsigned int index : indices) {
        for (int i = 0; i < 7; ++i) {
            ptr[count] = vertices[index * 7 + i];
            ++count;
        }
    }
    SceneObj.Data.push_back(tmp);

    return SceneObj;
}

static void SceneObject_Add_JSONObject(SceneObject& Obj, json JSONObj) {
    std::vector<unsigned long int> indices = JSONObj["Indices"].get<std::vector<unsigned long int>>( );

    std::vector<float> vertices = JSONObj["Vertices"].get<std::vector<float>>( );
    Array tmp = ffNewArray(indices.size( ), sizeof(float) * 7);

    int count = 0;
    float* ptr = (float*)tmp.Data;
    for (unsigned int index : indices) {
        for (int i = 0; i < 7; ++i) {
            ptr[count] = vertices[index * 7 + i];
            ++count;
        }
    }
    Obj.Data.push_back(tmp);
}

int FindRightSceneObject(std::vector<SceneObject> Objects, json JSONObj) {
    int DataType = 0;
    int RenderPrimitive;

    std::string ObjType = JSONObj["Type"];

    if (ObjType.compare("Mesh2D") == 0)
        DataType = Type::Mesh2D;
    else
        DataType = Type::Mesh3D;

    std::string ObjRenderType = JSONObj["Primitive"];

    if (ObjRenderType.compare("Line") == 0)
        RenderPrimitive = RenderType::Line;
    else if (ObjRenderType.compare("Triangle") == 0)
        RenderPrimitive = RenderType::Triangle;

    for (int ite = 0; ite < (int)Objects.size( ); ++ite) {
        if (Objects[ite].DataType == DataType && Objects[ite].RenderPrimitive == RenderPrimitive) return ite;
    }
    return -1;
}

json GetAxes( ) {
    std::ifstream i("shaders/grid.json");
    std::string data;
    i >> data;

    return json::parse(data);
}

void SceneLayout_Add_SceneObject(SceneLayout& Layout, SceneObject Obj) { Layout.MeshArrays.push_back(Obj); }

SceneLayout JSONString_to_SceneLayout(std::string& JSON_Data) {
    SceneLayout Layout;
    json Data = json::from_cbor(JSON_Data);
    json Axes = GetAxes( );
    Layout.MeshArrays.push_back(JSONObject_to_SceneObject(Axes, 1));
    for (auto& Geometry : Data["Geometry"]) {
        // int ite = FindRightSceneObject(Layout.MeshArrays, Geometry);

        // if (ite != -1) {
        //     SceneObject_Add_JSONObject(Layout.MeshArrays[ite], Geometry);
        // } else {
        //     Layout.MeshArrays.push_back(JSONObject_to_SceneObject(Geometry));
        // }
        Layout.MeshArrays.push_back(JSONObject_to_SceneObject(Geometry, 2));
    }

    return Layout;
}

static std::string GetRenderTypeName(RenderType r) {
    if (r == RenderType::Line)
        return std::string("Line");
    else if (r == RenderType::Triangle)
        return std::string("Triangle");
    return std::string("");
}

void LogSceneLayout(const SceneLayout& Layout) {
    std::string message("\nScene Layout :\n");

    message.append(std::to_string(Layout.MeshArrays.size( )));
    message.append(" Distinct group of objects :\n");
    int i = 0;
    for (auto& obj : Layout.MeshArrays) {
        message.append(std::string("\tGroup ") + std::to_string(i) + std::string(" with ") +
                       std::to_string(obj.Data.size( )) + std::string(" geometry "));
        message.append(std::string("[") + GetRenderTypeName(obj.RenderPrimitive) + std::string("] ") +
                       std::string(":\n"));
        int j = 0;
        for (auto& Geo : obj.Data) {
            message.append(std::string("\t\tGeometry " + std::to_string(j) + std::string(" has ") +
                                       std::to_string(Geo.ElementCount) + std::string(" vertices.\n")));
            ++j;
        }
        message.append(std::string("\n"));
        ++i;
    }
    LogInfo(std::string("SceneLayout Log Function"), "%s", message.c_str( ));
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