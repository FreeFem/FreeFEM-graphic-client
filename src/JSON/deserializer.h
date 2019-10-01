#ifndef DESERIALIZER_H_
#define DESERIALIZER_H_

#include <vector>
#include <string>
#include <cstdint>
#include "Array.h"

namespace ffGraph {
namespace JSON {

enum Type { Mesh2D, Mesh3D };

enum RenderType { Point = 0, Line = 1, Triangle = 3, Arrow = 0 };

struct SceneObject {
    Type DataType;
    RenderType RenderPrimitive;
    std::vector<Array> Data;
    int LineWidth = 2;
};

struct SceneLayout {
    std::vector<SceneObject> MeshArrays;
};

SceneLayout JSONString_to_SceneLayout(std::string& JSON_Data);

void SceneLayout_Add_SceneObject(SceneLayout& Layout, SceneObject& Obj);

void LogSceneLayout(const SceneLayout& Layout);

void DestroySceneLayout(SceneLayout& Layout);

}    // namespace JSON
}    // namespace ffGraph

#endif    // DESERIALIZER_H_