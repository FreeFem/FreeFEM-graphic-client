#ifndef DESERIALIZER_H_
#define DESERIALIZER_H_

#include <vector>
#include <string>
#include <cstdint>
#include "Array.h"
#include "LabelTable.h"

namespace ffGraph {
namespace JSON {

enum Type { Mesh2D, Mesh3D };

enum RenderType { Point = 0, Line = 1, Triangle = 3, Arrow = 0 };

enum GeometryType { Volume = 0, Surface = 1 };

struct SceneObject {
    Type DataType;
    RenderType RenderPrimitive;
    GeometryType GeoType;
    std::vector<Array> Data;
    int LineWidth = 2;
    bool Mergable = true;
};

struct SceneLayout {
    std::vector<SceneObject> MeshArrays;
    LabelTable ColorTable;
};

SceneLayout JSONString_to_SceneLayout(std::string& JSON_Data);

void SceneLayout_Add_SceneObject(SceneLayout& Layout, SceneObject& Obj);

void LogSceneLayout(const SceneLayout& Layout);

void DestroySceneLayout(SceneLayout& Layout);

}    // namespace JSON
}    // namespace ffGraph

#endif    // DESERIALIZER_H_